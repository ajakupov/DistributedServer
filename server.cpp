#include "mpi.h"
#include <iostream>

using namespace std;

char response[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html><html><head><title>Incredible</title>"
"<style>body { background-color: #000000}"
"h1 { font-size:4cm; text-align: center; color: black;"
" text-shadow: 0 0 2mm red}</style></head>"
"<body><h1>EISTI</h1><br><h2>WWW.EISTI.FR</h2></body></html>\r\n";

char buf_request[1024];

int main(int argc, char *argv[])
{  
	MPI_Init(&argc, &argv);

	// Open port.
	char port_name[MPI_MAX_PORT_NAME];
	MPI_Open_port(MPI_INFO_NULL, port_name);

	// Publish port name and accept client.
	MPI_Comm client;
	MPI_Publish_name("name", MPI_INFO_NULL, port_name);
	MPI_Comm_accept(port_name, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &client);
   
	// Receive data from client.
	int recvbuf_fd;
	//MPI_Recv(&recvbuf_fd, 1, MPI_INT, 0, 777, client, MPI_STATUS_IGNORE);

	//Receiving request
	MPI_Recv(&buf_request, 1024, MPI_CHAR, 0, 555, client, MPI_STATUS_IGNORE);
	cout<<"file descriptor = "<<recvbuf_fd<<endl;
	cout<<"request = "<<buf_request<<endl;

	int response_size = sizeof(response);

	//Sending response size with tag 111
	MPI_Send(&response_size, 1, MPI_INT, 0, 111, client);

	//Sending response with tag 888
	MPI_Send(response, sizeof(response), MPI_CHAR, 0, 888, client);

	MPI_Unpublish_name("name", MPI_INFO_NULL, port_name);

	MPI_Finalize();
	return 0;
}
