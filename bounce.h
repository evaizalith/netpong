#define BLANK ' '
#define DFL_SYMBOL 'o'
#define TOP_ROW 5
#define BOT_ROW 20
#define LEFT_EDGE 10
#define RIGHT_EDGE 70
#define X_INIT 10
#define Y_INIT 10
#define TICKS_PER_SEC 50
#define X_TTM 5
#define Y_TTM 8

struct ppball {
    int y_pos, x_pos, y_ttm, x_ttm, x_ttg, y_ttg, x_dir, y_dir;
    char symbol;
};

struct wall {
    int left_x;
    int right_x;
    int top_y;
    int bot_y;
};

// Library functions needed by server/client 
void receiveBall(int, int, int, int);
void sNewBall(char*); // Mutates the array passed to it
void sGetBallState(char*); // Mutates the array passed to it
int pongMain(); // Returns -1 on quit, 0 on ball returned, 1 on miss
