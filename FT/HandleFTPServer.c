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
    
    char command[2];
    char buf[BUFSIZE];
    char fileAck[10] = "FileAck";
    FILE *fp = NULL;
    Files files;
    int recvMsgSize;
    int count;
    int per;

    while(strcmp(command, "e")) {
        /* Receive command from the client */
        if((recvMsgSize = recv(clntSocket, command, 1, 0)) < 0)
            DieWithError("recv() failed");
   
        if(!strcmp(command, "p")) {  
            /* receive file struct */
            if((recvMsgSize = recv(clntSocket, (char *)&files, sizeof(files), 0)) < 0)
                DieWithError("recv() failed");
            else {
                send(clntSocket, fileAck, 10, 0);
                printf("Sent ACK to the client\n");
            }

            if(recvMsgSize > 0) {

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
            }//if
        }//if 'p'
        else if(!strcmp(command, "g")) {
            if((recvMsgSize = recv(clntSocket, files.fileName, 255, 0)) < 0)            
                DieWithError("send() sent a different number of bytes\n");
            
            fp = fopen(files.fileName, "rb");
            if(fp == NULL)
                DieWithError("Error");

            fseek(fp, 0L, SEEK_END);
            files.totalBytes = ftell(fp);
            fseek(fp, 0L, SEEK_SET);

            /* send file struct */
            if(send(clntSocket, (char *)&files, sizeof(files), 0) < 0)
                DieWithError("send() sent a different number of bytes\n");
            
            if(recv(clntSocket, fileAck, 10, 0) < 0)
                DieWithError("No ACK received from the client\n");
            else
                printf("ACK Received from the client\n");

            per = count = files.totalBytes / BUFSIZE;

            while(count) {
                fread(buf, 1, BUFSIZE, fp);

                if(send(clntSocket, buf, BUFSIZE, 0) != BUFSIZE)
                    DieWithError("send() sent a different number of bytes\n");

                count--;
            }

            /* send rest of the file */
            count = files.totalBytes - (per*BUFSIZE);
            fread(buf, 1, count, fp);
            printf("Sending => #########\n");
            if(send(clntSocket, buf, BUFSIZE, 0) != BUFSIZE)
                DieWithError("send() sent a different number of bytes\n");
            printf("%s(%d bytes) sending success\n", files.fileName, files.totalBytes);
            printf("\n");

            fclose(fp);
        }// if 'g'
        else if(!strcmp(command, "r")) {
            system("ls > ls.txt");

            fp = fopen("ls.txt", "r");
            if(fp == NULL)
                DieWithError("Error");

            fseek(fp, 0L, SEEK_END);
            files.totalBytes = ftell(fp);
            fseek(fp, 0L, SEEK_SET);
            
            /* send file size */
            if(send(clntSocket, (char *)&files.totalBytes, sizeof(int), 0) < 0)
                DieWithError("send() sent a different number of bytes\n");

            per = count = (files.totalBytes / BUFSIZE);
           
            while(count) {
                fread(buf, 1, BUFSIZE, fp);
                
                if(send(clntSocket, buf, BUFSIZE, 0) != BUFSIZE)
                    DieWithError("send() sent a different number of bytes\n");

                count--;
            }
         
            /* send rest of the file */
            count = files.totalBytes - (per*BUFSIZE);
            fread(buf, 1, count, fp);
            
            if(send(clntSocket, buf, BUFSIZE, 0) != BUFSIZE)
                DieWithError("send() sent a different number of bytes\n");
            
            fclose(fp);
            
        }//if 'r'
        else if(!strcmp(command, "e")) {
            break;
        }
    }//while not 'e'

    strcpy(command, "z");
}//function





