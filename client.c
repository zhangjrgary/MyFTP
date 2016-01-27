#include "header.h"

#define DOWNLOAD_PATH "./download/"		//规定下载文件夹路径

int processList(SA_in server_addr);
int processGet(char *str,SA_in server_addr);
int processPut(char *str,SA_in server_addr);
int processList(SA_in server_addr);
void processHelp();
void createDownloadDir();

int main(void)
{
	createDownloadDir();	
	int connect_fd;
	SA_in server_addr;	
	//XXX:	
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=inet_addr(SERVER_IP);
	server_addr.sin_port=htons(SERVER_PORT);
	//XXX
	
	connect_fd=socket(AF_INET,SOCK_STREAM,0);	
	char buf[BUFSIZE];
	while(1)
	{
		printf("<client >");
		memset(buf,'\0',BUFSIZE);
		fgets(buf,BUFSIZE,stdin);
		buf[strlen(buf)-1]='\0';
		if(strncmp("quit",buf,4)==0)
		{
			printf("退出\n");
			exit(0);
		}
		else if(strncmp("help",buf,4)==0)
		{
			processHelp();
		}
		else if(strncmp("list",buf,4)==0)
		{
			processList(server_addr);
		}
		else if(strncmp("get",buf,3)==0)
		{			
			processGet(buf,server_addr);
		}
		else if(strncmp("put",buf,3)==0)
		{
			processPut(buf,server_addr);
		}
		else
		{
			processHelp();
		}
		
	}	
	return 0;
}

void processHelp()
{
	printf("------帮助手册-------\n");
	printf("help:打印帮助信息\n");	
	printf("list:获取服务器可供下载的文件列表\n");
	printf("get:下载服务器文件\n");
	printf("put:上传文件给服务器\n");
	printf("quit:退出\n");
	printf("------帮助手册-------\n");
}

int processList(SA_in server_addr)
{
	char path[PATH_LEN]=DOWNLOAD_PATH;	
	char buf[BUFSIZE];
	int connect_fd;
	int count=0;	
	socklen_t len=sizeof(server_addr);				
	connect_fd=socket(AF_INET,SOCK_STREAM,0);
	if(connect(connect_fd,(SA *)&server_addr,len)==-1)
	{
		perror("connect fail");
		exit(-1);			
	}
	buf[0]='L';
	send(connect_fd,buf,BUFSIZE,0);			
	printf("----------当前可供下载的文件-------\n");	
	while(1)
	{
	memset(buf,'\0',BUFSIZE);
	count=recv(connect_fd,buf,BUFSIZE,0);
	if(count==0)
	break;	
	printf("%s",buf);
	}
	printf("----------当前可供下载的文件-------\n");	
	return 0;
}
int processPut(char *str,SA_in server_addr)//必须要指定绝对路径
{		
	char path[PATH_LEN];
	char filename[PATH_LEN];
	char *check_spcae=str+4;
	while(*check_spcae==' ') check_spcae++;	
	sprintf(path,"%s",check_spcae);		
	int i;
	for(i=strlen(str);i!=2&&str[i]!='/';i--);
	if(i==2)
	{
		printf("提醒:当前文件夹必须为./形式\n");
		return -1;
	}
	strcpy(filename,&str[i+1]);
	struct stat st;
	lstat(path,&st);
	if(!S_ISREG(st.st_mode))
	{
		printf("提醒:上传的必须是文件\n");
		return -1;
	}
	
	char buf[BUFSIZE];
	int connect_fd,file_fd;
	int count=-1;
	socklen_t len=sizeof(server_addr);			
	if((file_fd=open(path,O_RDONLY))==-1)
	{
		perror("文件不存在,或不可用");
		return -1;
	}
	
	connect_fd=socket(AF_INET,SOCK_STREAM,0);
	if(connect(connect_fd,(SA *)&server_addr,len)==-1)
	{
		perror("connect fail");
		return -1;		
	}
	sprintf(buf,"P%s",filename);
	send(connect_fd,buf,BUFSIZE,0);	
	recv(connect_fd,buf,BUFSIZE,0);
	if(buf[0]=='E')
	{
		printf("远端服务器已存在此文件\n");		
		return -1;
	}
	//发送文件模块	
	printf("开始发送文件%s\n",filename);	
	sleep(1);
	while(1)
	{
	memset(buf,'\0',BUFSIZE);
	count=read(file_fd,buf,BUFSIZE);
	if(count==0)
	break;
	send(connect_fd,buf,count,0);	//send与write一样都是有多少写多少	
	}
	close(file_fd);	
	if(count==-1)
	printf("文件%s上传失败\n",filename);
	//发送文件模块
	printf("文件%s上传成功\n",filename);		
	return 0;
}

int processGet(char *str,SA_in server_addr)
{
	char path[PATH_LEN]=DOWNLOAD_PATH;	
	char filename[PATH_LEN];
	char *check_spcae=str+4;
	while(*check_spcae==' ') check_spcae++;
	sprintf(filename,"%s",check_spcae);
	char buf[BUFSIZE];
	int connect_fd,file_fd;
	int count;	
	socklen_t len=sizeof(server_addr);	
	strcat(path,filename);	
		
	connect_fd=socket(AF_INET,SOCK_STREAM,0);
	if(connect(connect_fd,(SA *)&server_addr,len)==-1)
	{
		perror("connect fail");
		return -1;		
	}
	sprintf(buf,"G%s",filename);
	send(connect_fd,buf,BUFSIZE,0);		
	//接收文件模块	
	recv(connect_fd,buf,BUFSIZE,0);
	if(buf[0]=='E')
	{
		printf("远端服务器没有此文件\n");		
		return -1;
	}	
	if((file_fd=open(path,O_WRONLY|O_CREAT|O_EXCL,0644))==-1)
	{
		perror("该文件已存在,接收失败,请重试");		
		return -1;
	}
	printf("开始接收文件%s\n",filename);		
	while(1)
	{
	memset(buf,'\0',BUFSIZE);
	count=recv(connect_fd,buf,BUFSIZE,0);
	if(count==0)
	break;
	
	write(file_fd,buf,count);	
	}
	close(file_fd);	
	printf("文件%s接收成功\n",filename);
	//接收文件模块	
	return 0;
}

void createDownloadDir()
{
	mkdir("./download",0755);
	printf("默认下载路径为当前文件夹下download文件夹\n");
}







