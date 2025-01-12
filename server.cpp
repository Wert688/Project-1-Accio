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

bool fileExists(const std::string& filename)
{
	std::ifstream infile(filename);
	return infile.good();
}



int main(int argc, char *argv[])
{
  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  int filecntr = 0;
  // allow others to reuse the address
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt");
    return 1;
  }
  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

  // bind address to socket

  int port;
  port = atoi(argv[1]);
  if (port > 65535)
  {
	  perror("bad port");
	  return 1;
  }
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);     // short, network byte order
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind");
    return 2;
  }

  // set socket to listen status
  if (listen(sockfd, 1) == -1) {
    perror("listen");
    return 3;
  }

  // accept a new connection
  struct sockaddr_in clientAddr;
  socklen_t clientAddrSize = sizeof(clientAddr);
  int clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);
  filecntr++;
  if (clientSockfd == -1) {
    perror("accept");
    return 4;
  }

  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  std::cout << "Accept a connection from: " << ipstr << ":" <<
    ntohs(clientAddr.sin_port) << std::endl;

  // read/write data from/into the connection
  //int filecntr = 1;

 // bool isEnd = false;
  FILE * pfile;
  
  char *buf = new char[MAX];
  //int loc = 0;
  int bytesread = 1;
  //std::size_t filesize = 1;

  std::cout << std::to_string(filecntr) + ".file\n";
  /*while (fileExists(std::to_string(filecntr) + ".file"))
  {
	  filecntr++;
  }*/

  std::string thefile = std::string(".") + argv[2]+ "/" + std::to_string(filecntr) + ".file";
  while (fileExists(thefile))
  {
	  filecntr++;
	  thefile = std::string(".") + argv[2] + "/" + std::to_string(filecntr) + ".file";
  }
  const char* thefchar = thefile.c_str();
  std::cout << thefchar + std::string("\n");
  pfile = fopen(thefchar, "w");
  std::cout << "num bytesread is :" + std::to_string(bytesread) + "\n";
  while (bytesread != 0)
  {

	  bytesread = recv(clientSockfd, buf, 4098, 0);
	  std::cout << "num bytesread is :" + std::to_string(bytesread) + "\n";
	  if (bytesread < 0)
	  {
		  perror("recv");
		  return 5;
	  }
	  //std::cout << "num bytestot is :" + std::to_string(bytetot) + "\n";
	  if (bytesread != 0)
	  {
		  fwrite(&buf, sizeof(char), bytesread, pfile);
	  }
  }
  
  std::cout << "file transferred\n";

  //set skeleton code aside
  /*char buf[20] = {0};
  bool isEnd = false;
  std::stringstream ss;

  while (!isEnd) {
    memset(buf, '\0', sizeof(buf));

    if (recv(clientSockfd, buf, 20, 0) == -1) {
      perror("recv");
      return 5;
    }

    ss << buf << std::endl;
    std::cout << buf << std::endl;
    if (send(clientSockfd, buf, 20, 0) == -1) {
      perror("send");
      return 6;
    }

    if (ss.str() == "close\n")
      break;
	

    ss.str("");
  }
  */
  delete[] buf;
  fclose(pfile);
  close(clientSockfd);
  
  return 0;
}