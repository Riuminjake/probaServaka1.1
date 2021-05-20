#include <stdio.h>
#include <winsock2.h>
#include <locale.h>
#include <pthread.h>
#include <time.h>
#pragma comment(lib, "Ws2_32.lib")
#define PORT 1000
#define BUFLEN 4096
#define STRED_X 60
void vipis();
void *zapis();
char prislo();
char uslo();
BOOL l=TRUE;
int iResult;
char zapiska[BUFLEN];
time_t t;
struct tm* aTm;
COORD XY;
HANDLE  hConsole;
//pthread_t thread_id;
int main() {
    int iResult;
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    XY.X=0;
    XY.Y=0;
    hConsole= GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole,7);
    WSADATA proba;
    SOCKET w;
    SOCKET new_w;
    struct sockaddr_in new_adress;
    int new_len= sizeof(new_adress);
    struct sockaddr_in adress;
    struct hostent* ht;
    ZeroMemory (&adress,sizeof(adress));
    ZeroMemory (&new_adress,sizeof(new_adress));
    adress.sin_family=AF_INET;
    adress.sin_addr.S_un.S_addr=inet_addr("192.168.1.160");
    adress.sin_port=htons(PORT);
    if (0!=(iResult=WSAStartup(MAKEWORD(2,2),&proba))) {
        printf("WSAStartup failed: %d\\n", iResult);
        return WSAGetLastError();
    }
    if (INVALID_SOCKET==(w=socket(AF_INET,SOCK_STREAM,0)))
        return WSAGetLastError();
    if (-1==(connect(w,(struct sockaddr*)&adress,sizeof(adress)))) {
        ZeroMemory (&adress,sizeof(adress));
        closesocket(w);
        if (INVALID_SOCKET==(w=socket(AF_INET,SOCK_STREAM,0)))
            return WSAGetLastError();
        adress.sin_family=AF_INET;
        adress.sin_addr.S_un.S_addr=inet_addr("192.168.1.166");
        adress.sin_port=htons(PORT);
        if (SOCKET_ERROR == bind(w, (struct sockaddr *) &adress, sizeof(adress)))
            return E_FAIL;
        if (FAILED(listen(w, 10)))
            return E_FAIL;
        ZeroMemory (&new_adress, sizeof(new_adress));
        if (FAILED(new_w = accept(w, (struct sockaddr *) &new_adress, &new_len))) {
            perror("accept");
        }
    }
    else {
        new_w = w;
        printf("Connected to server!\n");
        XY.Y++;
    }
    int z=0;
    pthread_t thread_id;
    do {
        zapiska[0]='\0';
        Sleep(250);
        z=prislo(&new_w);
        if (z==0)
            break;
        pthread_create(&thread_id, NULL,zapis, NULL);
        if (zapiska[0]!='\0') {
            z = uslo(&new_w);
            //pthread_join(thread_id, NULL);
        }
    }while(z>0);
    pthread_join(thread_id, NULL);//конец разветвления программы
    closesocket(w);
    closesocket(new_w);
    WSACleanup();
    return 0;
}
void *zapis(){
    char b[100];
    char* z=NULL;
    do {
        z=fgets(b, 4000, stdin);
        strcat(zapiska,z);
    }while(z!=NULL);
}

char prislo(SOCKET *ConnectSocket) {
    SetConsoleTextAttribute(hConsole,7);
    char recvbuf[BUFLEN];
    BOOL odin_raz=FALSE;
    if (SOCKET_ERROR==ioctlsocket(*ConnectSocket,FIONBIO,(unsigned long*)&l))
        return 0;
    do{
    iResult = recv(*ConnectSocket, recvbuf, BUFLEN, 0);
    if ( iResult > 0 ){
        if (odin_raz==FALSE){
            XY.X=STRED_X;
            XY.Y+=2;
            // printf("Bytes received: %d\n", iResult);//prisli validne data, vypis poctu
            // XY.Y++;
            SetConsoleCursorPosition(hConsole, XY);
            t= time(NULL);
            aTm= localtime(&t);
            printf("%s",asctime(aTm));
            XY.Y++;
            SetConsoleCursorPosition(hConsole, XY);
            printf("Morpheus:");
            XY.Y++;
            odin_raz=TRUE;
        }
        recvbuf[iResult]='\0';
        vipis(recvbuf,60,iResult);
    }
    else if ( iResult == 0 ) {
        printf("Connection closed\n");    //v tomto pripade server ukoncil komunikaciu
        return 0;
    }}while(iResult>0);
    return 1;
}
char uslo(SOCKET *ConnectSocket) {
    SetConsoleTextAttribute(hConsole,7);
    //printf("Neo:\n");
   // XY.Y++;
  //  t= time(NULL);
  //  aTm= localtime(&t);
  //  printf("%s",asctime(aTm));
   // XY.Y++;
     XY.Y++;
    iResult = send(*ConnectSocket, zapiska, (int) strlen(zapiska), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        XY.Y++;
        closesocket(*ConnectSocket);
        WSACleanup();
        return 1;
    }
    //printf("Bytes Sent: %ld\n", iResult);     //vypisanie poctu odoslanych dat
   // XY.Y++;
    return 1;
}


void vipis (char* recvbuf,int x,int velkost){
    SetConsoleTextAttribute(hConsole,2);
    setlocale(LC_ALL, "");
    XY.X=x;
    char bukva;
    int dlina=0;
    int razmer=0;
    SetConsoleCursorPosition(hConsole, XY);
    for (int i=0,c=0;i<velkost-1;i++,c++) {
        bukva = recvbuf[i];
        if (i == 0 || i == razmer + 1 || ((razmer < 0) && (c == 60))) {
            if (razmer >= 0) {
                dlina = razmer;
                razmer = strchr(recvbuf + razmer + 1, ' ') - recvbuf;
            }
            if ((i == 0) && (razmer > 0))
                dlina = razmer;
            else if (razmer > 0)
                dlina = razmer - dlina;
            else {
                dlina = velkost - dlina;
            }
            if (dlina > 60)
                dlina = 0;
            if ((dlina + c) > 59) {
                c = 0;
                XY.X = x;
                XY.Y++;
                if (razmer < 0)
                    dlina = 0;
                SetConsoleCursorPosition(hConsole, XY);
            }
        }
        if (recvbuf[i]=='\n')
        {
            XY.Y++;
            c=x;            
        }
        else {
            SetConsoleCursorPosition(hConsole, XY);
            printf("%c", recvbuf[i]);
            XY.X++;
        }
    }
    printf("\n");
}
