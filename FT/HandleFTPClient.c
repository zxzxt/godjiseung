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
    char buf[BUFSIZE];
    char command;
    FILE *fp = NULL;
    Files files;

    printf("Welcome to Socket FT client!\n");   
   
    while(command != 'e') {     
        printf("ftp command [ p)ut   g)et   l)s   r)s   e)xit ] -> ");
        scanf("%s", &command);
    
        if(command == 'p') {
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

            fclose(fp);

        }//if
        else {
            printf("Undefined Command\n");
        }
    }//outter while
}









