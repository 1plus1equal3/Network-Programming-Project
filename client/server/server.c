#include <mysql/mysql.h>
#include "../data.h"
#include "admin_servant.c"
#include "user_servant.c"

#define MAX_QUERY_LEN 3000
MYSQL *conn;

//srand(time(NULL));

void extract_error(MYSQL *conn);
void connectToDB();
MYSQL_RES *make_query(char query[]);
int query_database_for_login(char *username, char *password, int *previlege, int *id);
void check_login(int socket, char *buffer);
int query_database_for_signup(char *username);
void addNewUser(char username[], char user_password[], char email[]);
void check_signup(int socket, char *buffer);
void *connection_handler(void *);

int main(){
    //Connect to database
    connectToDB();

    //Create socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == 1){
        perror("Fail in initializing socket\n");
        exit(EXIT_FAILURE);
    }
    else{
        printf("Server socket created successfully\n");
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //Bind the socket to the IP address and port
    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0){
        printf("Binding fail !\n");
        exit(0);
    }
    else{
        printf("Socket successfully binded...\n");
    }
    if(listen(server_socket, 100) != 0){
        printf("Listen failed...\n");
        exit(0);
    }
    else{
        printf("Server is listening...\n");
    }

    // pid_t pid;
    // int sin_size;
    // int conn_sock;
    // while(1){
	// 	sin_size=sizeof(struct sockaddr_in);
	// 	if ((conn_sock = accept(server_socket, (struct sockaddr *)&client, &sin_size))==-1){
	// 		if (errno == EINTR)
	// 			continue;
	// 		else{
	// 			perror("\nError: ");			
	// 			return 0;
	// 		}
	// 	}
		
	// 	/* For each client, fork spawns a child, and the child handles the new client */
	// 	pid = fork();
		
	// 	/* fork() is called in child process */
	// 	if(pid == 0){
	// 		close(server_socket);
	// 		printf("You got a new connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */
	// 		connection_handler(conn_sock);					
	// 		exit(0);
	// 	}
		
	// 	/* The parent closes the connected socket since the child handles the new client */
	// 	close(conn_sock);
	// }

    //Create threads
    int no_threads = 0;
    pthread_t threads[100];
    while(no_threads < 100){
        printf("Listening...\n");
        int client_socket = accept(server_socket, NULL, NULL);
        printf("Connection accepted\n");
        if(pthread_create(&threads[no_threads], NULL, connection_handler, &client_socket) <0 ){
            perror("Could not create thread\n");
            return 1;
        }
        if(client_socket < 0){
            printf("Server accept failed...\n");
            exit(0);
        }
        else printf("Server accept the client...\n");
        printf("Handler assigned\n");
        no_threads++;
    }
    int k = 0;
    for(k = 0; k < 100; k++){
        pthread_join(threads[k], NULL);
    }

    close(server_socket);
    mysql_close(conn);
    return 0;
}

void extract_error(MYSQL *conn) {
    fprintf(stderr, "%s\n", mysql_error(conn));
}

void connectToDB(){
    char *server = "localhost";
    char *user = "root";
    char *password = "123456"; 
    char *database = "network_db_02";

    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
        extract_error(conn);
        exit(1);
    }
    printf("Connect to database successfully\n");
}

MYSQL_RES *make_query(char query[]){
    MYSQL_RES *res;
    if (mysql_query(conn, query)) {
        extract_error(conn);
        return NULL;
    }

    res = mysql_use_result(conn);
    //mysql_close(conn);
    return res;
}

int query_database_for_login(char *username, char *password, int *previlege, int *id){ 
    //return 1 if login success and 0 if login fail
    //SELECT * FROM User_info WHERE username = '' AND password = '';
    char query[MAX_QUERY_LEN];
    strcpy(query, "SELECT * FROM User_info WHERE username = \'");
    strcat(query, username);
    strcat(query, "\' AND password = \'");
    strcat(query, password);
    strcat(query, "\';");
    // printf("\nquery = \'%s\'\n\n", query);
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row = mysql_fetch_row(res);
    if(row != NULL){
        *id = atoi(row[0]);
        *previlege = atoi(row[4]);
        mysql_free_result(res);
        return 1;
    }
    mysql_free_result(res);
    return 0;
}


void check_login(int socket, char *buffer){
    //Convert buffer to login_msg
    login_data *login = (login_data*)buffer;
    char *username = login->username;
    char *password = login->password;
    int previlege, id;
    int valid = query_database_for_login(username, password, &previlege, &id);

    if(valid == 1){
        printf("Login success\n");
    }
    else{
        printf("Login fail\n");
    }
    //Send login success or fail signal to client
    login_server_response response;
    response.opcode = 102;
    response.valid = valid;
    response.previlege = previlege;
    response.user_id = id;
    //Check response
    printf("Response:\n");
    printf("Opcode: %d\n", response.opcode);
    printf("Valid: %d\n", response.valid);
    printf("Previlege: %d\n", response.previlege);
    printf("Id: %d\n", response.user_id);
    send(socket, &response, sizeof(response), 0);
}

int query_database_for_signup(char *username){ 
    //return 1 if login success and 0 if login fail
    //SELECT * FROM User_info WHERE username = '' AND password = '';
    char query[MAX_QUERY_LEN];
    strcpy(query, "SELECT * FROM User_info WHERE username = \'");
    strcat(query, username);
    strcat(query, "\';");
    // printf("\nquery = \'%s\'\n\n", query);
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row = mysql_fetch_row(res);
    mysql_free_result(res);
    int result = (row == NULL);
    return result;
}

