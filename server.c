#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include "network.h"
#include "bounce.h"

#define DEBUG 1

int SERVER_RUNNING = 1;
int numBalls; 

// Logs debug info to a file if debug flag is enabled
void debugLog(const char* type, const char* message) {
    if (DEBUG) {
        FILE *debug = fopen("server_log.txt", "a");
        if(debug != NULL) {
            fprintf(debug, "%s %s", type, message);
            fclose(debug);
        }
    }
}

void processRequest(char* req, int fd) {
    debugLog("Request: ", req);
    char* request[5];

    // Parse request
    // Requests should only ever contain at most 5 pieces of information delimited by a space
    char* split = strtok(req, " ");
    int i = 0;
    while(split != NULL) {
        request[i++] = split;
        split = strtok(NULL, " ");
    }

    sleep(1);

    // Handle requests
    if (strcmp(request[0], "HELO") == 0) {
        char response[] = "NAME 1.0 Eva\n\0";
        if (send(fd, response, strlen(response), 0) == -1)
            debugLog("Failed to send: ", response);
        else
            debugLog("Sent: ", response);
        

    } 
    else if (strcmp(request[0], "SERV") == 0) {
        numBalls = atoi(request[1]);
  
        char ball[32];
        sNewBall(ball);
        if(send(fd, ball, strlen(ball), 0) == -1) 
            debugLog("Sent: ", ball);
        else
            debugLog("Failed to send: ", ball);

    }
    // This request contains the main game functions
    else if (strcmp(request[0], "BALL") == 0) {
        receiveBall(atoi(request[1]), atoi(request[2]), atoi(request[3]), atoi(request[4]));
        int result = pongMain(); // Play game
        
        // Player quit
        if (result == -1) {
            char done[] = "DONE I give up";
            send(fd, done, strlen(done), 0);
            debugLog("Sent: ", done);
        }
        // Ball returned
        else if (result == 0) {
            char ball[32];
            sGetBallState(ball);
            send(fd, ball, strlen(ball), 0);
            debugLog("Sent: ", ball);
        }
        // Ball missed
        else if (result == 1) {
            char miss[] = "MISS";
            send(fd, miss, strlen(miss), 0);
            debugLog("Sent: ", miss);
        }
    } 
    else if (strcmp(request[0], "MISS") == 0) {
        if (numBalls > 1) {
            // Generate new ball if there are balls left 
            char ball[32];
            sNewBall(ball);
            --numBalls;
            if(send(fd, ball, strlen(ball), 0) == -1) 
                debugLog("Sent: ", ball);
            else
                debugLog("Failed to send: ", ball);
        } else {
            // If no new balls, end game
            char done[] = "DONE Nice game, I win\n";
            if (send(fd, done, strlen(done), 0) == -1) 
                debugLog("Sent: ", done);
            else
                debugLog("Failed to send: ", done);
        }
    }
    else if (strcmp(request[0], "DONE") == 0) {
        char quit[] = "QUIT let's play again soon\n";
        if (send(fd, quit, strlen(quit), 0) == -1)
            debugLog("Sent: ", quit);
        else
            debugLog("Failed to send: ", quit);
        SERVER_RUNNING = 0;
    }
    else if (strcmp(request[0], "QUIT") == 0) {
        debugLog("EXIT\n", "");
        SERVER_RUNNING = 0;
    }

}

int main (int argc, char* argv[]) {
    int sock, fd;
    FILE *file;
    char request[32];
    int bytes;

    if (argc == 1) {
        fprintf(stderr, "Pass port as argument\n");
        return 1;
    }

    sock = makeServerSocket(atoi(argv[1]), 1);
    if (sock == -1) {
        fprintf(stderr, "Failed to create socket\n");
        return 1;
    }
    
    // If debug logging is enabled, then clean the log file
    if (DEBUG) {
        fclose(fopen("server_log.txt", "w"));
    }

    printf("Server opened; now listening for client.\n");

    while(SERVER_RUNNING == 1) {
        printf("Listening...\n");
        fd = accept(sock, NULL, NULL);
        if (fd == -1) {
            fprintf(stderr, "Unable to accept connection\n");
            continue;
        }

        bytes = recv(fd, request, 32, 0);
        printf("Request = %s\n", request);

        if (bytes > 0) // if request is not empty
            processRequest(request, fd);

        memset(request, 0, sizeof(request));

        close(fd);
    }

    close(sock);
}
