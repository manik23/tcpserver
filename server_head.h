#include <iostream>
#include <pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <mysql/mysql.h>
#include <stdio.h>

using namespace std;

#define SIZE_OF_IPV4_ADDRESS 16
#define MAC_ADDRESS_SIZE 18

struct client{
        int port;
        char ip_address[SIZE_OF_IPV4_ADDRESS];
};

struct client i_client;

int mysql_exec_sql(MYSQL *mysql,const char *create_definition)
{
        return
        mysql_real_query(mysql,create_definition,strlen(create_definition));
}

MYSQL buildConnection(){
        MYSQL mysql;
        if(mysql_init(&mysql)==NULL)
        {
            cout<<"Failed to initiate Mysql connection"<<endl;
                return mysql;
}
        if(!mysql_real_connect(&mysql,"localhost","root","open","p2p",0,NULL,0))
        {
                cout<<"Failed to connect to MySQL: Error: "<<mysql_error(&mysql)<<endl;
        return mysql;
   }
   cout<<"Logged on to database successfully"<<endl;
   if(mysql_select_db(&mysql,"p2p")==0)
        cout<<"Database selected"<<endl;
   else
        cout<<"Failed to connect to Database: Error: "<<mysql_error(&mysql)<<endl;
        return mysql;
}

int check_if_registered(char *mac_address){
        MYSQL_RES *result;

        MYSQL mysql=buildConnection();

        char check_if_register_def[10000];
        memset(check_if_register_def,0,sizeof(check_if_register_def));

        strcat(check_if_register_def,"SELECT COUNT(mac_address) FROM device WHERE mac_address='");
        strcat(check_if_register_def,mac_address);
        strcat(check_if_register_def,"';");

        if(mysql_exec_sql(&mysql,check_if_register_def)==0)
                cout<<"success"<<endl;
        else
                cout<<"Failed to select"<<mysql_error(&mysql)<<endl;

    result = mysql_store_result(&mysql);
    MYSQL_ROW row = mysql_fetch_row(result);

    mysql_close(&mysql);
    int val;
    sscanf(row[0],"%d",&val);
    if(val>0)
        return 1;
    else
        return 0;

}

int find_and_update(int port, char *ip_address, char *mac_address){
        MYSQL_RES *result;

        MYSQL mysql=buildConnection();

        char find_and_update_def[1000];
        memset(find_and_update_def,0,sizeof(find_and_update_def));

        strcat(find_and_update_def,"UPDATE device SET ip_address='");
        strcat(find_and_update_def,ip_address);
        strcat(find_and_update_def,"',port=");
        char port1[7];
        sprintf(port1,"%d",port);
        strcat(find_and_update_def,port1);
        strcat(find_and_update_def," WHERE mac_address='");
        strcat(find_and_update_def,mac_address);
        strcat(find_and_update_def,"';");

        //cout<<find_and_update_def<<endl;

        if(mysql_exec_sql(&mysql,find_and_update_def)==0){
                cout<<"Updated"<<endl;
                mysql_close(&mysql);
                return 1;
        }
        else{
                cout<<"Error in updation"<<mysql_error(&mysql)<<endl;
                mysql_close(&mysql);
                return 0;
        }
}

int create_new_node(int port, char *ip_address, char *mac_address){
 MYSQL_RES *result;

        MYSQL mysql=buildConnection();

        char create_new_def[1000];
        memset(create_new_def,0,sizeof(create_new_def));

        strcat(create_new_def,"INSERT INTO device (mac_address,ip_address,port) VALUES('");
        char port1[7];
        sprintf(port1,"%d",port);
        strcat(create_new_def,mac_address);
        strcat(create_new_def,"','");
        strcat(create_new_def,ip_address);
        strcat(create_new_def,"',");
        strcat(create_new_def,port1);
        strcat(create_new_def,");");

        if(mysql_exec_sql(&mysql,create_new_def)==0){
                cout<<"Insert successfull"<<endl;
                mysql_close(&mysql);
                return 1;
        }
        else{
                cout<<"Failed to create new entry"<<mysql_error(&mysql)<<endl;
                mysql_close(&mysql);
                return 0;
        }
}

