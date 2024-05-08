// Eva Powlison
//
// Struct and functions for the paddle

struct pppaddle {
    int top;
    int bot;
    int col;
    char symbol;
};

void paddle_init(struct pppaddle *paddle);
void paddle_up(struct pppaddle *paddle);
void paddle_down(struct pppaddle *paddle);
int paddle_contact(struct pppaddle *paddle, int x, int y);
