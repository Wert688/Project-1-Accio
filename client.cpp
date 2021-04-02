#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

#include <iostream>
#include <sstream>
#include <fstream>
#define MAX 108000000

int
main(int argc, char *argv[])
{
  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // struct sockaddr_in addr;
  // addr.sin_family = AF_INET;
  // addr.sin_port = htons(40001);     // short, network byte order
  // addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  // memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));
  // if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
  //   perror("bind");
  //   return 1;
  // }


  int port;
  port = atoi(argv[2]);
  if (port > 65535)
  {
	  perror("bad port");
	  return 1;
  }
  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);     // short, network byte order
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

  // connect to the server
  if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
	//std::cerr << "ERROR: Failed to connect to the server"
    perror("connect");
    return 2;
  }

  struct sockaddr_in clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  if (getsockname(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1) {
    perror("getsockname");
    return 3;
  }

  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  std::cout << "Set up a connection from: " << ipstr << ":" <<
    ntohs(clientAddr.sin_port) << std::endl;


  // send/receive data to/from connection
  //bool isEnd = false;
  std::string input;
  char *buf = new char[MAX];
  int sentbytes = 1;
  int flaps = 0;
  FILE * sfile;
  long lsize;
  //std::stringstream ss;

  std::cout << "Got to here\n";
  if (!(sfile = fopen(argv[3], "r")))
  {
	  perror("file");
	  return 5;
  }
  fseek(sfile, 0, SEEK_END);
  lsize = ftell(sfile);
  rewind(sfile);

  std::cout << "file opened successfully\n";
  while (!feof(sfile))
  {
	  if ((sentbytes = fread(buf, sizeof(char), lsize, sfile)) > 0)
	  {
		  std::cout << "num abt2snd is :" + std::to_string(sentbytes) + "\n";
		  std::cout << "file read to buffer successfully\n" << std::flush;
		  flaps = send(sockfd, buf, sentbytes, 0);
		  std::cout << "num aactual is :" + std::to_string(flaps) + "\n";
	  }
	  else
		  break;
  }
  /*
  while (!isEnd) {
    //memset(buf, '\0', sizeof(buf));

    std::cout << "send: ";
    std::cin >> input;
    if (send(sockfd, input.c_str(), input.size(), 0) == -1) {
      perror("send");
      return 4;
    }


    if (recv(sockfd, buf, 20, 0) == -1) {
      perror("recv");
      return 5;
    }
    ss << buf << std::endl;
    std::cout << "echo: ";
    std::cout << buf << std::endl;

    if (ss.str() == "close\n")
      break;

    ss.str("");
  }*/
  delete[] buf;
  fclose(sfile);
  close(sockfd);

  return 0;
}