void print_all_device(){
        MYSQL_RES *result;
        MYSQL_ROW row;
        unsigned int num_fields;
        MYSQL mysql=buildConnection();

        char print_all_def[1000];
        memset(print_all_def,0,sizeof(print_all_def));

        strcat(print_all_def,"SELECT * FROM device");
        cout<<"All registered devices"<<endl<<endl;
        cout<<"S.NO\tMac_address\t\tip_address"<<"\tport"<<endl;

        if(mysql_exec_sql(&mysql,print_all_def)==0){
        result = mysql_store_result(&mysql);
        if (result)
        {
                while ((row = mysql_fetch_row(result)))
                {
                        cout<<row[0]<<"\t"<<row[1]<<"\t"<<row[2]<<"\t"<<row[3]<<endl;
                }
        mysql_free_result(result);
        }
        else
        {
                if(mysql_field_count(&mysql) > 0)
                {
                        cout<<"Error getting records: "<<mysql_error(&mysql)<<endl;
                        }
                }
                mysql_close(&mysql);
        }
        else{
                        cout<<"Error in finding Entry"<<mysql_error(&mysql)<<endl;
                        mysql_close(&mysql);
                }
}

MYSQL_RES *search(char *mac_address){
        MYSQL mysql=buildConnection();
        MYSQL_RES *result;

        char search_def[1000];
        memset(search_def,0,sizeof(search_def));

        strcat(search_def,"SELECT * FROM device WHERE mac_address='");
        strcat(search_def,mac_address);
        strcat(search_def,"';");

        if(mysql_exec_sql(&mysql,search_def)==0){
                result=mysql_store_result(&mysql);
                return result;
        }
        else
                return NULL;
        }
/*******************************************************************************************************************************/

int check_for_registeration(int udpSocket,char *mac_address){

        // Search for the ip and the port corresponding to the device mac_address
        MYSQL_RES *result = search(mac_address);
        char status[5];
        memset(status,0,sizeof(status));

        if(result==NULL)
                cout<<"Error in finding the result"<<endl;
        else{
                MYSQL_ROW row;
                row=mysql_fetch_row(result);
                cout<<"IP :"<<row[2]<<"   Port : "<<row[3]<<" of R:T on udp "<<endl;
                   //R:T
                   struct sockaddr_in udpStruct1;
                   int intport;
                   sscanf(row[3],"%d",&intport);
                   udpStruct1.sin_family=AF_INET;
                   udpStruct1.sin_port=htons(intport);
                   udpStruct1.sin_addr.s_addr=inet_addr(row[2]);
                   socklen_t len=sizeof(udpStruct1);
                   sendto(udpSocket,"registeration_founded",strlen("registeration_founded"),0,(struct sockaddr*)&udpStruct1,sizeof(udpStruct1));


                   /*cout<<"sending udp req to make tcp connection"<<endl;
                   char port1[6];
                   sprintf(port1,"%d",port);
                   //while(strcmp(status,"ok")){
                       sendto(udpSocket,port1,sizeof(port1),0,(struct sockaddr*)&udpStruct1,sizeof(udpStruct1));
                       sendto(udpSocket,i_ip_address,SIZE_OF_IPV4_ADDRESS,0,(struct sockaddr*)&udpStruct1,sizeof(udpStruct1));
                       memset(status,0,sizeof(status));
                       //recvfrom(udpSocket,status,sizeof(status),0,(struct sockaddr*)&udpStruct1,&len);
                           //close(udpSocket);
                       cout<<"Done"<<endl;
               // }*/

        }
}




/********************************************************************************************************************************/
int listen_for_registeration(int udpSocket){
        int  nBytes;
        char mac_address[MAC_ADDRESS_SIZE],ip_address[SIZE_OF_IPV4_ADDRESS];
        struct sockaddr_in udpStructC;

        while(1){

                socklen_t len=sizeof(udpStructC);

                nBytes=recvfrom(udpSocket,mac_address,MAC_ADDRESS_SIZE,0,(struct sockaddr *)&udpStructC,&len);
                if(strcmp(mac_address,"ok")==0)
                    continue;
                int port=ntohs(udpStructC.sin_port);
                inet_ntop(AF_INET,&udpStructC.sin_addr,ip_address,SIZE_OF_IPV4_ADDRESS);

                int l,status;
                l=check_if_registered(mac_address);
                if(l==1)
                        status=find_and_update(port,ip_address,mac_address);
                else
                        status=create_new_node(port,ip_address,mac_address);


                // Now Send ACK to the R client that he is successfully registered with ACK ok
                if(status==1)
                        sendto(udpSocket,"ok",2,0,(struct sockaddr *)&udpStructC,sizeof(udpStructC));
                else
                        sendto(udpSocket,"error",5,0,(struct sockaddr *)&udpStructC,sizeof(udpStructC));
                print_all_device();
        }
}