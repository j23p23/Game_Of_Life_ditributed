// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT     30464
#define MAXLINE 1024


struct message {
    short type_of_message; //0 for "HELLO" message, 1 for data,2 SIZE ,3 data
    int TABLICA[MAXLINE];
};

// Driver code
int main() {
    int WYMIAR_X = 0;
    int WYMIAR_Y = 0;
    int NR_KLIENTA = 0;
    int MAX_NR_KLIENTOW = 0;
    int sockfd;
    int ILOSC_ZYWYCH_STAN_POCZATKOWY = 0;
    char buffer[MAXLINE];
    char *ZYWE = "@";
    char *MARTWE = "-";
    struct message hello, data, update;

    char *receive = "REC";
    struct sockaddr_in servaddr;

    unsigned char my_string[MAXLINE];

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    hello.type_of_message = 0;

    sendto(sockfd, &hello, sizeof(struct message),
           MSG_CONFIRM, (const struct sockaddr *) &servaddr,
           sizeof(servaddr));
    printf("Hello message sent.\n");
    int n, len;

    for (;;) {


        n = recvfrom(sockfd, &data, sizeof(struct message),
                     MSG_WAITALL, (struct sockaddr *) &servaddr,
                     &len);

        //printf("Server : %s\n", data.content);

        if (data.type_of_message == 2) {
            int licznik3 = 0;

            printf("ODEBRANO WYMIARY \n");

                WYMIAR_X = data.TABLICA[0];
                WYMIAR_Y = data.TABLICA[1];
                ILOSC_ZYWYCH_STAN_POCZATKOWY = data.TABLICA[2];
                NR_KLIENTA = data.TABLICA[3];
                MAX_NR_KLIENTOW = data.TABLICA[4];

            printf("WYMIAR X %d \n", WYMIAR_X);

            printf("WYMIAR Y %d \n", WYMIAR_Y);
            printf("ILOSC ZYWYCH STAN POCZATKOWY %d \n", ILOSC_ZYWYCH_STAN_POCZATKOWY);
            if (MAX_NR_KLIENTOW==1) {
                printf("w SIECI JEST TYLKO 1 WEZEL BRAK SASIADOW \n");
            }
            else if ((NR_KLIENTA == 0 && MAX_NR_KLIENTOW > 1) || (NR_KLIENTA == MAX_NR_KLIENTOW-1 && MAX_NR_KLIENTOW!=1)) {
                printf("TEN WEZEL JEST WEZLEM GRANICZNYM POSIADA 1 SASIADA \n");
            }
            else {
                printf("TEN WEZEL JEST WEZLEM WEWNETRZNYM POSIADA 2 SASIADOW \n");
            }


        }

        //ODBIOR WSPOLRZEDNYCH

        if (data.type_of_message == 3) {

            char *TABLICA_GRY[WYMIAR_X+2][WYMIAR_Y];

            for (int k = 0; k < WYMIAR_X+2; k++) {
                for (int j = 0; j < WYMIAR_Y; j++) {
                    TABLICA_GRY[k][j] = MARTWE;
                }
            }
            int counter = 0;
            while (data.TABLICA[counter] >= 0) {
                int Xret = data.TABLICA[counter];
                int Yret = data.TABLICA[counter + 1];
                if(NR_KLIENTA == 0) TABLICA_GRY[Xret+1][Yret] = ZYWE;
                else TABLICA_GRY[Xret][Yret] = ZYWE;
                counter += 2;
            }
            printf("STATE OF FIELD BEFORE COMPUTING");
            printf("\n--------------------------------------------------------------\n");
            for (int i = 1; i < WYMIAR_X+1; i++) {
                printf("|");
                for (int j = 0; j < WYMIAR_Y; j++) {
                    printf(" %s ", TABLICA_GRY[i][j]);
                }
                printf("|");
                printf("\n");


            }

            printf("\n--------------------------------------------------------------\n");
            char *new_array[WYMIAR_X][WYMIAR_Y];
            for (int i = 1; i < WYMIAR_X+1; i++) {
                for (int j = 0; j < WYMIAR_Y; j++) {
                    int count_neighbours = 0;
                    for (int a = i - 1; a <= i + 1; a++) {
                        for (int b = j - 1; b <= j + 1; b++) {
                            if ((a == i && b == j) || (a < 0 || b < 0) || (a >= WYMIAR_X+2 || b >= WYMIAR_Y)) {
                                continue;
                            }
                            if (TABLICA_GRY[a][b] == ZYWE) {
                                count_neighbours++;
                            }
                        }
                    }
                    if ((TABLICA_GRY[i][j] == ZYWE) && (count_neighbours == 2 || count_neighbours == 3)) {
                        new_array[i - 1][j] = ZYWE;
                    }
                    else if ((TABLICA_GRY[i][j] == MARTWE) && (count_neighbours == 3)) {
                        new_array[i - 1][j] = ZYWE;
                    }
                    else new_array[i-1][j] = MARTWE;
                }

            }
            counter = 0;
            memset(update.TABLICA, -1, sizeof(update.TABLICA));
            printf("STATE OF FIELD AFTER COMPUTING");
            printf("\n--------------------------------------------------------------\n");
            for (int i = 0; i < WYMIAR_X; i++) {
                printf("|");
                for (int j = 0; j < WYMIAR_Y; j++) {
                    printf(" %s ", new_array[i][j]);

                    if (new_array[i][j] == ZYWE) {
                        update.TABLICA[counter] = i;
                        update.TABLICA[counter + 1] = j;
                        counter += 2;
                    }
                }
                printf("|");
                printf("\n");
            }
            printf("\n--------------------------------------------------------------\n");
        }

        if (data.type_of_message == 4){
            update.type_of_message = 4;
        sendto(sockfd, &update, sizeof(struct message),
               MSG_CONFIRM, (const struct sockaddr *) &servaddr,
               sizeof(servaddr));
     }
    }


    close(sockfd);


    return 0;
}
