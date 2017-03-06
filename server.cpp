#include "server_head.h"
#include "server_tcp.h"




int main()
{
   int udpsock;
   struct sockaddr_in udpstruct;
   udpsock=socket(AF_INET,SOCK_DGRAM,0);

        udpstruct.sin_family=AF_INET;
        udpstruct.sin_port=htons(5000);
        udpstruct.sin_addr.s_addr=htonl(INADDR_ANY);

        if(bind(udpsock,(struct sockaddr *)&udpstruct,sizeof(udpstruct))<0)
        {
                cout<<"Bind unsuccessfull"<<endl;
                exit(1);
        }
        else
        {
                cout<<"Binding Completed"<<endl;
        }


pid_t pid;
pid=fork();

if(pid==0){
	tcp_handler(udpsock,udpstruct);
}
else{
    
    listen_for_registeration(udpsock);
}
	

}