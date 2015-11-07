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
    exit(0);
}
char *itoa(int num, char *str, int radix);
int main(int argc, char *argv[])
{
    int sockfd, portno, n, cameradevice;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    double width,height,realwidth,realheight;
    char buffer[256];
    char videomessage[30]="";
    char numberbuf[10];

    if (argc < 4) {
        printf("Usage: ./CVclient <server ip> <server port> <video width> <video hight> <camera device>\n");
        exit(0);
    }


    width = (double)atoi(argv[3]);
    height = (double)atoi(argv[4]);
    cameradevice = atoi(argv[5]);
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    VideoCapture cap(cameradevice);  
    if(!cap.isOpened())  
    {  
        printf("error");
        return -1;  
    }  
    cap.set(CV_CAP_PROP_FRAME_WIDTH,width);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,height);
    realwidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    realheight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); 
    itoa (realwidth,numberbuf,10);
    strcat(videomessage,numberbuf);
    strcat(videomessage,"|");
    itoa (realheight,numberbuf,10);
    strcat(videomessage,numberbuf);
    strcat(videomessage,"]");
 
    printf("width = %f, height = %f\n",realwidth,realheight);
    n = write(sockfd,videomessage,strlen(videomessage));
    if (n < 0) 
         error("ERROR writing to socket");

    
    Mat image;
    while(1){
    cap>>image; 
    image = (image.reshape(0,1));
    int  imgSize = image.total()*image.elemSize();
    // image = imread(argv[3], CV_LOAD_IMAGE_COLOR);
    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    n = write(sockfd,image.data,imgSize);
    if (n < 0) 
         error("ERROR writing to socket");

 
    waitKey(15);
    }
        close(sockfd);
    return 0;
}

char *itoa(int num, char *str, int radix) {
    char sign = 0;
    char temp[17];  //an int can only be 16 bits long
                    //at radix 2 (binary) the string
                    //is at most 16 + 1 null long.
    int temp_loc = 0;
    int digit;
    int str_loc = 0;

    //save sign for radix 10 conversion
    if (radix == 10 && num < 0) {
        sign = 1;
        num = -num;
    }
    
    //construct a backward string of the number.
    do {
        digit = (unsigned int)num % radix;
        if (digit < 10) 
            temp[temp_loc++] = digit + '0';
        else
            temp[temp_loc++] = digit - 10 + 'A';
        num = (((unsigned int)num) / radix);
    } while ((unsigned int)num > 0);

    //now add the sign for radix 10
    if (radix == 10 && sign) {
        temp[temp_loc] = '-';
    } else {
        temp_loc--;
    }


    //now reverse the string.
    while ( temp_loc >=0 ) {// while there are still chars
        str[str_loc++] = temp[temp_loc--];    
    }
    str[str_loc] = 0; // add null termination.

    return str;
}