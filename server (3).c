#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define PORT	 30464
#define MAXLINE 1024

struct message{
    short type_of_message; //0 for "HELLO" message, 1 for data, 2 for SIZE, 3 data, 4 TICK
    int TABLICA[MAXLINE];
};

// Driver code
int main() {
    unsigned char response[MAXLINE];
    int sockfd;
    char buffer[MAXLINE];
    struct message received_data,wysylka;
    char *hello = "HELLO";
    struct sockaddr_in servaddr, cliaddr;
    int lista_klientow[10];
    int maksymalna_liczba_klientow=10;
    int najwyzszy_deksryptor;
    fd_set readfds;
    int deskryptor;
    int aktywnosc_na_glownym_sockecie=0;
    struct sockaddr_in lista_adres[10];
    for(int i=0;i<maksymalna_liczba_klientow;i++)
    {
        lista_klientow[i]=0;
    }

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,
              sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port: %d \n", PORT);
    time_t endwait;
    time_t start = time(NULL);
    time_t seconds = 10; // end loop after this time has elapsed

    endwait = start + seconds;

    printf("Start time is : %s", ctime(&start));
    int len, n;
    int licznik=0;
    len = sizeof(cliaddr); //len is value/resuslt
    puts("Waiting for clients ...");
    while (start < endwait) {
        sleep(1);
        start = time(NULL);
        printf("Loop time is : %s", ctime(&start));

        n = recvfrom(sockfd,&received_data, sizeof(struct message),MSG_DONTWAIT, (struct sockaddr *) &cliaddr,&len);
        if(n>0)
        {
            if(received_data.type_of_message==0)
            {
                lista_adres[licznik]=cliaddr;
                licznik++;
                printf("Client has been added to list \n");
            }
        }
    }
    printf("Stopped listening, number of clients:   \n %d  ",licznik);
    printf("Uploading seed  \n  ");

    int wymiarx=0;
    int wymiary=0;
    char line[10];
    char line2[1];
    int licznikw=0;
    int *DANE;
    int temp=0;
    char const* const fileName = "seed.txt";

    FILE* file = fopen(fileName, "r");

    if(!file){
        printf("\n Unable to open : %s ", fileName);
        return -1;
    }
    while (fgets(line, sizeof(line), file)) {
        licznikw++;
        char * token = strtok(line, "X");
        // loop through the string to extract all other tokens
        while( token != NULL ) {
            token = strtok(NULL, "X");
        }
    }
    fclose(file);

    printf(" %d \n",licznikw);

    licznikw=licznikw*2;
    fopen(fileName, "r");
    DANE = malloc( licznikw * sizeof(int));
    licznikw=0;

    if(!file){
        printf("\n Unable to open : %s ", fileName);
        return -1;
    }
    while (fgets(line, sizeof(line), file)) {

        char * token = strtok(line, "X");
        // loop through the string to extract all other tokens
        while( token != NULL ) {
            DANE[licznikw] = atoi(token); //printing each token
            licznikw++;
            token = strtok(NULL, "X");
        }
    }
    fclose(file);

    for(int i=0;i<licznikw;i++)
    { printf("%d \n",DANE[i]);
    }
    //pierwsze 2 wartosci to wymiary x y nastepnie wielokrtnosci 2 to x a y to x+1

    wymiarx=DANE[0];
    wymiary=DANE[1];

    printf("Board size is X %d  Y %d \n",DANE[0],DANE[1]);

    //OBLICZANIE WYMIAROW PROSTOKATOW
    if(licznik==0)
    {
        printf("No clients were added to list");
        return 0;
    }

    int szerokosc=(DANE[0]-(DANE[0]%licznik))/licznik;
    printf("Average width %d \n",szerokosc);
    int TABLICA_SZEROKOSCI_POL[licznik];
    for(int i=0;i<licznik;i++)
    {
        TABLICA_SZEROKOSCI_POL[i]=szerokosc;
    }
    TABLICA_SZEROKOSCI_POL[licznik-1]=TABLICA_SZEROKOSCI_POL[licznik-1]+wymiarx%licznik;

    printf("szerokosc ostatniego pola  %d \n",TABLICA_SZEROKOSCI_POL[licznik-1]);

    for(int i=0;i<licznik;i++)
    {
        printf("Width of client  %d field is %d \n",i,TABLICA_SZEROKOSCI_POL[i]);
    }

    char *TABLICA_GRY [wymiarx][wymiary];
    char *ZYWE="@";
    char *MARTWE="-";
    // - martwe  # zywe
// Wypelnienie plansyz martwymi komorkami
    for(int i=0;i<wymiarx;i++)
    { for(int j=0;j<wymiary;j++)
        {
            TABLICA_GRY[i][j]=MARTWE;
        }
    }
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");

    //wypelnienie planszy zywymi komorkami w miejscach wskazanych w seedzie

    for(int i=2;i<licznikw-1;i+=2)
    {
        TABLICA_GRY[DANE[i]][DANE[i+1]]=ZYWE;
    }

    printf("\n--------------------------------------------------------------\n");
    for (int a = 0; a < wymiarx; a++) {
        printf("|");
        for (int b = 0; b < wymiary; b++) {
            printf(" %s ", TABLICA_GRY[a][b]);
        }
        printf("|");
        printf("\n");
    }
    printf("\n--------------------------------------------------------------\n");

    free(DANE);

    //POSZUKIWANIE ZYWYCH KOMOREK W KAZDYM OBSZARZE A NASTEPNIE SPAKOWANIE ICH DO WIADOMOSCI
    // ORAZ WYSLANIE DO ODPOWIEDNIEGO KLIENTA


    int PRZESUNIECIE_DO_POCZATKU_PLANSZY=0;
    int SZER=0;

    int TABLICA_ILOSC_ZYWYCH_KOMOREK[licznik]; // WARTOSC NA 0 POLU TO ILOSC ZYWYCH KOMOREK W 1 STREFIE
    for(int x=0;x<licznik;x++)
    {
        int LICZBA_ZYWYCH_DLA_KLIENTA=0;

        SZER=TABLICA_SZEROKOSCI_POL[x];

        if(x>0)
        {
            PRZESUNIECIE_DO_POCZATKU_PLANSZY=PRZESUNIECIE_DO_POCZATKU_PLANSZY+TABLICA_SZEROKOSCI_POL[x-1];
        }

        printf( " PRZESUNIECIE %d \n", PRZESUNIECIE_DO_POCZATKU_PLANSZY);

        for(int k=PRZESUNIECIE_DO_POCZATKU_PLANSZY;k<SZER+PRZESUNIECIE_DO_POCZATKU_PLANSZY;k++)
        {
            for(int j=0;j<wymiary;j++)
            {
                if (TABLICA_GRY[k][j]==ZYWE)
                {
                    LICZBA_ZYWYCH_DLA_KLIENTA++;
                    printf( " X %d Y %d \n", k,j);
                }
            }
        }
        TABLICA_ILOSC_ZYWYCH_KOMOREK[x]=LICZBA_ZYWYCH_DLA_KLIENTA;

        printf(" LICZBA ZYWYCH DLA KLIENTA %d wynosi %d \n",x,LICZBA_ZYWYCH_DLA_KLIENTA);

    }

    for(int i=0;i<licznik;i++)
    {


        cliaddr=lista_adres[i];
        char wiad[MAXLINE];
        sprintf(wiad,"%dX%dX%dX%dX%d",TABLICA_SZEROKOSCI_POL[i],wymiary,TABLICA_ILOSC_ZYWYCH_KOMOREK[i],i,licznik);
        wysylka.type_of_message=2;
        wysylka.TABLICA[0]=TABLICA_SZEROKOSCI_POL[i];
        wysylka.TABLICA[1]=wymiary;
        wysylka.TABLICA[2]=TABLICA_ILOSC_ZYWYCH_KOMOREK[i];
        wysylka.TABLICA[3]=i;
        wysylka.TABLICA[4]=licznik;
        len = sizeof(cliaddr);

        sendto(sockfd,&wysylka, sizeof(struct message),MSG_CONFIRM, (const struct sockaddr *) &cliaddr,len);
    }
    for(;;) {
        PRZESUNIECIE_DO_POCZATKU_PLANSZY = 0;
        SZER = 0;

        for (int i = 0; i < licznik; i++) {
            memset(wysylka.TABLICA, -1, sizeof(wysylka.TABLICA));
            SZER = TABLICA_SZEROKOSCI_POL[i];
            int INDEX_KOORDYNATOW = 0;
            if(i == 0){
                for (int k = PRZESUNIECIE_DO_POCZATKU_PLANSZY; k < SZER + PRZESUNIECIE_DO_POCZATKU_PLANSZY + 1; k++) {
                    for (int j = 0; j < wymiary; j++) {
                        if (TABLICA_GRY[k][j] == ZYWE) {
                            wysylka.TABLICA[INDEX_KOORDYNATOW] = k - PRZESUNIECIE_DO_POCZATKU_PLANSZY;
                            wysylka.TABLICA[INDEX_KOORDYNATOW + 1] = j;
                            INDEX_KOORDYNATOW = INDEX_KOORDYNATOW + 2;
                        }
                    }
                }
                PRZESUNIECIE_DO_POCZATKU_PLANSZY=-1;
            }
            else if (i!=licznik-1) {
                PRZESUNIECIE_DO_POCZATKU_PLANSZY += TABLICA_SZEROKOSCI_POL[i - 1];
                for (int k = PRZESUNIECIE_DO_POCZATKU_PLANSZY; k < SZER + PRZESUNIECIE_DO_POCZATKU_PLANSZY + 2; k++) {
                    for (int j = 0; j < wymiary; j++) {
                        if (TABLICA_GRY[k][j] == ZYWE) {
                            wysylka.TABLICA[INDEX_KOORDYNATOW] = k - PRZESUNIECIE_DO_POCZATKU_PLANSZY;
                            wysylka.TABLICA[INDEX_KOORDYNATOW + 1] = j;
                            INDEX_KOORDYNATOW = INDEX_KOORDYNATOW + 2;
                        }
                    }
                }
            }
            else{
                PRZESUNIECIE_DO_POCZATKU_PLANSZY += TABLICA_SZEROKOSCI_POL[i - 1];
                for (int k = PRZESUNIECIE_DO_POCZATKU_PLANSZY; k < SZER + PRZESUNIECIE_DO_POCZATKU_PLANSZY + 1; k++) {
                    for (int j = 0; j < wymiary; j++) {
                        if (TABLICA_GRY[k][j] == ZYWE) {
                            wysylka.TABLICA[INDEX_KOORDYNATOW] = k - PRZESUNIECIE_DO_POCZATKU_PLANSZY;
                            wysylka.TABLICA[INDEX_KOORDYNATOW + 1] = j;
                            INDEX_KOORDYNATOW = INDEX_KOORDYNATOW + 2;
                        }
                    }
                }
            }

            cliaddr = lista_adres[i];
            wysylka.type_of_message = 3;
            len = sizeof(cliaddr);
            sendto(sockfd, &wysylka, sizeof(struct message), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
        }
        for(int q=0;q<wymiarx;q++)
        { for(int w=0;w<wymiary;w++)
            {
                TABLICA_GRY[q][w]=MARTWE;
            }
        }
        int width = 0;
//SEKWENCYJNY ODBIOR FRAGMENTOW PLANSZY OD KAZDEGO Z KLIENTOW
        for (int i = 0; i < licznik; i++) {
            cliaddr = lista_adres[i];
            char wiad[MAXLINE];
            wysylka.type_of_message = 4;
            len = sizeof(cliaddr);
            sendto(sockfd, &wysylka, sizeof(struct message), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
            n = recvfrom(sockfd, &received_data, sizeof(struct message),
                         MSG_WAITALL, (struct sockaddr *) &cliaddr,
                         &len);
            int counter = 0;
            while (received_data.TABLICA[counter] >= 0) {
                TABLICA_GRY[received_data.TABLICA[counter] + width][received_data.TABLICA[counter + 1 ]]= ZYWE;
                counter += 2;
            }
        width += TABLICA_SZEROKOSCI_POL[i];
    }
        printf("\n--------------------------------------------------------------\n");
        for (int a = 0; a < wymiarx; a++) {
            printf("|");
            for (int b = 0; b < wymiary; b++) {
                printf(" %s ", TABLICA_GRY[a][b]);
            }
            printf("|");
            printf("\n");
        }
        printf("\n--------------------------------------------------------------\n");
        sleep(1);
    }
    return 0;
}
