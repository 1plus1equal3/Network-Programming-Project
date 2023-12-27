#include "../data.h"
#include "admin.c"
#include "user.c"

void showLoginMenu(int client_sockfd);
void login(int client_sockfd);
void signup(int client_sockfd);
void showMainAppMenu(int client_sockfd);
void showMainAppMenuAdmin(int client_sockfd);

login_data data;
int id;
//Client functions
void login(int client_sockfd){
    int check = 0;
    //login_data data;

    memset(&data, 0, sizeof(login_data));
    printf("Login to the system!\nEnter the username: ");
    // scanf(" %s", data.username);
    // printf("Enter the password: ");
    // scanf("%s", data.password);
    __fpurge(stdin);
    fgets(data.username, sizeof(data.username), stdin);
    data.username[strlen(data.username)-1] = '\0';
    __fpurge(stdin);
    printf("Enter the password: ");
    //scanf(" [^\n]%s", data.password);
    fgets(data.password, sizeof(data.password), stdin);
    __fpurge(stdin);

    data.password[strlen(data.password)-1] = '\0';
    data.opcode = 100; //Code for login
    send(client_sockfd, &data, sizeof(data), 0);//Send username and password
    login_server_response response;
    recv(client_sockfd, &response, sizeof(response), 0);//Receive check login flag from server
    if(response.valid == 1 && response.previlege == 0){
        printf("Login successfully!\n");
        id = response.user_id;
        data.user_id = id;
        showMainAppMenu(client_sockfd);
    }else if(response.valid == 1 && response.previlege == 1){
        printf("Login successfully!\n");
        id = response.user_id;
        data.user_id = id;
        showMainAppMenuAdmin(client_sockfd);
    }
    else{
        printf("Login failed! Please retry\n");
        showLoginMenu(client_sockfd);
    }
}

void signup(int client_sockfd){
    int check = 0;
    register_data data2;
    printf("Sign up to the system!\nEnter the username: ");
    scanf(" %s", data2.username);
    printf("Enter the password: ");
    scanf(" %s", data2.password);
    printf("Enter the email: ");
    scanf(" %s", data2.email);
    // printf("Enter the id: ");
    // scanf(" %d", &data.id);
    data2.opcode = 101; //Code for signup
    send(client_sockfd, &data2, sizeof(register_data), 0);//Send username and password
    recv(client_sockfd, &check, sizeof(check), 0);//Receive check login flag from server
    if(check == 1){
        printf("Sign up successfully!\n");
        showLoginMenu(client_sockfd);
    }
    else{
        printf("Sign up failed! Please retry\n");
        showLoginMenu(client_sockfd);
    }
}

//UI functions
void showLoginMenu(int client_sockfd){
    printf("Welcome to the system!\n");
    printf("1. Login\n");
    printf("2. Sign up\n");
    printf("3. Exit\n");
    printf("Please choose your option: ");
    int option;
    scanf(" %1d", &option);
    __fpurge(stdin);
    switch(option){
        case 1:
            login(client_sockfd);
            return;
        case 2:
            signup(client_sockfd);
            return;
        case 3:
            printf("Exit successfully!\n");
            return;
        default:
            printf("Invalid option!\n");
            break;
        }
    showLoginMenu(client_sockfd);
}

//Main function
int main(int argc, char const *argv[]){
    int client_sockfd;
    struct sockaddr_in serv_addr;
    char buff[BUFF_SIZE];
    //Construct socket
    client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    //Specify server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    //Request to connect server
    if(connect(client_sockfd, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr)) < 0){
        printf("Cannot connecting to server...Please retry!");
        return 0;
    }

    //Start application
    showLoginMenu(client_sockfd);
    return 0;
}