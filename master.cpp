#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <omp.h>
#include <stack>
#include "BufferQueue.h"
#include "BufferQueueReq.h"
#include "mpi.h"

using namespace std;

char response[1024];
char buf_request[1024];
char bad_request[] = "HTTP/1.1 404 NOT FOUND\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html><html><head><title>Incredible</title>"
"<style>body { background-color: #111 }"
"h1 { font-size:4cm; text-align: center; color: black;"
" text-shadow: 0 0 2mm red}</style></head>"
"<body><h1>Not Found</h1><br><h2>WWW.EISTI.FR</h2></body></html>\r\n";



int main (int argc, char *argv[]) {
	//Conversation socket buffer
	BufferQueue<unsigned int> *client_socket_buffer = new BufferQueue<unsigned int>(5);
	cout<<*client_socket_buffer;
	
	//Request buffer
	BufferQueueReq<char> *request_buffer = new BufferQueueReq<char>(5);
	cout<<*request_buffer;
	
	unsigned int server_socket_fd, client_socket_fd, port_number, client_length;
	char buffer[1024];
	struct sockaddr_in server_addr, client_addr;
	
	//Creating socket
	server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	//Verifying correct socket initialization	
	if (server_socket_fd < 0) {
		cerr<<"Error"<<endl;
		return 1;
	} 
	
	//cleaning up sever address structure by setting all the bytes to 0
	bzero((char *) &server_addr, sizeof(server_addr));
	
	//setting the server address properties
	server_addr.sin_family = AF_INET;
	
	//setting the port number
	port_number = 5000;
	
	//htons converts the integer to a network byte order
	server_addr.sin_port = htons(port_number);
	//setting sever address value to an IP of a hosting machine
	server_addr.sin_addr.s_addr = INADDR_ANY;
	
	//binding the socket to the initialized address
	unsigned int bind_check = bind(server_socket_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
	if (bind_check < 0) {
		cerr<<"Error binding the socket to the local address"<<endl;
		return 1;
	}
	
	//listening to incoming connections
	listen(server_socket_fd, 7);
	
	
	//infinite loop on read and write
	for (;;) {
		
		//parallelization on read and write
		#pragma omp parallel
		{
			//getting the client structure length that is needed to call accept function, all the later communication will be carried out via client socket file descriptor	
			client_length = sizeof(client_addr);

			//accepting connection, creating file descriptor of the convesation	
			client_socket_fd = accept(server_socket_fd, (struct sockaddr*) &client_addr, &client_length);
			cout<<"Connection established"<<endl;
			if (client_socket_fd < 0) {
				cerr<<"Error accepting the connection"<<endl;
			}

			int thread_id;	
			//getting the current thread ID
			thread_id = omp_get_thread_num();
			cout<<"Thread id: "<<thread_id<<endl;

			//saving conversation file descriptor to the BufferQueue
			#pragma omp critical
				client_socket_buffer->push(client_socket_fd);

			//cleaning the buffer up
			bzero(buffer, 1024);

			//obtaining number of threads
			int num_threads = omp_get_num_threads();
			cout<<"Number of threads: "<<num_threads<<endl;

			//reading data from the socket
			unsigned int communication_check = read(client_socket_fd, buffer, sizeof(buffer) - 1);
	
			if (communication_check < 0) {
				cerr<<"Error reading data from the socket"<<endl;
			}
	
			//printing out a client request
			cout<<"Client request"<<endl;
			cout<<buffer<<endl;
			
			//pushing buffer into the BufferQueue
			#pragma omp critical
				request_buffer->push(buffer);
			//cout<<"Request buffer:"<<request_buffer->pop()<<endl;
			cout<<"End of request buffer"<<endl;
		
			//processing client request with string tokenizer to detect get fevicon.ico request
			char *line = strtok(buffer, "\r\n");
			cout<<"First line: "<<line<<endl;

			if (strcmp(line, "GET /favicon.ico HTTP/1.1")== 0) {
				communication_check = write (client_socket_fd, bad_request, sizeof(bad_request));
				if (communication_check < 0) {
					cerr<<"Error writing data from the socket"<<endl;
				}
				close(client_socket_fd);
			}

			//Sending data to a remote server via MPI
			MPI_Init(&argc, &argv);
			// Look up for server's port name.
			char port_name[MPI_MAX_PORT_NAME];
			MPI_Lookup_name("name", MPI_INFO_NULL, port_name);
   
			// Connect to server.
			MPI_Comm server;
			MPI_Comm_connect(port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &server);

			// Sending file descriptor to server with tag = 777
			//int sendbuf = client_socket_fd;
			//MPI_Send(&sendbuf, 1, MPI_INT, 0, 777, server);
			
			// Sending request to server with tag = 555
			strcpy (buf_request,request_buffer->pop());
			cout<<"Buf request"<<buf_request<<endl;
			MPI_Send(buf_request, sizeof(buf_request), MPI_CHAR, 0, 555, server);
			
			// Receiving server response size with tag = 111
			int response_size;
			MPI_Recv(&response_size, 1, MPI_INT, 0, 111, server, MPI_STATUS_IGNORE);
			// Receiving server response with tag = 888
			MPI_Recv(&response, response_size, MPI_CHAR, 0, 888, server, MPI_STATUS_IGNORE);
			cout<<"server response = "<<response<<endl;
   
			MPI_Finalize();
	
			communication_check = write (client_socket_fd, response, sizeof(response));
	
			if (communication_check < 0) {
				cerr<<"Error writing data from the socket"<<endl;
			}
			//output of the queue
			cout<<"Inside converstation file buffer: "<<*client_socket_buffer;
		}
	}
	//closing the conversation file
	close(client_socket_fd);
	
	return 0;
}
