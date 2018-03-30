#include <stdio.h>
#include <sys/socket.h>//for sockets
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>//for chdir
#include <sys/types.h>//fork
#include <sys/stat.h>//setting mode of the file
#include <dirent.h>//for dirent in ls and lls command
#define listenQ 100


int main(int argc,char *argv[])
{
  //-------------variable declaration------------------------
  struct sockaddr_in serv_addr;
  int fd1,fd2,fd3,fd4,port_no,n;
  pid_t childpid;
  struct sockaddr_in c_addr;
  socklen_t clen=0;
  
  //----------------------------------------------------------------
  if(argc!=2)
  {
  	printf("Incorrect arguments error.\n");
  	printf("Please Enter two command line arguements in this format\n");
  	printf("<filename port_number>\n");
        exit(0);
  }
  
  //creating socket..AF_INET for IPv4 family,SOCK_STREAM
  // for Byte stream socket,0 for protocol
  fd1=socket(AF_INET, SOCK_STREAM, 0);
  if(fd1==-1)
  {
  printf("Error in creating socket at server\n");
  exit(0);
  }
 
  printf("Step1: Socket created at server succesfully\n");

  //converting char to integer
  port_no=atoi(argv[1]);
  
  serv_addr.sin_family = AF_INET;
  //32 bit host Byte order to network byte order
  //INADDR_ANY used to bind socket to all available interfaces/IPs
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  //16 bit host Byte order to network byte order
  serv_addr.sin_port = htons(port_no);
   
  //bind function binds IP address(32 or 128 bit) with 16-bit TCP port_number
  //fd1 is socket descriptor
  fd2=bind (fd1, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if(fd2==-1)
  {
  	printf("Error in binding Port and IP\n");
  	exit(0);
  }
  printf("Step2: Binding done succesfully\n");
  
  //listenQ=atoi(argv[2]);
  //converts an unconnected socket into a passive socket, 
  //indicating that the kernel should accept incoming connection
  // requests directed to this socket.
  fd3=listen(fd1,listenQ);
  if(fd3==-1)
  {
  	printf("Error in listening requests\n");
  	exit(0);
  }
  printf("Step3:passive connection established,Client can now connect to it..\n");
 
 
while(1){
    fd4 = accept(fd1, (struct sockaddr*)&c_addr, &clen);
    if(fd4 == -1)
    {
      printf("Error in accepting Request from Client\n");
      exit(0);
    }
    printf("Step4:Connection is established from client with ip %s and port %d\n", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
    printf("Step5: Now client can enter commands\n");

//------------------------------------------------------------------------------
//--------------Now Client's request will be served-----------------------------

    if((childpid = fork()) == 0)//forking for multiple clients
    {
      close(fd1);
      while(1){
        bzero(buf, sizeof(buf));
        recv(fd4, buf, 1024, 0);
        printf("\033[22;32mCommand Received from client is :\033[0m %s\n",buf);
//-----------------close command -----------------------------------------------     
        if(strcmp(buf,"close") == 0)
        {
          printf("Server  with ip %s and port no. %d is Closing connection for clients\n", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
          break;
        }
//-------------lls command -----------------------------------------------------
        else if(strcmp(buf,"lls") == 0)
        {
        printf("Local command : ""lls"" is running on client\n");
        continue;
        }
//-------------ls command -----------------------------------------------------       
        else if(strcmp(buf, "ls") == 0)
        {
        printf("Server command : ""ls"" is running on Server and Response is sent to client\n");
        DIR*t;
        char *pass = malloc(10*sizeof(char));
        int counter=0;
        struct dirent *d;
        t=opendir(".");
          while(d=readdir(t))
          {
          counter++;
          }
        sprintf(pass,"% d",counter);
        write(fd4,pass,sizeof(pass));
        t=opendir(".");
          while (counter!=0)
          {
          d=readdir(t);
          write(fd4,d->d_name,1024);
          counter--;
          }
        continue;
        }
//-------------lcd command -----------------------------------------------------
        else if(strcmp(buf, "lcd") == 0)
        {
        printf("Local command : ""lcd"" is running on client\n");
        continue;
        }
//-------------cd command -----------------------------------------------------
    
        else if(strcmp(buf, "cd") == 0)
        {
	char *dir=malloc(100*sizeof(char));
        char *buffer= malloc(200*sizeof(char));
        recv(fd4,dir,sizeof(dir),0);
	const char *d= getenv("HOME"); 

         if (!strcmp(dir,"home")) 
	{
	chdir(d);
 	//system("ls");
        send(fd4,d,sizeof(d),0);
	continue;
	}
	else if(!strcmp(dir,"..")) {
	printf("%s\n",dir);
	strtok(dir,"/");
	printf("After strtok %s",dir);
	chdir(dir);
 	system("ls");
	send(fd4,buffer,sizeof(buffer),0);
	continue;
	}
	else if (chdir(dir) == -1) 
	{
        strcpy(buffer,"No such directory\n");
        send(fd4,buffer,sizeof(buffer),0);
        continue;
	}
	else
	{
	char *cdir = getenv("HOME");
	strcat(cdir,"/");
        strcat(cdir,dir);
	printf("%s\n",cdir);
        chdir(cdir);
	//system("ls");
	send(fd4,cdir,sizeof(cdir),0);
        
	}
        continue;
        }
//-------------lchmod command -----------------------------------------------------    
        else if(strcmp(buf, "lchmod") == 0)
        {
        printf("Local command : ""lchmod"" is running on client\n");
        continue;
        }
//-------------chmod command -----------------------------------------------------  
        else if(strcmp(buf, "chmod") == 0)
        {
        char *mode=malloc(10*sizeof(char));
        char *file=malloc(1024*sizeof(char));
        char *buf2=malloc(1024*sizeof(char));
        read(fd4,mode,256);
        read(fd4,file,256);
        int i = strtol(mode, 0, 8);
        int j = chmod (file,i);
         if ( j< 0)
          {
          strcpy(buf2,"Error in channging permission\n");
          write(fd4,buf2,256);
          continue;
          }
         else
          {
          strcpy(buf2,"Permission changed succesfully\n");
          write(fd4,buf2,256);
          }
 	      printf("\n");
        continue;
        }

close(fd4);//Closing listening socket.
return 0;
}

//----------------------Server Program Ends Here---------------------------------
