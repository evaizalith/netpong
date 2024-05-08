#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include "network.h"
#include "bounce.h"

int CLIENT_RUNNING = 1;
int iteration = 0;
int numBalls = 3;

void talkToServer(int fd) {
    static char buf[32];
    static char* response[5];
    
    if (iteration == 0) {
        char helo[] = "HELO 1.0 20 16 Emma\n";
        send(fd, helo ,strlen(helo), 0);
        printf("Sent: %s\n", helo);
    } else {
        // Parse requests
        if (strcmp(response[0], "NAME") == 0) {
            // Request serve ball
            char serve[] = "SERV 3\n";
            send(fd, serve, strlen(serve), 0);
            --numBalls;
            printf("Sent: %s\n", serve);
        }
        else if (strcmp(response[0], "DONE") == 0) {
            char quit[] = "QUIT let's play again soon\n";
            send(fd, quit, strlen(quit), 0);
            printf("Sent: %s\n", quit);
            exit(0);
        }
        // Main game functions
        else if (strcmp(response[0], "BALL") == 0) {
            receiveBall(atoi(response[1]), atoi(response[2]), atoi(response[3]), atoi(response[4]));
            int result = pongMain(); // Play game
        
            // Player quit
            if (result == -1) {
                char done[] = "DONE I give up";
                send(fd, done, strlen(done), 0);
            }
            // Ball returned
            else if (result == 0) {
                char ball[32];
                sGetBallState(ball);
                send(fd, ball, strlen(ball), 0);
            }
            // Ball missed
            else if (result == 1) {
                char miss[] = "MISS";
                send(fd, miss, strlen(miss), 0);
            }
        }
        else if (strcmp(response[0], "MISS") == 0) {
            if (numBalls > 1) {
                // Generate new ball if there are balls left 
                char ball[32];
                sNewBall(ball);
                --numBalls;
                send(fd, ball, strlen(ball), 0);
            } else {
                // If no new balls, end game
                char done[] = "DONE Nice game, I win\n";
                send(fd, done, strlen(done), 0); 
            }
        }
        else if (strcmp(response[0], "QUIT") == 0) {
            CLIENT_RUNNING = 0;
            return;
        }
    }

    printf("Listening...\n");
    recv(fd, buf, 32, 0);

    memset(response, 0, sizeof(response));
    printf("Received: %s\n", buf);

    // Parse response
    char* split = strtok(buf, " ");
    int i = 0;
    while(split != NULL) {
        response[i++] = split;
        split = strtok(NULL, " ");
    }


    ++iteration;
}

int main(int argc, char* argv[]) {
    int fd;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>", argv[0]);
        return 1;
    }


    while(CLIENT_RUNNING) {
        fd = connectToServer(argv[1], atoi(argv[2]));
        if (fd == -1) {
            fprintf(stderr, "Failed to connect to server\n");
            return 1;
        }

        talkToServer(fd);

        close(fd);
    }

}
