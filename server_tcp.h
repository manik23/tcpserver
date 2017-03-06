int tcp_handler(int udpsock,struct sockaddr_in udpstruct){
    pid_t pid;
    int fd[2];
    int fd1[2];

    pipe(fd);
    pipe(fd1);
    pid=fork();


    if(pid==0){
    char ip[16],inport[6];
    char R[16],J[6];
    int intJ=5005;
    int bz1,port1=5002,connfd1;
	char buffer[1024];
    cout<<"ok1"<<endl;
	//B:Z1 at B ip of server and Z=5002
	struct sockaddr_in addr2,caddr2; //addr2 will help him to listen on 5002 
	bz1=socket(AF_INET,SOCK_STREAM,0);
	addr2.sin_family=AF_INET;
	addr2.sin_port=htons(port1);
	addr2.sin_addr.s_addr=htonl(INADDR_ANY);
	//cout<<"ok2"<<endl;
	if(bind(bz1,(struct sockaddr *)&addr2,sizeof(addr2))<0)
	{
		cout<<"tcp handler not binded on 5002 for listening NR:K"<<endl;
		perror("error");
	}
	else
	{
		cout<<"binded on 5002"<<endl;
	}
	//cout<<"ok3"<<endl;
	if(listen(bz1,10)<0)
	{
		perror("error");
	}

	while(1)
	{
		socklen_t len1=sizeof(caddr2);
		connfd1=accept(bz1,(struct sockaddr *)&caddr2,&len1);
		inet_ntop(AF_INET,&caddr2.sin_addr,R,16);
		sprintf(J,"%d",intJ);
		//cout<<"ok4"<<endl;
		close(fd[1]);
		read(fd[0],ip,sizeof(ip));
		close(fd[1]);
		read(fd[0],inport,sizeof(inport));
		//sendig the R:J To NI:Y
		close(fd1[0]);
		write(fd1[1],R,sizeof(R));
		close(fd1[0]);
		write(fd1[1],J,sizeof(J));
		cout<<"NI:y\t"<<ip<<"\t"<<inport<<endl;  // these are ip of Clinet I 

		if(connfd1<0){
			perror("error");
		}
		//reading ack
		read(connfd1,buffer,sizeof(buffer));
		cout<<buffer<<endl;
		//cout<<"closing socket on 5002"<<endl;
		cout<<"sending the NI:Y to the R:J"<<endl;
		write(connfd1,ip,sizeof(ip));
		write(connfd1,inport,sizeof(inport));
		close(connfd1);
	}

	
}
   else
   {
   	int bz,port=5001,connfd;
   	int poort;
   	char ip[16],inport[6];
   	char R[16],J[6];
	//B:Z at B ip of server and Z=5001 
	char mac[17];
	struct sockaddr_in addr,caddr; //addr2 will help him to listen on 5002 
	bz=socket(AF_INET,SOCK_STREAM,0);
	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);
	addr.sin_addr.s_addr=htonl(INADDR_ANY);

	if(bind(bz,(struct sockaddr *)&addr,sizeof(addr))<0)
	{
		cout<<"tcp handler not binded on 5001 for listening NI:Y"<<endl;
		perror("error");
	}
	else
	{
		cout<<"binded on 5001"<<endl;
	}
	listen(bz,10);
	while(1)
	{
		socklen_t len=sizeof(caddr);
		connfd=accept(bz,(struct sockaddr *)&caddr,&len);
		inet_ntop(AF_INET,&caddr.sin_addr,ip,16);
        poort=ntohs(caddr.sin_port);
        sprintf(inport,"%d",poort);
        close(fd[0]);
	    write(fd[1],ip,sizeof(ip));
	    close(fd[0]);
	    write(fd[1],inport,sizeof(inport));
	    
	    /******************/
	    read(connfd,mac,17);
	    cout<<mac<<endl;
		check_for_registeration(udpsock,mac);
		//cout<<"closing socket on 5001"<<endl;
		close(fd1[1]);
	    read(fd1[0],R,sizeof(R));
	    close(fd1[1]);
	    read(fd1[0],J,sizeof(J));
		//cout<<R<<"\t"<<J<<"\t R:J"<<endl;
		//sending th R:J to the NY:I
		write(connfd,R,sizeof(R));
		write(connfd,J,sizeof(J));

		close(connfd);

	}
   	/***************/
	
}

}