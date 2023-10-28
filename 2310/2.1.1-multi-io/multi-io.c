
#include <sys/socket.h>
#include <errno.h>  //perror
#include <netinet/in.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h> //close()

//�߳�ͷ�ļ�����
#include <pthread.h>



//����һ��linux�߳�
void *client_thread(void *arg)
{
    int clientfd = *(int *)arg;
    
    while (1) {
     char buffer[128] = {0};
     int count = recv(clientfd,buffer,128,0); 
     if(count == 0)
     {
         break;
     }
     /* count : 128 ��ʲô״̬*/
     send(clientfd,buffer,128,0);
     printf(" clientfd: %d,count:%d ,buffer:%s \n",clientfd,count,buffer);
    }
    close(clientfd);
}

//TCP SERVER 
int main(){
  	int sockfd = socket(AF_INET,SOCK_STREAM,0);
  	struct sockaddr_in serveraddr;
  	memset(&serveraddr,0,sizeof(struct sockaddr_in));
  	//�󶨱���
  	serveraddr.sin_family = AF_INET;
  	//���ص�ַ  ANY�����ַ
  	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  	// 1024��ǰ�Ķ˿���ϵͳ���ã�
    serveraddr.sin_port = htons(2048);
    //��  
    //
	if(-1 == bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(struct sockaddr)))
    {
      perror("bind");
      return -1;
    }
    
    listen(sockfd,10);
    #if 0
    struct sockaddr_in clientaddr;//����һ���ͻ���
    socklen_t len = sizeof(clientaddr);
    int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
    
    printf("accept \n ");
    
   #if 0
     //����buffer
    char buffer[128] = {0};
    int count = recv(clientfd,buffer,128,0); 
     /* count : 128 ��ʲô״̬*/
    send(clientfd,buffer,128,0);
     printf("sockfd:%d clientfd: %d,count:%d ,buffer:%s \n",sockfd,clientfd,count,buffer);
   #else
        while (1) {
             char buffer[128] = {0};
             int count = recv(clientfd,buffer,128,0); 
             if(count == 0)
             {
                 break;
             }
             /* count : 128 ��ʲô״̬*/
             send(clientfd,buffer,128,0);
             printf("sockfd:%d clientfd: %d,count:%d ,buffer:%s \n",sockfd,clientfd,count,buffer);
        }
   #endif
   #else
    while (1){
         struct sockaddr_in clientaddr;//����һ���ͻ���
    	 socklen_t len = sizeof(clientaddr);
    	 int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
         
        //����һ���߳�ID
         pthread_t thid;
         pthread_create(&thid,NULL,client_thread,&clientfd);
    }
    
   #endif
    
    getchar(); 
    //close(clientfd); //�ر�ͨѶ
}

