// Eva Powlison
//
// Function definitions for the pong paddle

#include <curses.h>
#include "paddle.h"
#include "bounce.h"

void paddle_init(struct pppaddle *paddle) {

    // Clear column
    for(int i = TOP_ROW; i < BOT_ROW + 1; i++) {
        mvaddch(i, RIGHT_EDGE, ' ');
    }

    // Draw 
    for(int i = paddle->top; i < paddle->bot; i++) {
        mvaddch(i, RIGHT_EDGE, paddle->symbol);
    }
}

void paddle_up(struct pppaddle *paddle) {
    if (paddle->top > TOP_ROW) {
        paddle->top--;
        paddle->bot--;
    }
}

void paddle_down(struct pppaddle *paddle) {
    if (paddle->bot < BOT_ROW + 1) {
        paddle->bot++;
        paddle->top++;
    }
}

int paddle_contact(struct pppaddle *paddle, int x, int y) {
    for (int i = paddle->top; i < paddle->bot; i++) {
        if (y == i) return 1;
    }

    return 0;
}
