#include <stdio.h>
/* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h> /* for close() */
#define RCVBUFSIZE 32 /* Size of receive buffer */

void DieWithError(char *errorMessage);
/* Error handling function */

void HandleTCPClient(int clntSocket) {

    FILE *stream = NULL;
    stream = fopen("echo_history.log", "a");
    if(stream == NULL)
        DieWithError("File open error !");

	char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */
    int recvMsgSize; /* Size of received message */
	int counter = 0;

    /* Receive message from client */
	if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
		DieWithError("recv() failed");
   
	/* Send received string and receive again until end of transmission */
	while (recvMsgSize > 0) { /* zero indicates end of transmission */
        echoBuffer[recvMsgSize] = '\0';
        printf("msg<- ");
        printf("%s\n", echoBuffer);

        if(strcmp(echoBuffer, "hello") == 0 && counter == 0)
            strcpy(echoBuffer, "hi");

        /* Echo message back to client */
        if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
			DieWithError("send() failed");
       
        printf("msg-> ");
        printf("%s\n", echoBuffer);
    
        fprintf(stream,"%s\n", echoBuffer);
		
        /* See if there is more data to receive */
		if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
			DieWithError("recv() failed");

        counter = 1;
	}
    fclose(stream);
	close(clntSocket);
	/* Close client socket */
}
