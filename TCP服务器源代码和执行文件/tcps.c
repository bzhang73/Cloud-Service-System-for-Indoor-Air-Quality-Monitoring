#include<stdlib.h>

#include<stdio.h>

#include<sys/socket.h>

#include<netdb.h>

#include<string.h>

#include<unistd.h>

#include<netinet/in.h>

#include<arpa/inet.h>

#include<signal.h>

//mongodb_c_drive
#include <bson.h>
#include <bcon.h>
#include <mongoc.h>



#define PORT 8900



int execute(char* command,char* buf);



void sig_child(int signo)

{

	pid_t pid;

	int stat, i;

	while((pid=waitpid(-1,&stat,WNOHANG))>0);



	return;

}





int main(int argc, char* argv[])

{
	//mongo-c-drive
	 mongoc_client_t* mongoclient;
    mongoc_database_t* database;
    mongoc_collection_t* collection;
    bson_t *command, reply, *insert;
    bson_error_t error;
    char* str;
    bool retval;


    struct sockaddr_in server;

	struct sockaddr_in client;

	int len;

	int port;

	int listend;

	int connectd;

	int sendnum;

	int opt;

	int recvnum;

	char send_buf[2048];

	char recv_buf[2048];

	char cmd[2048];

	pid_t pid;

	

    port= PORT;

	memset(send_buf,0,2048);

	memset(recv_buf,0,2048);

	

    opt = SO_REUSEADDR;



	signal(SIGCHLD,sig_child);

      

    if (-1==(listend=socket(AF_INET,SOCK_STREAM,0)))

    {

        perror("create listen socket error\n");

	    exit(1);

    }

    setsockopt(listend,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));



    memset(&server,0,sizeof(struct sockaddr_in));

    server.sin_family = AF_INET;

    server.sin_addr.s_addr = htonl(INADDR_ANY);

    server.sin_port = htons(port);



    if (-1==bind(listend,(struct sockaddr *)&server,sizeof(struct sockaddr)))

    {

        perror("bind error\n");

	    exit(1);

    }



    if (-1==listen(listend,5))

    {

        perror("listen error\n");

	    exit(1);

    }



    while(1)

    {

        memset(recv_buf,0,2048);

		memset(send_buf,0,2048);



        if (-1==(connectd=accept(listend,(struct sockaddr*)&client,&len)))

    	{

            perror("create connect socket error\n");

		    continue;

    	}
	printf("client connected ...\n");

    

		if(fork()==0)

		{

			close(listend);

				while(1)

			{
			  memset(send_buf,0,2048);

				memset(recv_buf,0,2048);

			if (0>(recvnum = recv(connectd,recv_buf,sizeof(recv_buf),0)))

			{

				perror("recv error\n");

				close(connectd);

				continue;

			}

			else if(recvnum==0)

			{

				close(connectd);

				exit(0);

			}

			

			recv_buf[recvnum]='\0';

			



			if (0==strncmp(recv_buf,"quit",4))

			{

				//perror("quitting remote controling\n");

				break;



			}
		        else
			{
			//	printf("%s\n", recv_buf);
				char tp[32];
				char valueS[32];
				int value;
				int flag = 0;

				int i;
				int j;

				for(i = 0; i < recvnum; i++)
				{	
				//	printf("%c %d", recv_buf[i], i);					

					if(recv_buf[i] == ' ' && flag == 0)
					{
						flag = 1;
						continue;
						//printf("flag = 1\n");
					}
					if(flag == 1 && recv_buf[i] != ' ')
					{
						tp[i - 3] = recv_buf[i];
						continue;
						//printf("%c", tp[i - 3]);
					}
					if(recv_buf[i] == ' ' && flag == 1)
					{
						flag = 2;
						tp[i - 3] = '\0';
						continue;
					}
					if(recv_buf[i] == ':')
					{
						i++;
						for(j = i; j < recvnum; j++)
						{
							valueS[j - i] = recv_buf[j];
						}
						value = atoi(valueS);
					}
				}

				printf("value:%s  %d\n", tp, value);
//mongo-c-drive
	mongoc_init();
    mongoclient = mongoc_client_new("mongodb://localhost:27017");
    database = mongoc_client_get_database(mongoclient, "nodedb");
    collection = mongoc_client_get_collection(mongoclient, "nodedb", tp);
    command = BCON_NEW("ping", BCON_INT32(1));
    retval = mongoc_client_command_simple(mongoclient,"root",command, NULL , &reply, &error);


	 if(!retval)
    {
        fprintf(stderr, "%s\n", error.message);
        return EXIT_FAILURE;
    }

	//mongo-c-drive
	str = bson_as_json(&reply, NULL);
    printf("%s\n", str);
    insert = BCON_NEW("name", BCON_UTF8(tp), "value", BCON_INT32(value));



    if(!mongoc_collection_insert(collection, MONGOC_INSERT_NONE, insert, NULL, &error))
    {
        fprintf(stderr, "%s\n", error.message);
    }
				 
			}
			//printf("the message is:\n %s\n",recv_buf);
			
		       // send(connectd,recv_buf,strlen(recv_buf),0);

			}

		}

		else

		{

			close(connectd);
			
			


		}



		

        

		

		if (0==strncmp(recv_buf,"quit",4))

		{

			perror("quitting remote controling\n");

			printf("Client quit\n");

			send(connectd,recv_buf,sizeof(recv_buf),0);

			close(connectd);

			exit(0);



		}



	//	close(connectd);

		

    }



    close(listend);

			bson_destroy(insert);
			bson_destroy(&reply);
			bson_destroy(command);
			bson_free(str);

			mongoc_collection_destroy(collection);
			mongoc_database_destroy(database);
			mongoc_client_destroy(mongoclient);
			mongoc_cleanup();
    return 0;

}




