#include <stdio.h>
/* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h> /* for close() */
#define RCVBUFSIZE 32 /* Size of receive buffer */

void DieWithError(char *errorMessage);
/* Error handling function */

void HandleFTPServer(int clntSocket);

void HandleTCPClient(int clntSocket) {

	char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */
    int recvMsgSize; /* Size of received message */

    /* Receive message from client */
	if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
		DieWithError("recv() failed");

    /* Send 'hi' after the connection  */
    if(recvMsgSize > 0) {
        echoBuffer[recvMsgSize] = '\0';
        printf("msg<- ");
        printf("%s\n", echoBuffer);

        strcpy(echoBuffer, "hi");
        if(send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
            DieWithError("send() failed");
        printf("msg-> ");
        printf("%s\n", echoBuffer);

        if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
            DieWithError("recv() failed");
    }

	/* Send received string and receive again until end of transmission */
	while (recvMsgSize > 0) { /* zero indicates end of transmission */
        
        if(!strcmp(echoBuffer, "FT")) {
            HandleFTPServer(clntSocket);
        }
        else {
            echoBuffer[recvMsgSize] = '\0';
            printf("msg<- ");
            printf("%s\n", echoBuffer);

            /* Echo message back to client */
            if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
			    DieWithError("send() failed");
       
            printf("msg-> ");
            printf("%s\n", echoBuffer);
        }

        /* See if there is more data to receive */
		if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
			DieWithError("recv() failed");
	}
	close(clntSocket);
	/* Close client socket */
}
