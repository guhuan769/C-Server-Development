#include <sys/socket.h>
#include <errno.h>  //perror
#include <netinet/in.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h> //close()

//�߳�ͷ�ļ�����
#include <pthread.h>
#include <sys/poll.h>
#include <sys/epoll.h>


//����һ��linux�߳�
//block
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

	//����һ���̶���С
	
	int epfd = epoll_create(1);// int size

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = sockfd;

	//����ĸ�sockfd�����ĸ��¼�
	epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&ev);

	struct epoll_event events[1024] = {0};
 	while (1) {
		int nready = epoll_wait(epfd,events,1024,-1);
		int i = 0;
		for(i = 0;i < nready;i ++){
			int connfd = events[i].data.fd;//
			if(sockfd == connfd){
				struct sockaddr_in clientaddr;
				socklen_t len = sizeof(clientaddr);

				int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
				
				ev.events = EPOLLIN | EPOLLET;//���ش�����EPOLLIN | EPOLLET
				ev.data.fd = clientfd;
				epoll_ctl(epfd,EPOLL_CTL_ADD,clientfd,&ev);
				printf("clientfd:%d",clientfd);
			}else if (events[i].events & EPOLLIN){

				char buffer[128] = {0};
				int count = recv(connfd,buffer,128,0);
				printf("clientfd:%d,count:%d,buffer:%s",connfd,count,buffer);
				if(count == 0){
					printf(" disconnect \n");
						
					epoll_ctl(epfd,EPOLL_CTL_DEL,connfd,NULL);
					close(i);

					continue;
				}
				send(connfd,buffer,count,0);
				printf("clientfd:%d,count:%d,buffer:%s",connfd,count,buffer);
			}
		}
	}
	
	
    getchar(); 
    //close(clientfd); //�ر�ͨѶ
}
