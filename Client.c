#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>




int main(int argc,char *argv[])
{  
 //-------------variable declaration------------------------
   struct sockaddr_in serv_addr;
   char buf[1024];
   int fd1,fd2,port_no;

   
 //----------------------------------------------------------------
   if(argc!=3)
  {
  	printf("Incorrect arguments error.\n");
  	printf("Please Enter three command line arguements in this format\n");
  	printf("<filename port_number IP_Addr>\n");
        exit(0);
  }

  fd1=socket(AF_INET, SOCK_STREAM, 0);
  if(fd1==-1)
  {
  printf("Error in creating socket at client\n");
  exit(0);
  }
  printf("Step1: Socket created at client succesfully\n");


  port_no=atoi(argv[1]);
  char ip[50];
  strcpy(ip,argv[2]);
  serv_addr.sin_addr.s_addr = inet_addr(ip);
  
  serv_addr.sin_family = AF_INET;
  //16 bit host Byte order to network byte order
  serv_addr.sin_port = htons(port_no);
  
  fd2=connect(fd1, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if(fd2==-1)
  {
  printf("Error in connectin socket to server\n");
  exit(0);
  }
  printf("Step2: Socket connected with server succesfully\n");
  printf("Step3: Now client can enter the commands\n");
//------------------------------------------------------------------------------
//--------------Now Client will Enter Commands----------------------------------
  while(1)
{
    printf("\033[22;32m$$>\033[0m");
    scanf("%s", buf);
    send(fd1, buf, strlen(buf), 0);
//-----------------close command -----------------------------------------------      
    if(strcmp(buf,"close") == 0)
    {
      close(fd1);
      printf("Disconnected from server.\n");
      exit(1);
    }
//-------------lls command -----------------------------------------------------
    else if(strcmp(buf, "lls") == 0)
    {
    printf("Listing client's current directory files\n\n");
    DIR*p;
    struct dirent *d;
    p=opendir(".");
      while(d=readdir(p))
      printf("%s\t\t",d->d_name);
    printf("\n");
    continue;
    }
//-------------ls command -----------------------------------------------------     
    else if(strcmp(buf, "ls") == 0){
    char *pass = malloc(10*sizeof(char));
    char *buf = malloc(1024*sizeof(char));
    int counter;
    read(fd1,pass,sizeof(pass));
    counter=atoi(pass);
    printf("Listing Servers current directory files\n\n");
    while (counter!=0)
    {
      read(fd1,buf,1024);
      printf("%s\t\t",buf);
      counter--;
    } 
     printf("\n");
      continue;
    }
//-------------lcd command -----------------------------------------------------
    else if(strcmp(buf, "lcd") == 0)
    {
    	char *dir=malloc(100*sizeof(char));
        printf("Enter the directory name: ");
        scanf("%s",dir);
	const char *d= getenv("HOME"); 
        if (!strcmp(dir,"home")) 
	{
	const char *t= getenv("HOME"); 
	chdir(t);
 	printf("Currently in home directory\n");
	continue;
	}
	else if(!strcmp(dir,"..")) 
	{
	chdir(dir);
	printf("Came out from current directory\n");
 	continue;
	}
	else if (chdir(dir) == -1) 
	{
	printf("No such directory named %s\n", dir);
        continue;
	}
	else
	{
	char *cdir = getenv("HOME");
	strcat(cdir,"/");
        strcat(cdir,dir);
	chdir(cdir);
	printf("Current Directory: %s\n",cdir);
        }
     continue;
    }
//-------------cd command -----------------------------------------------------
    else if(strcmp(buf, "cd") == 0)
    {
	char *dir=malloc(100*sizeof(char));
	char *buffer= malloc(200*sizeof(char));
        printf("Enter the directory name: ");
        scanf("%s",dir);
        send(fd1,dir,sizeof(dir),0);
	recv(fd1,buffer,sizeof(buffer),0);
	printf("%s\n",buffer);
    continue;
    }
//-------------lchmod command -----------------------------------------------------    
    else if(strcmp(buf, "lchmod") == 0)
    {
      char *mode=malloc(10*sizeof(char));
      char *file=malloc(1024*sizeof(char));
      printf("Enter the mode: ");
      scanf("%s",mode);
      strtok(mode,"\n");
      printf("Enter the file name: ");
      scanf("%s",file);
      strtok(mode,"\n");
      int i = strtol(mode, 0, 8);
      int j = chmod (file,i);
      if ( j < 0)
      {
      printf("Error in changing permission\n");
      continue;
      }
      else
      {
      printf("Permission changed succesfully\n");
      }
    continue;
    }
//-------------chmod command -----------------------------------------------------  
    else if(strcmp(buf, "chmod") == 0)
    {
     char *mode=malloc(10*sizeof(char));
     char *file=malloc(1024*sizeof(char));
     char *buf2=malloc(1024*sizeof(char));
     printf("Enter the mode : ");
     scanf("%s",mode);
     strtok(mode,"\n");
     write(fd1,mode,256);
     printf("Enter the file name: ");
     scanf("%s",file);
     strtok(mode,"\n");
     write(fd1,file,256);
     read(fd1,buf2,256);
     printf("%s",buf2);
    continue;
    }
 
return 0;
}


//----------------------Client Program Ends Here---------------------------------
