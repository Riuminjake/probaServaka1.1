#include <stdio.h>
#include <winsock2.h>
#include <locale.h>
#include <pthread.h>
#include <time.h>
#pragma comment(lib, "Ws2_32.lib")
#define PORT 9034
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
    SetConsoleTextAttribute(hConsole,2);
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
    adress.sin_addr.S_un.S_addr=inet_addr("192.168.1.166");
    adress.sin_port=htons(PORT);
    if (0!=(iResult=WSAStartup(MAKEWORD(2,2),&proba))) {
        printf("WSAStartup failed: %d\\n", iResult);
        return WSAGetLastError();
    }
    if (INVALID_SOCKET==(w=socket(AF_INET,SOCK_STREAM,0)))
        return WSAGetLastError();
    if (SOCKET_ERROR==bind(w,(struct sockaddr*)&adress,sizeof (adress)))
        return E_FAIL;
    if (FAILED(listen(w,10)))
        return E_FAIL;
    ZeroMemory (&new_adress,sizeof(new_adress));
    if (FAILED(new_w=accept(w,(struct sockaddr*)&new_adress,&new_len))) {
        perror("accept");}
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
    closesocket(new_w);
    WSACleanup();
    return 0;
}
void *zapis(){
    fgets(zapiska,4000,stdin);
    return NULL;
}

char prislo(SOCKET *ConnectSocket) {
    char recvbuf[BUFLEN];
    if (SOCKET_ERROR==ioctlsocket(*ConnectSocket,FIONBIO,(unsigned long*)&l))
        return 0;
    iResult = recv(*ConnectSocket, recvbuf, BUFLEN, 0);
    if ( iResult > 0 ){
        XY.X=STRED_X;
        XY.Y+=2;
        SetConsoleCursorPosition(hConsole, XY);
        printf("Bytes received: %d\n", iResult);//prisli validne data, vypis poctu
        XY.Y++;
        SetConsoleCursorPosition(hConsole, XY);
        t= time(NULL);
        aTm= localtime(&t);
        printf("%s",asctime(aTm));
        XY.Y++;
        SetConsoleCursorPosition(hConsole, XY);
        printf("Morpheus:");
        XY.Y++;
        recvbuf[iResult]='\0';
        vipis(recvbuf,60,iResult);
        return 1;
    }
    else if ( iResult == 0 ) {
        printf("Connection closed\n");    //v tomto pripade server ukoncil komunikaciu
        return 0;
    }
    return 1;
}
char uslo(SOCKET *ConnectSocket) {
    printf("Neo:\n");
    XY.Y++;
    t= time(NULL);
    aTm= localtime(&t);
    printf("%s",asctime(aTm));
    XY.Y++;
    XY.Y++;
    iResult = send(*ConnectSocket, zapiska, (int) strlen(zapiska), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        XY.Y++;
        closesocket(*ConnectSocket);
        WSACleanup();
        return 1;
    }
    printf("Bytes Sent: %ld\n", iResult);     //vypisanie poctu odoslanych dat
    XY.Y++;
    return 1;
}


void vipis (char* recvbuf,int x,int velkost){
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
        SetConsoleCursorPosition(hConsole, XY);
        printf("%c",recvbuf[i]);
        XY.X++;
    }
    printf("\n");
}
/*   if (NULL==(ht=gethostbyname("qastack.ru")))
       a = WSAGetLastError();
   adress.sin_addr.S_un.S_addr=((struct in_addr*)ht->h_addr_list[0])->s_addr;
   adress.sin_family=AF_INET;
   adress.sin_port=htons(80);*/
/*if (FAILED(new_w=accept(w,(struct sockaddr*)&new_adress,&new_len))) {
perror("accept");}
if (SOCKET_ERROR==(send(new_w,(char*)&stroka,6,0)))
a = WSAGetLastError();
if (SOCKET_ERROR==(a=recv(new_w,(char*) & priem,4000,0)))
a = WSAGetLastError();
for (int i=0;i<a;i++){
printf("%c",priem[i]);
}*/