// Eva Powlison
// Systems Programming 53203
// March 7 2024

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <signal.h>
#include <string.h>
#include "bounce.h"
#include "paddle.h"

struct ppball ball;
struct pppaddle paddle = {10, 16, RIGHT_EDGE, '#'}; 

int score = 0;              // Number of times player has successfully hit the ball
int RUNNING = 1;            // Tracks whether the game is running or not
int missOrHit;              // 0 if ball returned, 1 if ball missed 

// Updates state of ball
void receiveBall(int y_pos, int x_ttm, int y_ttm, int y_dir) {
    ball.x_pos = LEFT_EDGE + 1;
    ball.y_pos = y_pos;
    ball.x_ttm = ball.x_ttg = x_ttm;
    ball.y_ttm = ball.y_ttg = y_ttm;
    ball.x_dir = 1;
    ball.y_dir = y_dir;
}

// Forms a new ball and returns it as a string in format for protocol
// String has form [net_position, xttm, yttm, ydir]
void sNewBall(char* string) {
    int y_pos = Y_INIT + (rand() % 10);
    int x_ttm = X_TTM + (rand() % 5);
    int y_ttm = Y_TTM + (rand() % 5);
    int y_dir = (rand() % 2) ? 1 : -1;

    sprintf(string, "BALL %d %d %d %d\n", y_pos, x_ttm, y_ttm, y_dir);
}

// Returns current state of ball as a string in format for protocol
// String has form [net_position, xttm, yttm, ydir]
void sGetBallState(char* string) {
    sprintf(string, "BALL %d %d %d %d\n", ball.y_pos, ball.x_ttm, ball.y_ttm, ball.y_dir);
}

// Create new ball with random position, speed, and direction
void resetBall() {
    ball.x_pos = X_INIT + (rand() % 10);
    ball.y_pos = Y_INIT + (rand() % 10);
    ball.x_ttm = ball.x_ttg = X_TTM + (rand() % 5);
    ball.y_ttm = ball.y_ttg = Y_TTM + (rand() % 5);
    ball.x_dir = (rand() % 2) ? 1 : -1; // Randomly selects 1 or -1
    ball.y_dir = (rand() % 2) ? 1 : -1;
}

int set_ticker(int n_msecs) {
    struct itimerval newTimeset;
    long n_sec, n_usecs;

    n_sec = n_msecs / 1000;
    n_usecs = (n_msecs % 1000) * 1000L;

    newTimeset.it_interval.tv_sec = n_sec; 
    newTimeset.it_interval.tv_usec = n_usecs;
    newTimeset.it_value.tv_sec = n_sec;
    newTimeset.it_value.tv_usec = n_usecs;

    return setitimer(ITIMER_REAL, &newTimeset, NULL);
}

// End game 
void wrapup() {
    set_ticker(0);
    endwin();
}

// Returns 0 on contact with walls, 1 on contact with paddle, -1 on contact with right border
int bounceOrLose(struct ppball *bp) {
    if (bp->y_pos == TOP_ROW) {
        bp->y_dir = 1;
        return 0;
    } else if (bp->y_pos == BOT_ROW) {
        bp->y_dir = -1;
        return 0;
    } else if (bp->x_pos == LEFT_EDGE) {
        bp->x_dir = 1;
        return 2;
    } else if (bp->x_pos == RIGHT_EDGE) {
        bp->x_dir = -1;
        if (paddle_contact(&paddle, RIGHT_EDGE, bp->y_pos) == 1) return 1;
        return -1;
    }

    return 0;
}

void moveBall(int signum) {
    int x_cur, y_cur, moved;

    signal(SIGALRM, SIG_IGN);
    x_cur = ball.x_pos;
    y_cur = ball.y_pos;
    moved = 0;

    paddle_init(&paddle);

    if (ball.x_ttm > 0 && ball.x_ttg-- == 1) {
        ball.x_pos += ball.x_dir;
        ball.x_ttg = ball.x_ttm;
        moved = 1;
    }

    if (ball.y_ttm > 0 && ball.y_ttg-- == 1) {
        ball.y_pos += ball.y_dir;
        ball.y_ttg = ball.y_ttm;
        moved = 1;
    }

    if (moved) {
        mvaddch(y_cur, x_cur, BLANK);
        mvaddch(y_cur, x_cur, BLANK);
        mvaddch(ball.y_pos, ball.x_pos, ball.symbol);
        int result = bounceOrLose(&ball);
        // Ball hits border
        if (result == -1) {
            RUNNING = 0;
            missOrHit = 1;
            set_ticker(0);
        }
        // Ball hits paddle
        if (result == 1) {
            ball.x_ttm += (rand() % 5);
            ball.y_ttm += (rand() % 5);
            ++score;
        }
        // Ball returns
        if (result == 2) {
            RUNNING = 0;
            missOrHit = 0;
            set_ticker(0);
        }
        move(LINES-1, COLS-1);
        refresh();
    }

    signal(SIGALRM, moveBall);

}

void setup() {
    ball.symbol = DFL_SYMBOL;

    initscr();
    noecho();
    crmode();

    // Floor
    for (int i = LEFT_EDGE; i < RIGHT_EDGE; i++) {
        mvaddch(BOT_ROW + 1, i, '-');
    }

    // Ceiling
    for (int i = LEFT_EDGE; i < RIGHT_EDGE; i++) {
        mvaddch(TOP_ROW - 1, i, '-');
    }

    // Left Wall
    for (int i = TOP_ROW - 1; i < BOT_ROW + 2; i++) {
        mvaddch(i, LEFT_EDGE - 1, '|');
    }

    signal (SIGINT, SIG_IGN);
    mvaddch(ball.y_pos, ball.x_pos, ball.symbol);
    refresh();

    signal (SIGALRM, moveBall);
    set_ticker(1000 / TICKS_PER_SEC);
}

// Returns -1 on quit, 0 on ball sent back, 1 on miss  
int pongMain() {
    RUNNING = 1;
    srand(getpid());
    setup();

    int c;
    while ((c = getchar()) != 'Q' && RUNNING == 1) {
        if (c == 'k') paddle_up(&paddle);
        if (c == 'j') paddle_down(&paddle);
    }

    wrapup();

    if (c == 'Q') return -1;

    return missOrHit;
}

