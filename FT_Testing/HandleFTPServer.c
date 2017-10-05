#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFSIZE 30000

void DieWithError(char *errorMessage);

void HandleFTPServer(int clntSocket) {
    
    typedef struct Files {
        char fileName[255];
        int totalBytes;
    }Files;
    
    char buf[BUFSIZE];
    FILE *fp = NULL;
    Files files;
    int recvMsgSize;
    int count;
    
    if((recvMsgSize = recv(clntSocket, (char *)&files, sizeof(files), 0)) < 0)
        DieWithError("recv() failed");

    printf("파일을 전송 받습니다.\n");
    printf("전송하는 파일: %s, 전송받는 파일크기: %d Byte\n", files.fileName, files.totalBytes);

    fp = fopen(files.fileName, "wb");
    count = (files.totalBytes / BUFSIZE);

    while(count) {
        if((recvMsgSize = recv(clntSocket, buf, BUFSIZE, 0)) < 0)
            DieWithError("recv() failed");

        fwrite(buf, 1, BUFSIZE, fp);
        count--;
    }

    count = files.totalBytes - ((files.totalBytes / BUFSIZE) * BUFSIZE);
    if((recvMsgSize = recv(clntSocket, buf, BUFSIZE, 0)) < 0)
        DieWithError("recv() failed");

    fwrite(buf, 1, count, fp);
    
    fclose(fp);
}





