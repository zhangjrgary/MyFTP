#include <stdio.h>
#include <string.h>
int main(void)
{
	char buff[1024];
	memset(buff,0,1024);
	FILE *fp;
	fp=fopen("a.txt","w");
	
	fgets(buff,1024,stdin);
	
	printf("%d:%d:%d:%d\n=",buff[0],buff[1],buff[2],buff[3]);
	
	fputs(buff,stdout);
	
	rewind(fp);
	char c;
	while((c=fgetc(fp))!=EOF)
	{
		printf("%5d",c);
	}

	fclose(fp);
}
