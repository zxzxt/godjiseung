#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#define BUFSIZE 30000

void DieWithError(char *errorMessage);

void HandleFTPClient(int sock, char *servIP) {

    /* struct cotains the info. of a file */
    typedef struct Files {
        char fileName[255];
        int totalBytes;
    }Files;
    
    int count, per;  /* To divide the total bytes by the buffer size */
    int recvMsgSize;
    char buf[BUFSIZE];
    char command[2];
    char fileAck[10] = "FileACK";
    FILE *fp = NULL;
    Files files;

    printf("Welcome to Socket FT client!\n");   
   
    while(strcmp(command, "e")) {     
        printf("ftp command [ p)ut   g)et   l)s   r)s   e)xit ] -> ");
        scanf("%s", command);
    
        /* Send command to the server */
        if(send(sock, command, 1, 0) < 0)
            DieWithError("send() sent a different number of bytes\n");

        if(!strcmp(command, "p")) {
            printf("filename to put to server -> ");
            scanf("%s", files.fileName);
            getchar();      /* to ignore Enter */

            fp = fopen(files.fileName,"rb"); /* to check whether specific file exists or not */
            if(fp == NULL)
                DieWithError("Error");

            fseek(fp, 0L, SEEK_END); /* move cursor to the end of the file */
            files.totalBytes = ftell(fp);   /* total bytes */
            fseek(fp, 0L, SEEK_SET);    /* move cursor to the front of the file */
        
            /* send struct that contains a info. of the file */
            if(send(sock, (char *)&files, sizeof(files), 0) < 0)
                DieWithError("send() sent a different number of bytes \n");
         
            if(recv(sock, fileAck, 10, 0) < 0)
                DieWithError("No ACK received from the server\n");
            else
                printf("ACK Received from the server\n");

            per = count = files.totalBytes / BUFSIZE;
 
            while(count) {               
                /* read file and write in buffer */
                fread(buf, 1, BUFSIZE, fp);
                
                /* send */
                if (send(sock, buf, BUFSIZE, 0) != BUFSIZE)
                    DieWithError("send() sent a different number of bytes\n");
         
                count--;
            }
      
            /* send rest of the file */
            count = files.totalBytes - (per*BUFSIZE);
            fread(buf, 1, count ,fp);
            printf("Sending => #########\n");
            if (send(sock, buf, BUFSIZE, 0) != BUFSIZE)
                DieWithError("send() sent a different number of bytes\n");
            printf("%s(%d bytes) uploading success to %s\n", files.fileName, files.totalBytes, servIP);
            printf("\n");

            fclose(fp);

        }//if
        else if(!strcmp(command, "g")) {
            printf("filename to get from server -> ");
            scanf("%s", files.fileName);
            getchar();
            
            if(send(sock, files.fileName, 255, 0) != 255)
                DieWithError("send() sent a different number of bytes\n");
            
            if((recvMsgSize = recv(sock, (char *)&files, sizeof(files), 0)) < 0)
                DieWithError("recv() failed");
            else {
                send(sock, fileAck, 10, 0);
                printf("Sent ACK to the server\n");
            }
            
            if(recvMsgSize > 0) {
                printf("파일을 전송 받습니다.\n");
                printf("전송받는 파일: %s, 전송받는 파일크기: %d Byte\n", files.fileName, files.totalBytes);

                /* create file */
                fp = fopen(files.fileName, "wb");

                per = count = (files.totalBytes / BUFSIZE);
                while(count) {
                    if((recvMsgSize = recv(sock, buf, BUFSIZE, 0)) < 0)
                        DieWithError("recv() failed");

                    /* write file from buffer */
                    fwrite(buf, 1, BUFSIZE, fp);
                    count--;
                }

                /* receive rest of the file */
                count = files.totalBytes - (per*BUFSIZE);
                if((recvMsgSize = recv(sock, buf, BUFSIZE, 0)) < 0)
                    DieWithError("recv() failed");

                fwrite(buf, 1, count, fp);

                fclose(fp);
            }//if
        }// if 'g'
        else if(!strcmp(command, "l")) {
            system("ls");
        }
        else if(!strcmp(command, "e")) {
            break;
        }
        else {
            printf("Undefined Command\n");
        }
    }//outter while
}









