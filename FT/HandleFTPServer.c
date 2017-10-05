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
    int per;

   
    /* receive file struct */
    if((recvMsgSize = recv(clntSocket, (char *)&files, sizeof(files), 0)) < 0)
        DieWithError("recv() failed");
 
    while(recvMsgSize > 0) {

        printf("파일을 전송 받습니다.\n");
        printf("전송받는 파일: %s, 전송받는 파일크기: %d Byte\n", files.fileName, files.totalBytes);
    
        /* create file */
        fp = fopen(files.fileName, "wb");

        per = count = (files.totalBytes / BUFSIZE);
        while(count) {
            if((recvMsgSize = recv(clntSocket, buf, BUFSIZE, 0)) < 0)
                DieWithError("recv() failed");
        
            /* write file from buffer */
            fwrite(buf, 1, BUFSIZE, fp);
            count--;
        }
    
        /* receive rest of the file */
        count = files.totalBytes - (per * BUFSIZE);
        if((recvMsgSize = recv(clntSocket, buf, BUFSIZE, 0)) < 0)
        DieWithError("recv() failed");

        fwrite(buf, 1, count, fp);
        fclose(fp);
        
        /* if there is more file to receive */
        if((recvMsgSize = recv(clntSocket, (char *)&files, sizeof(files), 0)) < 0)
            DieWithError("recv() failed");
    }
}





