/* time_server.c - main */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#define BUFSIZE 100
#define EMSG "Cannot find content"
#define NAMESIZ 20
#define MAXCONT 200


typedef struct pdu{
	char type;
	char data[BUFSIZE];
}PDU;                                                               
PDU rpdu;

struct {
	char usr[NAMESIZ]; /* Username of host */
	char name[NAMESIZ];/* Content Name of host */
	char f_name[NAMESIZ];/* File Name of Host */
	int addr;
	int port;
} list[MAXCONT];

char user[NAMESIZ];
char fname[NAMESIZ];
/*------------------------------------------------------------------------
 * main - Iterative UDP server
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	struct  sockaddr_in fsin;	/* the from address of a client	*/
	char	buf[100];		/* "input" buffer; any size > 0	*/
	char    *pts;
	int	sock;			/* server socket		*/
	time_t	now;			/* current time			*/
	int	alen;			/* from-address length		*/
	struct  sockaddr_in sin; /* an Internet endpoint address         */
        int     s, type;        /* socket descriptor and socket type    */
	int 	port=3000;
	int	n_cont = 0;	/* number of registered content */
	int i, j, done;
        
        
	switch(argc){
		case 1:
			break;
		case 2:
			port = atoi(argv[1]);
			break;
		default:
			fprintf(stderr, "Usage: %s [port]\n", argv[0]);
			exit(1);
	}

        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_port = htons(port);
                                                                                                 
    /* Allocate a socket */
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s < 0)
		fprintf(stderr, "can't creat socket\n");
                                                                                
    /* Bind the socket */
        if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		fprintf(stderr, "can't bind to %d port\n",port);
        listen(s, 5);	
	alen = sizeof(fsin);

	while (1) {
		PDU spdu, rpdu;
		recvfrom(s, &spdu, BUFSIZE, 0, (struct sockaddr *)&fsin, &alen);
		
		if (spdu.type == 'R'){ //Content Registration
			int check;
			check = 0;
			char temp[BUFSIZE]; //temp hold of content name			
			strcpy (list[n_cont].usr, spdu.data); // Received username 1st
			bzero(spdu.data, BUFSIZE);
			//recvfrom(s, &list[n_cont].name, BUFSIZE, 0, (struct sockaddr *)&fsin, &alen); // content name
			recvfrom(s, &spdu, BUFSIZE, 0, (struct sockaddr *)&fsin, &alen); // content name
			strcpy (temp, spdu.data); // Received name 
			for(i=0; i<n_cont; i++){
				if(strcmp(list[i].name, spdu.data) == 0){
					// Content name already occurs in the list
					spdu.type = 'E';
					bzero(spdu.data, BUFSIZE);
					strcpy(spdu.data,"Registered content not found");
					(void)sendto(s,&spdu, BUFSIZE, 0,(struct sockaddr *)&fsin, sizeof(fsin));
					check = 1;					
					break;
				}
			}
			if(check == 0) {
				// Content name is unique, send Acknowledgement + add content
				strcpy(list[n_cont].name, temp);
				recvfrom(s, &list[n_cont].f_name, BUFSIZE, 0, (struct sockaddr *)&fsin, &alen); // file name
				recvfrom(s, &list[n_cont].port, BUFSIZE, 0, (struct sockaddr *)&fsin, &alen); // port
				list[n_cont].addr = fsin.sin_addr.s_addr; // ip address in dot format -> inet_ntoa(fsin.sin_addr)
				printf("Owner: %s\nContent Name: %s\nFile: %s\nAddress: %d:%d\n", list[n_cont].usr, list[n_cont].name, list[n_cont].f_name, list[n_cont].addr, list[n_cont].port);
			
				n_cont++;
				bzero(spdu.data, BUFSIZE);
				spdu.type = 'A';
				strcpy(spdu.data,"Content has been registered.\n");
				(void)sendto(s, &spdu, strlen(spdu.data), 0, (struct sockaddr *)&fsin, sizeof(fsin)); // Sent acknowledgement
			}
			
			
		}
		else if(spdu.type == 'D'){ // Content Download Request
			done = 0;
			for (i=0; i<n_cont; i++){			
				if (strcmp(list[i].name, spdu.data) == 0){ // Searching for content name				
					done = 1;
					spdu.type = 'A';
					strcpy(spdu.data,"registered content found.\n");
					(void)sendto(s, &spdu, strlen(spdu.data), 0, 
						(struct sockaddr *)&fsin, sizeof(fsin)); // Sent acknowledgement
					bzero(spdu.data, BUFSIZE);
					spdu.type = 'D';
					sprintf(spdu.data, "%d", list[i].addr);
					(void)sendto(s,&spdu, BUFSIZE, 0, 
						(struct sockaddr *)&fsin, sizeof(fsin)); // Sending peer server address
					bzero(spdu.data, BUFSIZE);
					sprintf(spdu.data, "%d", list[i].port);
					(void)sendto(s,&spdu , BUFSIZE, 0, 
						(struct sockaddr *)&fsin, sizeof(fsin)); // Sending peer server port
					break;
				}	
			}
			if (done == 0){
				spdu.type = 'E';
				strcpy(spdu.data,"Registered content not found");
				(void)sendto(s,&spdu, BUFSIZE, 0,
					(struct sockaddr *)&fsin, sizeof(fsin));
			}
			
		}
		else if(spdu.type == 'T'){ // Content De-Registration
			char c_name[20], file[20];
			strcpy(user,spdu.data);
			recvfrom(s, &c_name, BUFSIZE, 0, (struct sockaddr *)&fsin, &alen); // receive content name
			//recvfrom(s, &file, BUFSIZE, 0, (struct sockaddr *)&fsin, &alen); // receive file name
			bzero(spdu.data, BUFSIZE); 
			if(n_cont != 0) {
				for (i=0; i<n_cont; i++){
					if (strcmp(list[i].usr, user) == 0){
						if (strcmp(list[i].name, c_name) == 0) {
							for(j = i; j < n_cont - 1; j++) {
								list[j] = list[j + 1];
							}
							strcpy(list[n_cont-1].usr, "\0");
							strcpy(list[n_cont-1].name, "\0");
							strcpy(list[n_cont-1].f_name, "\0");
							list[n_cont-1].addr = '\0';
							list[n_cont-1].port = '\0';
							n_cont--;
							spdu.type = 'T';
							sprintf(spdu.data, " Content: %s deregistered successfully.\n", c_name);
						}
					}
					else {
						spdu.type = 'E';
						strcpy(spdu.data, " Error: User not found.");
					}
				}
			}
			else {
				spdu.type = 'E';
				strcpy(spdu.data, " Error: There is no content registered.");
			}
			(void)sendto(s, &spdu.data, strlen(spdu.data), 0, 
						(struct sockaddr *)&fsin, sizeof(fsin));
		}

		else if(spdu.type == 'O'){ // Content Data
			spdu.type = 'O';
			bzero(spdu.data, BUFSIZE);
			sprintf(spdu.data, "%d", n_cont);
			(void)sendto(s, &spdu, BUFSIZE, 0,
					(struct sockaddr *)&fsin, sizeof(fsin));
			for (i=0; i<n_cont; i++) {
				spdu.type = 'O';
				bzero(spdu.data, BUFSIZE);
				sprintf(spdu.data, "Owner: %s, Content Name: %s, File: %s, Address: %d:%d\n", list[i].usr, list[i].name, list[i].f_name, list[i].addr, list[i].port);
				//sprintf(spdu.data, "Owner: %s, File: %s, Address: %d:%d\n", list[i].usr, list[i].name, list[i].addr, list[i].port);
				(void)sendto(s, &spdu, BUFSIZE, 0,
					(struct sockaddr *)&fsin, sizeof(fsin));
			}
		}

		else if(spdu.type == 'Q'){ // Quit
			for(i = 0; i < n_cont; i++) {
				strcpy(list[n_cont-1].usr, "\0");
				strcpy(list[n_cont-1].name, "\0");
				strcpy(list[n_cont-1].f_name, "\0");
				list[n_cont-1].addr = '\0';
				list[n_cont-1].port = '\0';
				n_cont--;
			}
		}
		
	}
}