void addNewUser(char username[], char user_password[], char email[]){
    MYSQL *conn;

    char *server = "localhost";
    char *user = "root";
    char *password = "123456"; 
    char *database = "network_db_01";

    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
        extract_error(conn);
        exit(1);
    }

    char query[MAX_QUERY_LEN];
    strcpy(query, "INSERT INTO");
    strcat(query, username);
    strcat(query, "\';");
    if(strcmp(email, "") == 0){
        strcpy(query, "INSERT INTO User_info(username, password, role) VALUES ( \'");
        strcat(query, username);
        strcat(query, "\', \'");
        strcat(query, user_password);
        strcat(query, "\', ");
        strcat(query, "0");
        strcat(query, " );");
    }
    else{
        strcpy(query, "INSERT INTO User_info(username, password, email, role) VALUES ( \'");
        strcat(query, username);
        strcat(query, "\', \'");
        strcat(query, user_password);
        strcat(query, "\', \'");
        strcat(query, email);
        strcat(query, "\', ");
        strcat(query, "0");
        strcat(query, " );");
    }

    //query
    if (mysql_query(conn, query)) {
        extract_error(conn);
        exit(1);
    }
}

void check_signup(int socket, char *buffer){
    //Convert buffer to register_msg
    register_data *register_msg = (register_data*)buffer;

    //Testcase
    char *username = register_msg->username;
    char *password = register_msg->password;
    char *email = register_msg->email;
    printf("Username: %s\n", username);
    printf("Password: %s\n", password);
    printf("Email: %s\n", email);
    int valid = query_database_for_signup(username);
    if(valid == 1){
        addNewUser(username, password, email);
        printf("Add new user successfully\n");
    }
    else{
        printf("Sign up failed!\n");
    }
    
    //Send add new user success or fail signal to client
    send(socket, &valid, 4, 0);
}


void *connection_handler(void *client_socket){
    int socket = *(int*)client_socket;
    char client_message[MAX_QUERY_LEN];
    int len;
    while((len = recv(socket, client_message, sizeof(client_message), 0)) > 0){
    client_message[len] = '\0';
    char opcode[4];
    strncpy(opcode, client_message, 4);
    //printf("%c\n",opcode[0]);
    int code = *(int*)opcode;
    printf("\nOpcode: %d\n", code);
    switch (code){
    //########## User request ##########
    case 100:
        check_login(socket, client_message);
        break;
    case 101:
        check_signup(socket, client_message);
        break;
    case 103:
        requestUserInfo(socket, client_message);
        break;
    case 203:
        requestExamList(socket, client_message);
        break;
    case 204:
        requestQuestionList_PublicExam(socket, client_message);
        break;
    case 205:
        evaluateExam(socket, client_message);
        break;
    case 207:
        getUserHistory(socket, client_message);
        break;
    case 301:
        requestAdmin(socket, client_message);
        break;
    //########## Admin request ##########
    case 302:
        getAdminRequestInfo(socket, client_message);
        break;
    case 304:
        approveAdminRequest(socket, client_message);
        break;
    case 601:
        searchQuestionById(socket, client_message);
        break;
    case 602:
        searchQuestionByContent(socket, client_message);
        break;
    case 603:
        advanceQuestionSearch(socket, client_message);
        break;
    case 604:
        addNewQuestion(socket, client_message);
        break;
    case 605:
        deleteQuestion(socket, client_message);
        break;
    case 701:
        searchExamById(socket, client_message);
        break;
    case 702:
        createRandomExam(socket, client_message);
        break;
    case 703:
        createExamByQuestion(socket, client_message);
        break;
    case 704:
        deleteExam(socket, client_message);
        break;
    //########## Room request ##########
    case 801:
        createRoom(socket, client_message);
        break;
    case 802:
        deleteRoom(socket, client_message);
        break;
    case 803:
        showRoomYouCreated(socket, client_message);
        break;
    case 804:
        showRoomYouWereAdded(socket, client_message);
        break;
    case 805:
        showAllYourRoom(socket, client_message);
        break;
    case 811:
        addUserToRoom(socket, client_message);
        break;
    case 812:
        deleteUserFromRoom(socket, client_message);
        break;
    case 813:
        addTestFromBank(socket, client_message);
        break;
    case 814:
        createTest(socket, client_message);
        break;
    case 815:
        deleteTestFromRoom(socket, client_message);
        break;
    case 816:
        Admin_showSubmittedRecordOfTest(socket, client_message);
        break;
    case 817:
        Admin_showAllSubmittedRecord(socket, client_message);
        break;
    case 831:
        User_showSubmittedRecordOfTest(socket, client_message);
        break;
    case 832:
        User_showAllSubmittedRecord(socket, client_message);
        break;
    case 833:
        user_do_test(socket, client_message);
        break;
    //########## Advanced request ##########
    case 1001:
        getImageQuestion(socket, client_message);
        break;
    case 1002:
        getSoundQuestion(socket, client_message);
        break;
    case 1003:
        getUserStatistic(socket, client_message);
        break;
    default:
        break;
    }
    memset(client_message, 0, sizeof(client_message));
    }
    close(socket);
}