#include "header.h"

#define FTP_PATH "./ftp/"	//规定ftp共享文件夹路径

int processList();
int processGet(char *str);
int processPut(char *str);
void createFtpDir();


int connect_fd;

int main(void)
{	
	createFtpDir();
	int listen_fd;
	SA_in sever_addr,client_addr;	
	//XXX:	
	memset(&client_addr,0,sizeof(client_addr));
	sever_addr.sin_family=AF_INET;
	sever_addr.sin_addr.s_addr=inet_addr(SERVER_IP);
	sever_addr.sin_port=htons(SERVER_PORT);
	//XXX
	
	socklen_t len;		
	char buf[BUFSIZE];
	listen_fd=socket(AF_INET,SOCK_STREAM,0);
	if(bind(listen_fd,(SA *)&sever_addr,sizeof(sever_addr))==-1)
	{
	perror("bind fail");
	exit(-1);
	}
	listen(listen_fd,10);		
	while(1)
	{		
		if((connect_fd=accept(listen_fd,(SA *)&client_addr,&len))==-1)
		{
			perror("accept fail");		
		}
		if(fork()==0)
		{
			close(listen_fd);
			memset(buf,'\0',BUFSIZE);
			recv(connect_fd,buf,BUFSIZE,0);				
			switch(buf[0])
			{			
				case 'P':processGet(buf);break;
				case 'G':processPut(buf);break;
				case 'L':processList();break;					
			}
			exit(0);			
		}
		else
		{
			close(connect_fd);		
		}
	}	
	return 0;
}



int processPut(char *str)
{
	char path[PATH_LEN]=FTP_PATH;	
	char filename[PATH_LEN];
	sprintf(filename,"%s",str+1);	
	strcat(path,filename);
	char buf[BUFSIZE];
	int file_fd;
	int count;		
	if((file_fd=open(path,O_RDONLY))==-1)
	{
		perror("文件不存在,或不可用");
		buf[0]='E';
		send(connect_fd,buf,BUFSIZE,0);					
		return -1;
	}	
	buf[0]='S';
	send(connect_fd,buf,BUFSIZE,0);		
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
	printf("文件%s发送成功\n",filename);	
	//发送文件模块		
	return 0;
}

int processGet(char *str)
{
	char path[PATH_LEN]=FTP_PATH;	
	char filename[PATH_LEN];
	sprintf(filename,"%s",str+1);	
	strcat(path,filename);	
	char buf[BUFSIZE];
	int file_fd;
	int count=0;					
	if((file_fd=open(path,O_WRONLY|O_CREAT|O_EXCL,0644))==-1)
	{
		perror("该文件已存在,接收失败,请重试");
		buf[0]='E';
		send(connect_fd,buf,BUFSIZE,0);			
		return -1;
	}	
	buf[0]='S';
	send(connect_fd,buf,BUFSIZE,0);			
	//接收文件模块	
	sleep(1);
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
	printf("%s接收成功\n",filename);
	//接收文件模块	
	return 0;
}

int processList()
{	
	char path[PATH_LEN]=FTP_PATH;	
	char buf[BUFSIZE];	
	int count;	
	DIR* dirp;
	struct dirent* dir;	
	dirp = opendir(path);	
	while(1)
	{
	if((dir = readdir(dirp))==NULL)
	break;
	if(strcmp(dir->d_name,".")==0||strcmp(dir->d_name,"..")==0)
	continue;
	memset(buf,'\0',BUFSIZE);
	sprintf(buf,"%s\n",dir->d_name);
	send(connect_fd,buf,BUFSIZE,0);
	}
	closedir(dirp);		
	return 0;
}

void createFtpDir()
{
	mkdir("./ftp",0755);	
	printf("FTP 服务器ip:%s,端口%d\n",SERVER_IP,SERVER_PORT);
}







