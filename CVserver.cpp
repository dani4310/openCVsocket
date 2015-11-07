#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, width, height;
     socklen_t clilen;
     char buffer[1024];
     char messagerec[30] = "";
     char *mesptr = messagerec;
     struct sockaddr_in serv_addr, cli_addr;
     int n;



     if (argc < 2) {
         printf("Usage: ./CVserver <server port>\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");
     bzero(buffer,1024);

     char c;
    while(1){
        if ( read(newsockfd, &c, 1) == 1) {
             *mesptr ++=c;
             if (c == ']')
                break;
        } 
    }
    mesptr++;
    *mesptr = '\0';
    width = atoi(strtok(messagerec, "|"));
    height = atoi(strtok(NULL,"]"));



    Mat  img = Mat::zeros( height,width, CV_8UC3);
    int  imgSize = img.total()*img.elemSize();
    char sockData[imgSize];
    int bytes;
 //Receive data here
while(1){
   for (int i = 0; i < imgSize; i += bytes) {
        if ((bytes = recv(newsockfd, sockData +i, imgSize  - i, 0)) == -1) {
            printf("recv failed\n");
        }
   }

 // Assign pixel value to img

 int ptr=0;
    for (int i = 0;  i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {                                     
            img.at<cv::Vec3b>(i,j) = cv::Vec3b(sockData[ptr+ 0],sockData[ptr+1],sockData[ptr+2]);
            ptr=ptr+3;
        }
    }

     namedWindow( "Server", CV_WINDOW_AUTOSIZE );// Create a window for display.
     imshow( "Server", img );  
     waitKey(30);
 }
    close(newsockfd);
    close(sockfd);
     return 0; 
}