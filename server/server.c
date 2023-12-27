#include <mysql/mysql.h>
#include "../data.h"

#define MAX_QUERY_LEN 3000
MYSQL *conn;

//srand(time(NULL));
int key_arr[1000];
char start_time_exam[20];
char end_time_exam[20];

void extract_error(MYSQL *conn);
void connectToDB();
MYSQL_RES *make_query(char query[]);
int query_database_for_login(char *username, char *password, int *previlege, int *id);
void check_login(int socket, char *buffer);
int query_database_for_signup(char *username);
void addNewUser(char username[], char user_password[], char email[]);
void check_signup(int socket, char *buffer);
exam_data** getExamList(int num_exam);
void requestExamList(int socket, char *buffer);
void requestQuestionList(int socket, char *buffer);
void saveHistory(char username[], int test_id, int number_of_right_ans, int number_of_questions);
void evaluateExam(int socket, char *buffer);
void getUserHistory(int socket, char *buffer);
void requestUserInfo(int socket, char *buffer);
void requestAdmin(int socket, char *buffer);
void searchQuestionById(int socket, char *buffer);
void searchQuestionByContent(int socket, char *buffer);
void advanceQuestionSearch(int socket, char *buffer);
void addNewQuestion(int socket, char *buffer);


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
    char *database = "network_db_01";

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

//! ####### USER FUNCTIONS #######

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

//Get exam list for client
exam_data** getExamList(int num_exam){
    exam_data** exam_list = malloc(sizeof(exam_data*));
    *exam_list = malloc(sizeof(exam_data)*num_exam);
    char query[MAX_QUERY_LEN];
    char select[] = "SELECT * FROM Test order by rand() limit";
    sprintf(query, "%s %d;", select, num_exam);
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row;
    int i = 0;
    while((row = mysql_fetch_row(res))){
        (*exam_list)[i].opcode = 201;
        (*exam_list)[i].exam_id = atoi(row[0]);
        strcpy((*exam_list)[i].title, row[1]);
        (*exam_list)[i].number_of_question = atoi(row[2]);
        i++;
    }
    //Free memory
    mysql_free_result(res);
    return exam_list;
}

void requestExamList(int socket, char *buffer){
    //Convert buffer to request_exam_list
    request_exam_list *request = (request_exam_list*)buffer;
    int user_id = request->user_id;
    printf("  ###User: %d request exam list\n", user_id);
    exam_data ** exam_list = getExamList(request->number_of_exam);
    //check exam_list
    printf("Exam list:\n");
    for(int i = 0; i<request->number_of_exam; i++){
        printf("Exam %d: %s\n", i, (*(exam_list)+i)->title);
    }
    send(socket, &request->number_of_exam, sizeof(request->number_of_exam), 0);
    char rcv[10];
    for(int i = 0; i<request->number_of_exam; i++){
        send(socket, (*(exam_list)+i), sizeof(exam_data), 0);
        recv(socket, rcv, sizeof(rcv), 0);
        if(strcmp(rcv, "OK") == 0) continue;
    }
    printf("Send exam list successfully\n");
}

void Shuffle(int arr[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        // Swap arr[i] and arr[j]
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

//Phần này việt đang sửa, nếu có bug thì sorry 2 ô trước =))))
char *getDate(char cur[20]){
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  sprintf(cur, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  return cur;
}

int compareDateTime(const char* date_str1, const char* date_str2) {
    struct tm time1, time2;

    // Parse date_str1
    sscanf(date_str1, "%d-%d-%d %d:%d:%d", &time1.tm_year, &time1.tm_mon, &time1.tm_mday, &time1.tm_hour, &time1.tm_min, &time1.tm_sec);
    time1.tm_year -= 1900; // Adjust for year starting from 1900
    time1.tm_mon -= 1;     // Adjust for month starting from 0

    // Parse date_str2
    sscanf(date_str2, "%d-%d-%d %d:%d:%d",&time2.tm_year, &time2.tm_mon, &time2.tm_mday, &time2.tm_hour, &time2.tm_min, &time2.tm_sec);
    time2.tm_year -= 1900;
    time2.tm_mon -= 1;

    // Convert struct tm to time_t
    time_t time_seconds1 = mktime(&time1);
    time_t time_seconds2 = mktime(&time2);

    // Compare time_t values
    if (difftime(time_seconds1, time_seconds2) < 0) {
        return -1; // date_str1 is earlier than date_str2
    } else if (difftime(time_seconds1, time_seconds2) == 0) {
        return 0; // date_str1 is equal to date_str2
    } else {
        return 1; // date_str1 is later than date_str2
    }
}

void requestQuestionList_PublicExam(int socket, char *buffer){
    request_question_list *request = (request_question_list*)buffer;
    int user_id = request->user_id;
    printf("  ###User: %d request question list\n", user_id);
    int test_id = request->exam_id;
    int num_question = request->number_of_question;
    //!TODO: get question list from database
     
     // Comment lại bvi chưa xong á =)))
    char query[MAX_QUERY_LEN];
    char select[] = "SELECT * FROM Test_question where test_id =";
    sprintf(query, "%s %d;", select, test_id);
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row;
    int i = 0;
    char question_ids[num_question][255];
    while((row = mysql_fetch_row(res))){
        strcpy(question_ids[i], row[0]);
        i++;
    }  
    //Free memory
    mysql_free_result(res);
    question_data **question_list;
    question_list = malloc(sizeof(question_data*));
    *question_list = malloc(sizeof(question_data)*num_question);
    
    // int random_ques[num_question];
    // for(int i = 0; i < num_question; i++){
    //     random_ques[i] = i + 1;
    // }
    
    // Shuffle(random_ques, num_question);

    for(int j = 0; j < num_question; j++){
        char query[MAX_QUERY_LEN];
        char select[] = "SELECT * FROM Question where question_id =";
        sprintf(query, "%s '%s';", select, question_ids[j]);
        MYSQL_RES *res = make_query(query);
        MYSQL_ROW row;
        while((row = mysql_fetch_row(res))){
            (*question_list)[j].opcode = 202;
            strcpy((*question_list)[j].question_id, row[0]);
            strcpy((*question_list)[j].content, row[1]);
            strcpy((*question_list)[j].op[0], row[2]);
            strcpy((*question_list)[j].op[1], row[3]);
            strcpy((*question_list)[j].op[2], row[4]);
            strcpy((*question_list)[j].op[3], row[5]);
        }
        //Free memory
        mysql_free_result(res);
    }
    //check question_list
    for(int j = 0; j < num_question; j++){
        printf("Question %d: %s\n", j+1, (*question_list)[j].question_id);
        printf("Content: %s\n", (*question_list)[j].content);
        printf("A: %s\n", (*question_list)[j].op[0]);
        printf("B: %s\n", (*question_list)[j].op[1]);
        printf("C: %s\n", (*question_list)[j].op[2]);
        printf("D: %s\n", (*question_list)[j].op[3]);
    }
    //!TODO: send question list to client
    send(socket, ((*question_list)), sizeof(question_data), 0);
    char rcv[10];
    for(int j = 1; j < num_question; j++){
        recv(socket, rcv, sizeof(rcv), 0);
        if(strcmp(rcv, "OK") == 0)
            send(socket, ((*question_list)+j), sizeof(question_data), 0);
    }
    // for(int i = 0; i < num_question; i++){
    //     key_arr[i] = random_ques[i];
    // } 
}  
//Khang ơi check ib

void saveHistory(char username[], int test_id, int number_of_right_ans, int number_of_questions){
    char query[MAX_QUERY_LEN];
    char score[10];
    sprintf(score, "%d/%d", number_of_right_ans, number_of_questions);
    sprintf(query, "INSERT INTO History(username, test_id, score, test_date) VALUES ('%s', '%d', '%s', CURDATE())", username, test_id, score);
    MYSQL_RES *res = make_query(query);
    //Free memory
    mysql_free_result(res);
}

int getDifferentTime(const char* date_str1, const char* date_str2) {
    struct tm time1, time2;

    // Parse date_str1
    sscanf(date_str1, "%d-%d-%d %d:%d:%d", &time1.tm_year, &time1.tm_mon, &time1.tm_mday, &time1.tm_hour, &time1.tm_min, &time1.tm_sec);
    time1.tm_year -= 1900; // Adjust for year starting from 1900
    time1.tm_mon -= 1;     // Adjust for month starting from 0

    // Parse date_str2
    sscanf(date_str2, "%d-%d-%d %d:%d:%d",&time2.tm_year, &time2.tm_mon, &time2.tm_mday, &time2.tm_hour, &time2.tm_min, &time2.tm_sec);
    time2.tm_year -= 1900;
    time2.tm_mon -= 1;

    // Convert struct tm to time_t
    time_t time_seconds1 = mktime(&time1);
    time_t time_seconds2 = mktime(&time2);

    time_t diffInSeconds = difftime(time_seconds2, time_seconds1);

    // Convert the time difference to minutes
    int diffInMinutes = (int)(diffInSeconds / 60);

    return diffInMinutes;
}

void evaluateExam(int socket, char *buffer){
    //Convert buffer to answer
    submit_ans *ans = (submit_ans*)buffer;
    int user_id = ans->user_id;
    printf("  ###User: %d evaluate exam result\n", user_id);
    int test_id = ans->exam_id;
    int num_question = ans->number_of_question;
    char user_ans[num_question+1];
    strcpy(user_ans, ans->submit_ans);
    //print user_ans
    printf("User answer:\n");
    for(int j = 0; j < num_question; j++){
        printf("Question %d: %c\n", j+1, user_ans[j]);
    }
    //Query database to get question from test_id
    char query[MAX_QUERY_LEN];
    char select[] = "SELECT * FROM Test_question where test_id =";
    sprintf(query, "%s %d;", select, test_id);
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row;
    int i = 0;
    char question_ids[num_question][255];
    while((row = mysql_fetch_row(res))){
        strcpy(question_ids[i], row[0]);
        i++;
    }  
    //Free memory
    mysql_free_result(res);
    //Query database to get correct answer
    char correct_ans[num_question+1];
    for(int j = 0; j < num_question; j++){
        char query[MAX_QUERY_LEN];
        char select[] = "SELECT * FROM Answer where question_id =";
        sprintf(query, "%s '%s';", select, question_ids[j]);
        MYSQL_RES *res = make_query(query);
        MYSQL_ROW row;
        while((row = mysql_fetch_row(res))){
            correct_ans[j] = atoi(row[1]) + 'A' - 1;
        }
        //Free memory
        mysql_free_result(res);
    }
    printf("Correct answer:\n");
    //Check correct_ans
    for(int j = 0; j < num_question; j++){
        printf("Question %d: %c\n", j+1, correct_ans[j]);
    }
    //Compare user answer and correct answer
    int point = 0;
    for(int j = 0; j < num_question; j++){
        if(user_ans[j] == correct_ans[j]) point++;
    }
    printf("Point: %d\n", point);
    //Send point to client
    send(socket, &point, sizeof(point), 0);
    //Saved user result to database
    saveHistory(ans->username, test_id, point, num_question);
}

void getUserHistory(int socket, char *buffer){
    //Convert buffer to request_exam_result
    request_user_history *request = (request_user_history*)buffer;
    int user_id = request->user_id;
    printf("  ###User: %d request history result\n", user_id);
    char query[MAX_QUERY_LEN];
    MYSQL_RES *res;
    MYSQL_ROW row;

    sprintf(query, "SELECT count(*) from History Where username = '%s';", request->username);
    res = make_query(query);
    row = mysql_fetch_row(res);
    int number_of_history_records = atoi(row[0]);
    memset(query, 0, sizeof(query));
    sprintf(query, "SELECT h.test_date, h.username, t.title, h.score FROM History as h, Test as t WHERE h.test_id = t.test_id AND h.username = '%s';", request->username);
    mysql_free_result(res);
    res = make_query(query);
    exam_result **history_list = malloc(sizeof(exam_result*));
    *history_list = (exam_result *)malloc(sizeof(exam_result)*number_of_history_records);
    int i=0;
    while( (row = mysql_fetch_row(res)) != NULL){
        (*history_list)[i].opcode = 207;
        strcpy((*history_list)[i].date, row[0]);
        strcpy((*history_list)[i].username, row[1]);
        strcpy((*history_list)[i].title, row[2]);
        strcpy((*history_list)[i].score, row[3]);
        i++;
    }
    
    //Print history_list
    for(i = 0; i<number_of_history_records; i++){
        printf("History %d:\n", i+1);
        printf("Date: %s\n", (*history_list)[i].date);
        printf("Username: %s\n", (*history_list)[i].username);
        printf("Title: %s\n", (*history_list)[i].title);
        printf("Score: %s\n", (*history_list)[i].score);
    }
    char oke_signal[OKE_SIGNAL_LEN];
    send(socket, &number_of_history_records, sizeof(int), 0);
    recv(socket, oke_signal, OKE_SIGNAL_LEN, 0);
    if(strcmp(oke_signal, "OK") != 0){
        //RAISE ERROR
    }

    for(i = 0; i<number_of_history_records; i++){
        send(socket, (*history_list)+i, sizeof(exam_result), 0);
        recv(socket, oke_signal, OKE_SIGNAL_LEN, 0);
        if(strcmp(oke_signal, "OK") != 0){
            // RAISE ERROR
        }
    }
    //send(socket, oke_signal, sizeof(oke_signal)+1, 0);
}

void requestUserInfo(int socket, char *buffer){
    //Convert buffer to request_user_info
    request_user_info *request = (request_user_info*)buffer;
    int user_id = request->user_id;
    printf("  ###User: %d request user info\n", user_id);
    char *username = request->username;
    //Query database to get user info
    char query[MAX_QUERY_LEN];
    char select[] = "SELECT * FROM User_info where username =";
    sprintf(query, "%s '%s';", select, username);
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))){
        register_data *user = malloc(sizeof(register_data));
        user->opcode = 102;
        strcpy(user->username, row[1]);
        strcpy(user->password, "********");
        strcpy(user->email, row[3]);
        //Send user info to client
        send(socket, user, sizeof(register_data), 0);
    }
    //Free memory
    mysql_free_result(res);
}

void requestAdmin(int socket, char *buffer){
    //opcode: 301
    //Convert buffer to request_user_info
    request_user_info *request = (request_user_info*)buffer;
    int user_id = request->user_id;
    printf("  ###User: %d request admin previlegde\n", user_id);
    char *username = request->username;
    //!TODO: Query database to get user info
    char query[MAX_QUERY_LEN];
    sprintf(query, "SELECT * FROM Request_admin WHERE username = '%s';", request->username);
    MYSQL_RES *res;
    MYSQL_ROW row;
    res = make_query(query);
    row = mysql_fetch_row(res);
    if(row == NULL){
        sprintf(query, "INSERT INTO Request_admin(username, request_date) VALUES ('%s', CURDATE());", request->username);
        MYSQL_RES *res = make_query(query);
        char oke_signal[] = "OK";
        send(socket, oke_signal, sizeof(oke_signal)+1, 0);
        //Free memory
        mysql_free_result(res);
    }
    else{
        char oke_signal[] = "DUPLICATED";
        send(socket, oke_signal, sizeof(oke_signal)+1, 0);
    }
    //Free memory
    mysql_free_result(res);
}

//! ####### END USER FUNCTIONS #######


/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//@ “Work hard in silence, let your success be the noise.” ―Frank Ocean

$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/


//!####### ADMIN FUNCTIONS #######

//@TODO: Search Operation
void searchQuestionById(int socket, char *buffer){
    //Convert buffer to search_question_by_id
    search_question_by_id *request = (search_question_by_id*)buffer;
    int user_id = request->user_id;
    printf("  ###Admin: %d search question by id\n", user_id);
    char *question_id = request->question_id;
    printf("Question id: %s\n", question_id);
    //Get number of question
    char query[MAX_QUERY_LEN];
    char select[] = "SELECT count(*) FROM Question where question_id like '";
    sprintf(query, "%s%c%s%c';", select, '%', question_id, '%');
    printf("Query: %s\n", query);
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    int number_of_question = atoi(row[0]);
    printf("Number of question: %d\n", number_of_question);
    //Send number of question to client
    if(request->next_page == 0){
        int n = number_of_question;
        send(socket, &n, sizeof(n), 0);
    }
    //No question found
    if(number_of_question == 0){
        mysql_free_result(res);
        return;
    } 
    //Get question from database
    recv(socket, &number_of_question, sizeof(number_of_question), 0); //receive begin index
    memset(query, 0, sizeof(query));
    char select1[] = "SELECT * FROM Question where question_id like '";
    sprintf(query, "%s%c%s%c' limit %d, 10;", select1, '%', question_id, '%', number_of_question);
    printf("Query: %s\n", query);
    mysql_free_result(res);
    res = make_query(query);
    int i = 0;
    question_data **question_list;
    question_list = malloc(sizeof(question_data*));
    *question_list = malloc(sizeof(question_data)*10);
    while((row = mysql_fetch_row(res)) != NULL && i < 10){
        (*question_list)[i].opcode = 202;
        strcpy((*question_list)[i].question_id, row[0]);
        strcpy((*question_list)[i].content, row[1]);
        strcpy((*question_list)[i].op[0], row[2]);
        strcpy((*question_list)[i].op[1], row[3]);
        strcpy((*question_list)[i].op[2], row[4]);
        strcpy((*question_list)[i].op[3], row[5]);
        i++;
    }
    //check question_list
    for(int j = 0; j < i; j++){
        printf("Question %d: %s\n", j+1, (*question_list)[j].question_id);
        printf("Content: %s\n", (*question_list)[j].content);
        printf("A: %s\n", (*question_list)[j].op[0]);
        printf("B: %s\n", (*question_list)[j].op[1]);
        printf("C: %s\n", (*question_list)[j].op[2]);
        printf("D: %s\n", (*question_list)[j].op[3]);
    }
    //Send question list to client
    char rcv[10];
    send(socket, &i, sizeof(i), 0);
    printf("Send number of question: %d\n", i);
    for(int j = 0; j < i; j++){
        recv(socket, rcv, sizeof(rcv), 0);
        if(strcmp(rcv, "OK") == 0){
            send(socket, ((*question_list)+j), sizeof(question_data), 0);
            continue;
        }else{
            //!TODO: send error signal
        }
    }
    //Free memory
    mysql_free_result(res);
}

void searchQuestionByContent(int socket, char *buffer){
    //Convert buffer to search_question_by_content
    search_question_by_content *request = (search_question_by_content*)buffer;
    int user_id = request->user_id;
    printf("  ###Admin: %d search question by question's content\n", user_id);
    char *content = request->question_content;
    //Get number of question
    char query[MAX_QUERY_LEN];
    char select[] = "SELECT count(*) FROM Question where question_content like '";
    sprintf(query, "%s%c%s%c';", select, '%', content, '%');
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    int number_of_question = atoi(row[0]);
    printf("Number of question: %d\n", number_of_question);
    //Send number of question to client
    if(request->next_page == 0){
        int n = number_of_question;
        send(socket, &n, sizeof(n), 0);
    }
    //No question found
    if(number_of_question == 0){
        mysql_free_result(res);
        return;
    } 
    //Get question from database
    recv(socket, &number_of_question, sizeof(number_of_question), 0); //receive begin index
    memset(query, 0, sizeof(query));
    char select1[] = "SELECT * FROM Question where question_content like '";
    sprintf(query, "%s%c%s%c' limit %d, 10;", select1, '%', content, '%', number_of_question);
    printf("Query: %s\n", query);
    mysql_free_result(res);   
    res = make_query(query);
    int i = 0;
    question_data **question_list;
    question_list = malloc(sizeof(question_data*));
    *question_list = malloc(sizeof(question_data)*10);
    while((row = mysql_fetch_row(res)) != NULL && i < 10){
        (*question_list)[i].opcode = 202;
        strcpy((*question_list)[i].question_id, row[0]);
        strcpy((*question_list)[i].content, row[1]);
        strcpy((*question_list)[i].op[0], row[2]);
        strcpy((*question_list)[i].op[1], row[3]);
        strcpy((*question_list)[i].op[2], row[4]);
        strcpy((*question_list)[i].op[3], row[5]);
        i++;
    }
    //check question_list
    for(int j = 0; j < i; j++){
        printf("Question %d: %s\n", j+1, (*question_list)[j].question_id);
        printf("Content: %s\n", (*question_list)[j].content);
        printf("A: %s\n", (*question_list)[j].op[0]);
        printf("B: %s\n", (*question_list)[j].op[1]);
        printf("C: %s\n", (*question_list)[j].op[2]);
        printf("D: %s\n", (*question_list)[j].op[3]);
    }
    //Send question list to client
    char rcv[10];
    send(socket, &i, sizeof(i), 0);
    for(int j = 0; j < i; j++){
        recv(socket, rcv, sizeof(rcv), 0);
        if(strcmp(rcv, "OK") == 0){
            send(socket, ((*question_list)+j), sizeof(question_data), 0);
            continue;
        }
    }
    //Free memory
    mysql_free_result(res);
}

void advanceQuestionSearch(int socket, char *buffer){
    //Convert buffer to advance_question_search
    advance_search_question *request = (advance_search_question*)buffer;
    int user_id = request->user_id;
    printf("  ###Admin: %d advance search question\n", user_id);
    char *question_id = request->question_id;
    char *content = request->question_content;
    char *subject = request->subject;
    char *topic = request->topic;
    //Get number of question
    char query[MAX_QUERY_LEN];
    char select[] = "SELECT count(*) FROM Question where question_id like '";
    sprintf(query, "%s%c%s%c' and question_content like '%c%s%c' and subject like '%c%s%c' and topic like '%c%s%c';", select, '%', question_id, '%', '%', content, '%', '%', subject, '%', '%', topic, '%');
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    int number_of_question = atoi(row[0]);
    printf("Number of question: %d\n", number_of_question);
    //Send number of question to client
    if(request->next_page == 0){
        int n = number_of_question;
        send(socket, &n, sizeof(n), 0);
    }
    //No question found
    if(number_of_question == 0){
        mysql_free_result(res);
        return;
    } 
    //Get question from database
    recv(socket, &number_of_question, sizeof(number_of_question), 0); //receive begin index
    memset(query, 0, sizeof(query));
    char select1[] = "SELECT * FROM Question where question_id like '";
    sprintf(query, "%s%c%s%c' and question_content like '%c%s%c' and subject like '%c%s%c' and topic like '%c%s%c' limit %d, 10;", select1, '%', question_id, '%', '%', content, '%', '%', subject, '%', '%', topic, '%', number_of_question);
    mysql_free_result(res);
    res = make_query(query);
    int i = 0;
    question_data **question_list;
    question_list = malloc(sizeof(question_data*));
    *question_list = malloc(sizeof(question_data)*10);
    while((row = mysql_fetch_row(res)) && i < 10){
        (*question_list)[i].opcode = 202;
        strcpy((*question_list)[i].question_id, row[0]);
        strcpy((*question_list)[i].content, row[1]);
        strcpy((*question_list)[i].op[0], row[2]);
        strcpy((*question_list)[i].op[1], row[3]);
        strcpy((*question_list)[i].op[2], row[4]);
        strcpy((*question_list)[i].op[3], row[5]);
        i++;
    }
    //check question_list
    for(int j = 0; j < i; j++){
        printf("Question %d: %s\n", j+1, (*question_list)[j].question_id);
        printf("Content: %s\n", (*question_list)[j].content);
        printf("A: %s\n", (*question_list)[j].op[0]);
        printf("B: %s\n", (*question_list)[j].op[1]);
        printf("C: %s\n", (*question_list)[j].op[2]);
        printf("D: %s\n", (*question_list)[j].op[3]);
    }
    //Send question list to client
    send(socket, &i, sizeof(i), 0);
    char rcv[10];
    for(int j = 0; j < i; j++){
        recv(socket, rcv, sizeof(rcv), 0);
        if(strcmp(rcv, "OK") == 0){
            send(socket, ((*question_list)+j), sizeof(question_data), 0);
            continue;
        }
    }
    //Free memory
    mysql_free_result(res);
}

void addNewQuestion(int socket, char *buffer){
    //Convert buffer to add_new_question
    add_question *request = (add_question*)buffer;
    int user_id = request->user_id;
    printf("  ###Admin: %d add new question\n", user_id);
    char *question_id = request->question_id;
    char *content = request->question_content;
    char *op[4];
    for(int i = 0; i < 4; i++){
        op[i] = request->op[i];
    }
    int cop = request->ans;
    char *subject = request->subject;
    char *topic = request->topic;
    //Insert new question to database
    char query[MAX_QUERY_LEN];
    sprintf(query, "INSERT INTO Question(question_id, question_content, opa, opb, opc, opd, question_subject, question_topic) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s');", question_id, content, op[0], op[1], op[2], op[3], subject, topic);
    MYSQL_RES *res = make_query(query);
    memset(query, 0, sizeof(query));
    sprintf(query, "INSERT INTO Answer(question_id, ans) VALUES ('%s', %d);", question_id, cop);
    //Free memory
    mysql_free_result(res);
    res = make_query(query);
    //Send add new question success signal to client
    int oke_signal = 1;
    send(socket, &oke_signal, sizeof(oke_signal), 0);
    //Free memory
    mysql_free_result(res);
}

void deleteQuestion(int socket, char *buffer){
    //Convert buffer to delete_question
    delete_question *request = (delete_question*)buffer;
    int user_id = request->user_id;
    printf("  ###Admin: %d delete question by id\n", user_id);
    char *question_id = request->question_id;
    //Delete question from database
    char query[MAX_QUERY_LEN];
    sprintf(query, "DELETE FROM Question WHERE question_id = '%s';", question_id);
    MYSQL_RES *res = make_query(query);
    //Send delete question success signal to client
    int oke_signal = 1;
    send(socket, &oke_signal, sizeof(oke_signal), 0);
    //Free memory
    mysql_free_result(res);
}

//!TODO: Exam Operation
void searchExamById(int socket, char *buffer){
    //Convert buffer to search_exam_by_id
    search_exam_by_id *request = (search_exam_by_id*)buffer;
    int user_id = request->user_id;
    printf("  ###Admin: %d search exam by id\n", user_id);
    int exam_id = request->exam_id;
    //Get number of exam
    char query[MAX_QUERY_LEN];
    char select[] = "SELECT count(*) FROM Test where test_id = ";
    sprintf(query, "%s %d;", select, exam_id);
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    int number_of_exam = atoi(row[0]);
    printf("Number of exam: %d\n", number_of_exam);
    //Send number of exam to client
    int n = number_of_exam;
    send(socket, &n, sizeof(n), 0);
    if(number_of_exam == 0){
        mysql_free_result(res);
        return;
    }
    //Get exam from database
    memset(query, 0, sizeof(query));
    char select1[] = "SELECT * FROM Test where test_id = ";
    sprintf(query, "%s %d;", select1, exam_id);
    mysql_free_result(res);
    res = make_query(query);
    exam_data exam_list;
    row = mysql_fetch_row(res);
    (exam_list).opcode = 201;
    (exam_list).exam_id = atoi(row[0]);
    strcpy((exam_list).title, row[1]);
    (exam_list).number_of_question = atoi(row[2]);
    if(row[3] != NULL) (exam_list).admin_id = atoi(row[3]);
    else (exam_list).admin_id = 0;
    //check exam_list
    printf("Exam : %s\n", (exam_list).title);
    
    //Send exam list to client
    char rcv[10];
    send(socket, &(exam_list), sizeof(exam_data), 0);
    recv(socket, rcv, sizeof(rcv), 0);
    //Free memory
    mysql_free_result(res);
    if(strcmp(rcv, "OK") == 0) return;
    else {
        //!TODO: Process ERROR
    }
}

void createRandomExam(int socket, char *buffer){
    //Convert buffer to create_random_exam
    create_random_exam *request = (create_random_exam*)buffer;
    int user_id = request->user_id;
    printf("  ###Admin: %d create exam with random question\n", user_id);
    char *title = request->title;
    int number_of_question = request->number_of_question;
    int admin_id = request->admin_id;
    //Create new exam
    char query[MAX_QUERY_LEN];
    sprintf(query, "INSERT INTO Test(title, number_of_question, admin_id) VALUES ('%s', %d, %d);", title, number_of_question, admin_id);
    make_query(query);
    //Get test_id
    memset(query, 0, sizeof(query));
    char select[] = "SELECT test_id FROM Test where title = '";
    sprintf(query, "%s%s';", select, title);
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    int test_id = atoi(row[0]);
    printf("Test id: %d\n", test_id);
    //Get question list
    memset(query, 0, sizeof(query));
    char select1[] = "SELECT question_id FROM Question order by rand() limit";
    sprintf(query, "%s %d;", select1, number_of_question);
    mysql_free_result(res);
    res = make_query(query);
    int i = 0;
    char question_ids[number_of_question][255];
    while((row = mysql_fetch_row(res))){
        strcpy(question_ids[i], row[0]);
        i++;
    }  
    //Insert question list to Test_question
    for(int j = 0; j < number_of_question; j++){
        memset(query, 0, sizeof(query));
        sprintf(query, "INSERT INTO Test_question(test_id, question_id) VALUES (%d, '%s');", test_id, question_ids[j]);
        make_query(query);
    }
    //Send create exam success signal to client
    int oke_signal = 1;
    send(socket, &oke_signal, sizeof(oke_signal), 0);
}

void createExamByQuestion(int socket, char *buffer){
    //Convert buffer to create_exam_by_question
    create_exam *request = (create_exam*)buffer;
    int user_id = request->user_id;
    printf("  ###Admin: %d create exam\n", user_id);
    char *title = request->title;
    int number_of_question = request->number_of_ques;
    send(socket, &number_of_question, sizeof(number_of_question), 0);
    char question_ids[number_of_question][255];
    recv(socket, question_ids, sizeof(question_ids), 0);
    //Create new exam
    char query[MAX_QUERY_LEN];
    sprintf(query, "INSERT INTO Test(title, number_of_question) VALUES ('%s', %d);", title, number_of_question);
    MYSQL_RES *res = make_query(query);
    if(res == NULL){
        int reply = 0;
        send(socket, &reply, sizeof(reply), 0);
        return;
    }
    //Get test_id
    memset(query, 0, sizeof(query));
    char select[] = "SELECT test_id FROM Test where title = '";
    sprintf(query, "%s%s';", select, title);
    mysql_free_result(res);
    res = make_query(query);
    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    int test_id = atoi(row[0]);
    printf("Test id: %d\n", test_id);
    //Insert question list to Test_question
    mysql_free_result(res);
    for(int j = 0; j < number_of_question; j++){
        memset(query, 0, sizeof(query));
        sprintf(query, "INSERT INTO Test_question(test_id, question_id) VALUES (%d, '%s');", test_id, question_ids[j]);
        make_query(query);
    }
}

void deleteExam(int socket, char *buffer){
    //Convert buffer to delete_exam
    request_edit *request = (request_edit*)buffer;
    int user_id = request->user_id;
    printf("  ###Admin: %d delete exam by id\n", user_id);
    int exam_id = request->exam_id;
    //Delete exam from database
    char query[MAX_QUERY_LEN];
    sprintf(query, "DELETE FROM Test WHERE test_id = %d ;", exam_id);
    MYSQL_RES *res = make_query(query);
    //Send delete exam success signal to client
    int oke_signal = 1;
    send(socket, &oke_signal, sizeof(oke_signal), 0);
    //Free memory
    mysql_free_result(res);
}

//!TODO: Room Operation
void createRoom(int socket, char client_message[]){
    room_create_del *room = (room_create_del*)client_message;
    char query[MAX_QUERY_LEN];
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(query, "SELECT * FROM Room WHERE r_name = '%s';", room->r_name);
    res = make_query(query);
    row = mysql_fetch_row(res);
    
    int oke_signal;
    if(row == NULL){
        sprintf(query, "INSERT INTO Room(r_name, admin_id, create_date) VALUES ('%s', (select user_id from User_info where username = '%s'), CURDATE());", room->r_name, room->username);
        make_query(query);
        oke_signal = 1;
    }
    else{
        oke_signal = -1;
    }
    mysql_free_result(res);
    send(socket, &oke_signal, sizeof(int), 0);
}

void deleteRoom(int socket, char client_message[]){
    room_create_del *room = (room_create_del*)client_message;
    char query[MAX_QUERY_LEN];
    MYSQL_RES *res;
    MYSQL_ROW row;
    int oke_signal;

    sprintf(query, "SELECT * FROM Room WHERE r_name = '%s';", room->r_name);
    res = make_query(query);
    row = mysql_fetch_row(res);
    

    if(row == NULL){
        oke_signal = -1;
        send(socket, &oke_signal, sizeof(int), 0);
        mysql_free_result(res);
        return;
    }
    mysql_free_result(res);
    sprintf(query, "SELECT count(Room.r_id) from Room, User_info\
                    WHERE User_info.username = '%s' AND Room.r_name = '%s'\
                    AND Room.admin_id = User_info.user_id; ", room->username, room->r_name);
    res = make_query(query);
    row = mysql_fetch_row(res);
    int num = atoi(row[0]);
    

    if(num == 0){
        oke_signal = -2;
        send(socket, &oke_signal, sizeof(int), 0);
        mysql_free_result(res);
        return;
    }
    mysql_free_result(res);
    
    oke_signal = 1;
    sprintf(query, "DELETE FROM Room WHERE r_name = '%s';", room->r_name);
    make_query(query);
    send(socket, &oke_signal, sizeof(int), 0);
}


void showRoomYouCreated(int socket, char client_message[]){
    // printf("Call here?\n");
    //opcode = 803
    show_your_room *your_room = (show_your_room*)client_message;
    // typedef struct{
    // int opcode; // 800 for stroring information of the room
    // int r_id;
    // char r_name[256];
    // char admin_name[256];
    // char create_date[16];
    // }room_info;

    char query[MAX_QUERY_LEN];
    int num_of_room;
    room_info room;
    sprintf(query, "SELECT COUNT(*) FROM Room, User_info WHERE User_info.username = '%s' AND Room.admin_id = User_info.user_id;", your_room->username);
    // printf("Before make query\n");
    MYSQL_RES *res = make_query(query);
    // printf("make query oke\n");
    MYSQL_ROW row = mysql_fetch_row(res);
    
    num_of_room = atoi(row[0]);
    mysql_free_result(res);

    
    // printf("server received yet?\n");
    send(socket, &num_of_room, sizeof(int), 0);
    // printf("Server send oke?\n");

    if(num_of_room == 0){
        return;
    }

    char oke_signal[OKE_SIGNAL_LEN];
    memset(oke_signal, 0, OKE_SIGNAL_LEN);

    recv(socket, oke_signal, OKE_SIGNAL_LEN, 0);

    sprintf(query, "SELECT r.r_id, r.r_name, u.username, r.create_date FROM Room as r, User_info as u WHERE u.username = '%s' AND u.user_id = r.admin_id;", your_room->username);
    res = make_query(query);
    while((row = mysql_fetch_row(res)) != NULL){
        memset(&room, 0, sizeof(room_info));
        room.r_id = atoi(row[0]);
        strcpy(room.r_name, row[1]);
        strcpy(room.admin_name, row[2]);
        strcpy(room.create_date, row[3]);
        send(socket, &room, sizeof(room_info), 0);
        recv(socket, oke_signal, OKE_SIGNAL_LEN, 0);
    }

    mysql_free_result(res);
    send(socket, oke_signal, OKE_SIGNAL_LEN, 0);

}




void showRoomYouWereAdded(int socket, char client_message[]){
    //opcode = 804
    //opcode = 803
    show_your_room *your_room = (show_your_room*)client_message;
    // typedef struct{
    // int opcode; // 800 for stroring information of the room
    // int r_id;
    // char r_name[256];
    // char admin_name[256];
    // char create_date[16];
    // }room_info;

    char query[MAX_QUERY_LEN];
    int num_of_room;
    room_info room;
    sprintf(query, "SELECT COUNT(*) FROM User_info, Room_user WHERE User_info.username = '%s' AND Room_user.user_id = User_info.user_id;", your_room->username);
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row = mysql_fetch_row(res);
    
    num_of_room = atoi(row[0]);
    mysql_free_result(res);

    send(socket, &num_of_room, sizeof(int), 0);

    
    if(num_of_room == 0){
        return;
    }

    char oke_signal[OKE_SIGNAL_LEN];
    memset(oke_signal, 0, OKE_SIGNAL_LEN);

    recv(socket, oke_signal, OKE_SIGNAL_LEN, 0);

    

    sprintf(query, "SELECT r.r_id, r.r_name, u.username, r.create_date\
                    FROM Room as r, User_info as u\
                    WHERE r.admin_id = u.user_id AND r.r_id = (select r_id from Room_user, User_info\
                    where Room_user.user_id = User_info.user_id\
                    AND User_info.username = '%s');", your_room->username);
    
    res = make_query(query);
    while((row = mysql_fetch_row(res)) != NULL){
        memset(&room, 0, sizeof(room_info));
        room.r_id = atoi(row[0]);
        strcpy(room.r_name, row[1]);
        strcpy(room.admin_name, row[2]);
        strcpy(room.create_date, row[3]);
        send(socket, &room, sizeof(room_info), 0);
        recv(socket, oke_signal, OKE_SIGNAL_LEN, 0);
    }
    mysql_free_result(res);


    send(socket, oke_signal, OKE_SIGNAL_LEN, 0);

    

}

void showAllYourRoom(int socket, char client_message[]){
    //opcode = 805
    show_your_room *your_room = (show_your_room*)client_message;
    // typedef struct{
    // int opcode; // 800 for stroring information of the room
    // int r_id;
    // char r_name[256];
    // char admin_name[256];
    // char create_date[16];
    // }room_info;

    char query[MAX_QUERY_LEN];
    int num_of_room;
    room_info room;
    sprintf(query, "SELECT COUNT(*) FROM Room, User_info WHERE User_info.username = '%s' AND Room.admin_id = User_info.user_id;", your_room->username);
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row = mysql_fetch_row(res);
    
    num_of_room = atoi(row[0]);
    mysql_free_result(res);

    sprintf(query, "SELECT COUNT(*) FROM User_info, Room_user WHERE User_info.username = '%s' AND Room_user.user_id = User_info.user_id;", your_room->username);
    res = make_query(query);
    row = mysql_fetch_row(res);
    
    num_of_room = num_of_room + atoi(row[0]);
    mysql_free_result(res);


    
    send(socket, &num_of_room, sizeof(int), 0);

    if(num_of_room == 0){
        return;
    }

    char oke_signal[OKE_SIGNAL_LEN];
    memset(oke_signal, 0, OKE_SIGNAL_LEN);

    recv(socket, oke_signal, OKE_SIGNAL_LEN, 0);

    sprintf(query, "SELECT r.r_id, r.r_name, u.username, r.create_date FROM Room as r, User_info as u WHERE u.username = '%s' AND u.user_id = r.admin_id;", your_room->username);
    res = make_query(query);
    while((row = mysql_fetch_row(res)) != NULL){
        memset(&room, 0, sizeof(room_info));
        room.r_id = atoi(row[0]);
        strcpy(room.r_name, row[1]);
        strcpy(room.admin_name, row[2]);
        strcpy(room.create_date, row[3]);
        send(socket, &room, sizeof(room_info), 0);
        recv(socket, oke_signal, OKE_SIGNAL_LEN, 0);
    }

    mysql_free_result(res);

    
    sprintf(query, "SELECT r.r_id, r.r_name, u.username, r.create_date\
                    FROM Room as r, User_info as u\
                    WHERE r.admin_id = u.user_id AND r.r_id = (select r_id from Room_user, User_info\
                    where Room_user.user_id = User_info.user_id\
                    AND User_info.username = '%s');", your_room->username);
    
    res = make_query(query);
    while((row = mysql_fetch_row(res)) != NULL){
        memset(&room, 0, sizeof(room_info));
        room.r_id = atoi(row[0]);
        strcpy(room.r_name, row[1]);
        strcpy(room.admin_name, row[2]);
        strcpy(room.create_date, row[3]);
        send(socket, &room, sizeof(room_info), 0);
        recv(socket, oke_signal, OKE_SIGNAL_LEN, 0);
    }
    mysql_free_result(res);


    
    send(socket, oke_signal, OKE_SIGNAL_LEN, 0);
}

void addUserToRoom(int socket, char client_message[]){
    //opcode = 811

    /*
    Các trường hợp có thể xảy ra:
        - user_id ko tồn tại
            -> flag = -2
        - user_id đã được add vào rồi
            -> flag = -3
        - user_id  trùng với admin_id (người tạo phòng) => cũng ko được add
            -> flag = -4
        - add user oke => 1
    */

    request_add_remove_student *request = (request_add_remove_student*)client_message;
    
    char query[MAX_QUERY_LEN];
    MYSQL_RES *res;
    MYSQL_ROW row;
    int flag;

    // user_id  ko tồn tại => -2
    {
        sprintf(query, "SELECT * FROM User_info WHERE user_id = '%d';", request->user_id);
        res = make_query(query);
        row = mysql_fetch_row(res);
        if(row == NULL){
            mysql_free_result(res);
            flag = -2;
            send(socket, &flag, sizeof(int), 0);
            return;
        }
        mysql_free_result(res);
    }

    // user_id  được add vảo rồi => -3
    {
        sprintf(query, "SELECT * from Room_user WHERE r_id = '%d' AND user_id = '%d';", request->r_id, request->user_id);
        res = make_query(query);
        row = mysql_fetch_row(res);
        if(row != NULL){
            mysql_free_result(res);
            flag = -3;
            send(socket, &flag, sizeof(int), 0);
            return;
        }
        mysql_free_result(res);
    }

    // user_id là người tạo ra phòng => -4
    {
        sprintf(query, "SELECT * from Room WHERE r_id = '%d' AND admin_id = '%d';", request->r_id, request->user_id);
        res = make_query(query);
        row = mysql_fetch_row(res);
        if(row != NULL){
            mysql_free_result(res);
            flag = -4;
            send(socket, &flag, sizeof(int), 0);
            return;
        }
        mysql_free_result(res);
    }


    //1 success: 
    {
        sprintf(query, "INSERT INTO Room_user VALUES (%d, %d);", request->r_id, request->user_id);
        make_query(query);
        flag = 1;
        send(socket, &flag, sizeof(int), 0);
    }
}

void deleteUserFromRoom(int socket, char client_message[]){
    //opcode = 812

    /*
    Các trường hợp có thể xảy ra:
        - user_id ko ở trong room
            -> flag = -2
        - add user oke => 1
    */

    request_add_remove_student *request = (request_add_remove_student*)client_message;
    
    char query[MAX_QUERY_LEN];
    MYSQL_RES *res;
    MYSQL_ROW row;
    int flag;

    // user_id ko ở trong room -> -2
    {
        sprintf(query, "SELECT * from Room_user WHERE r_id = '%d' AND user_id = '%d';", request->r_id, request->user_id);
        res = make_query(query);
        row = mysql_fetch_row(res);
        if(row == NULL){
            mysql_free_result(res);
            flag = -2;
            send(socket, &flag, sizeof(int), 0);
            return;
        }
        mysql_free_result(res);
    }

    //1 success: 
    {
        sprintf(query, "DELETE FROM Room_user Where r_id = '%d' AND user_id = '%d';", request->r_id, request->user_id);
        make_query(query);
        flag = 1;
        send(socket, &flag, sizeof(int), 0);
    }
}



//!TODO: Admin request operation
void getAdminRequestInfo(int socket, char *buffer){
    //Convert buffer to approve_admin_request
    check_admin_previlege_request *request = (check_admin_previlege_request*)buffer;
    int user_id = request->user_id;
    printf("  ###Admin: %d get admin request list\n", user_id);
    //Query database to get number of request
    char query[MAX_QUERY_LEN];
    char select[] = "SELECT count(*) FROM Request_admin";
    MYSQL_RES *res = make_query(select);
    MYSQL_ROW row;
    int number_of_request = atoi(row[0]);
    printf("Number of request: %d\n", number_of_request);
    //Send number of request to client
    send(socket, &number_of_request, sizeof(number_of_request), 0);
    //No request found
    if(number_of_request == 0){
        mysql_free_result(res);
        return;
    }
    //Get request from database
    recv(socket, &number_of_request, sizeof(number_of_request), 0); //receive begin index
    memset(query, 0, sizeof(query));
    char select1[] = "SELECT * FROM Request_admin limit "; 
    sprintf(query, "%s %d, 10;", select1, number_of_request);
    mysql_free_result(res);
    res = make_query(query);
    int i = 0;
    admin_request **request_list;
    request_list = malloc(sizeof(admin_request*));
    *request_list = malloc(sizeof(admin_request)*number_of_request);
    while((row = mysql_fetch_row(res))){
        (*request_list)[i].opcode = 303;
        (*request_list)[i].id = atoi(row[0]);
        strcpy((*request_list)[i].username, row[1]);
        strcpy((*request_list)[i].date, row[2]);
        i++;
    }
    //check request_list
    for(int j = 0; j < number_of_request; j++){
        printf("Request %d: %d\n", j+1, (*request_list)[j].id);
        printf("Username: %s\n", (*request_list)[j].username);
        printf("Request date: %s\n", (*request_list)[j].date);
    }
    //Send request list to client
    char rcv[10];
    send(socket, &i, sizeof(i), 0);
    for(int j = 0; j < i; j++){
        recv(socket, rcv, sizeof(rcv), 0);
        if(strcmp(rcv, "OK") == 0){
            send(socket, ((*request_list)+j), sizeof(admin_request), 0);
            continue;
        }
    }
    //Free memory
    mysql_free_result(res);
}

void approveAdminRequest(int socket, char *buffer){
    //Convert buffer to approve_admin_request
    approve_admin_request *request = (approve_admin_request*)buffer;
    int user_id = request->user_id;
    printf("  ###Admin: %d approve admin request\n", user_id);
    char *username = request->username;
    //Delete request from database
    char query[MAX_QUERY_LEN];
    sprintf(query, "DELETE FROM Request_admin WHERE username = '%s';", username);
    MYSQL_RES *res = make_query(query);
    if(res == NULL){
        int reply = 0;
        send(socket, &reply, sizeof(reply), 0);
        return;
    }
    //Update User_info
    memset(query, 0, sizeof(query));
    sprintf(query, "UPDATE User_info SET role = 1 WHERE username = '%s';", username);
    //Free memory
    mysql_free_result(res);
    make_query(query);
    //Send approve request success signal to client
    int oke_signal = 1;
    send(socket, &oke_signal, sizeof(oke_signal), 0);
}

//!####### END ADMIN FUNCTIONS #######


//!####### ADVANCED FUNCTION #######
//!TODO: Get image question
void getImageQuestion(int sockfd, char *buffer){
    //Convert buffer to get_image_question
    image_exam *request = (image_exam*)buffer;
    int user_id = request->user_id;
    printf("  ###Admin: %d get image embbed question\n", user_id);
    int num_question = request->number_of_question;
    //Query database to get question
    char query[MAX_QUERY_LEN];
    char select[] = "SELECT * FROM Advance_question WHERE question_type = 1 order by rand() limit";
    sprintf(query, "%s %d;", select, num_question);
    //printf("Query: %s\n", query);
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row;
    int i = 0;
    printf("Number of question: %d\n", num_question);
    question_image **question_list;
    question_list = malloc(sizeof(question_image*));
    *question_list = malloc(sizeof(question_image)*num_question);
    char image_path_str[num_question][255];
    while((row = mysql_fetch_row(res))){
        strcpy((*question_list)[i].question_id, row[0]);
        strcpy((*question_list)[i].question_content, row[1]);
        if(row[2] != NULL) strcpy((*question_list)[i].op[0], row[2]);
        if(row[3] != NULL) strcpy((*question_list)[i].op[1], row[3]);
        if(row[4] != NULL) strcpy((*question_list)[i].op[2], row[4]);
        if(row[5] != NULL) strcpy((*question_list)[i].op[3], row[5]);
        (*question_list)[i].ans = atoi(row[6]);
        strcpy((*question_list)[i].image_path, row[7]);
        i++;
    }

    //check question_list
    for(int j = 0; j < i; j++){
        printf("Question %d: %s\n", j+1, (*question_list)[j].question_id);
        printf("Content: %s\n", (*question_list)[j].question_content);
        printf("A: %s\n", (*question_list)[j].op[0]);
        printf("B: %s\n", (*question_list)[j].op[1]);
        printf("C: %s\n", (*question_list)[j].op[2]);
        printf("D: %s\n", (*question_list)[j].op[3]);
        printf("Image path: %s\n", (*question_list)[j].image_path);
    }

    //Send all images to client
    char rcv[10];
    send(sockfd, &i, sizeof(i), 0);
    for(int j = 0; j < i; j++){
        recv(sockfd, rcv, sizeof(rcv), 0);
        if(strcmp(rcv, "OK") == 0){
        char image_full_path[1000];
        sprintf(image_full_path, "/home/duy/NetworkProgramming/Project/main/main_reconstruct/server/source/image/%s", (*question_list)[j].image_path);
        //Open image file
        FILE *image = fopen(image_full_path, "rb");
        if(image == NULL){
            printf("Error: Image file not found\n");
            send(sockfd, "ERROR", sizeof("ERROR"), 0);
            continue;
        }
        //Send image name to client
        printf("Image path: %s\n", image_full_path);
        send(sockfd, (*question_list)[j].image_path, sizeof((*question_list)[j].image_path), 0);
        recv(sockfd, rcv, sizeof(rcv), 0);
        if(strcmp(rcv, "OK") == 0){
            char image_buffer[1024];
            while (!feof(image)) {
                fread(image_buffer, sizeof(image_buffer), 1, image);
                send(sockfd, image_buffer, sizeof(image_buffer), 0);
                recv(sockfd, rcv, sizeof(rcv), 0);
                memset(image_buffer, 0, sizeof(image_buffer));
		    }
        }else{
            fclose(image);
            return;
        }
        send(sockfd, "END", sizeof("END"), 0);
        //Close file
        fclose(image);
        }
    }

    //Send question list to client
    send(sockfd, &i, sizeof(i), 0);
    for(int j = 0; j < i; j++){
        recv(sockfd, rcv, sizeof(rcv), 0);
        if(strcmp(rcv, "OK") == 0){
            send(sockfd, ((*question_list)+j), sizeof(question_image), 0);
            continue;
        }
    }

    //Get answer from client
    char answer[num_question+1];
    recv(sockfd, answer, sizeof(answer), 0);
    //Evaluate all question
    int score = 0;
    for(int j = 0; j < num_question; j++){
        if(answer[j] - 'A' + 1 == (*question_list)[j].ans) score++;
    }
    //Send score to client
    send(sockfd, &score, sizeof(score), 0);
    //Free memory
    mysql_free_result(res);
}

//!TODO: Get sound question
void getSoundQuestion(int sockfd, char *buffer){
    //Convert buffer to get_image_question
    sound_exam *request = (sound_exam*)buffer;
    int user_id = request->user_id;
    printf("  ###Admin: %d get sound embbed question\n", user_id);
    int num_question = request->number_of_question;
    //Query database to get question
    char query[MAX_QUERY_LEN];
    char select[] = "SELECT * FROM Advance_question WHERE question_type = 2 order by rand() limit";
    sprintf(query, "%s %d;", select, num_question);
    //printf("Query: %s\n", query);
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row;
    int i = 0;
    printf("Number of question: %d\n", num_question);
    question_image **question_list;
    question_list = malloc(sizeof(question_image*));
    *question_list = malloc(sizeof(question_image)*num_question);
    char image_path_str[num_question][255];
    while((row = mysql_fetch_row(res))){
        strcpy((*question_list)[i].question_id, row[0]);
        strcpy((*question_list)[i].question_content, row[1]);
        if(row[2] != NULL) strcpy((*question_list)[i].op[0], row[2]);
        if(row[3] != NULL) strcpy((*question_list)[i].op[1], row[3]);
        if(row[4] != NULL) strcpy((*question_list)[i].op[2], row[4]);
        if(row[5] != NULL) strcpy((*question_list)[i].op[3], row[5]);
        (*question_list)[i].ans = atoi(row[6]);
        strcpy((*question_list)[i].image_path, row[7]);
        i++;
    }

    //check question_list
    for(int j = 0; j < i; j++){
        printf("Question %d: %s\n", j+1, (*question_list)[j].question_id);
        printf("Content: %s\n", (*question_list)[j].question_content);
        printf("A: %s\n", (*question_list)[j].op[0]);
        printf("B: %s\n", (*question_list)[j].op[1]);
        printf("C: %s\n", (*question_list)[j].op[2]);
        printf("D: %s\n", (*question_list)[j].op[3]);
        printf("Audio path: %s\n", (*question_list)[j].image_path);
    }

    //Send all images to client
    char rcv[10];
    send(sockfd, &i, sizeof(i), 0);
    for(int j = 0; j < i; j++){
        recv(sockfd, rcv, sizeof(rcv), 0);
        if(strcmp(rcv, "OK") == 0){
        char audio_full_path[1000];
        sprintf(audio_full_path, "/home/duy/NetworkProgramming/Project/main/main_reconstruct/server/source/soundtrack/%s", (*question_list)[j].image_path);
        //Open image file
        FILE *image = fopen(audio_full_path, "rb");
        if(image == NULL){
            printf("Error: Image file not found\n");
            send(sockfd, "ERROR", sizeof("ERROR"), 0);
            continue;
        }
        //Send image name to client
        printf("Audio path: %s\n", audio_full_path);
        send(sockfd, (*question_list)[j].image_path, sizeof((*question_list)[j].image_path), 0);
        recv(sockfd, rcv, sizeof(rcv), 0);
        if(strcmp(rcv, "OK") == 0){
            char image_buffer[1024];
            while (!feof(image)) {
                fread(image_buffer, sizeof(image_buffer), 1, image);
                send(sockfd, image_buffer, sizeof(image_buffer), 0);
                recv(sockfd, rcv, sizeof(rcv), 0);
                memset(image_buffer, 0, sizeof(image_buffer));
		    }
        }else{
            fclose(image);
            return;
        }
        send(sockfd, "END", sizeof("END"), 0);
        //Close file
        fclose(image);
        }
    }

    //Send question list to client
    send(sockfd, &i, sizeof(i), 0);
    for(int j = 0; j < i; j++){
        recv(sockfd, rcv, sizeof(rcv), 0);
        if(strcmp(rcv, "OK") == 0){
            send(sockfd, ((*question_list)+j), sizeof(question_image), 0);
            continue;
        }
    }

    //Get answer from client
    char answer[num_question+1];
    recv(sockfd, answer, sizeof(answer), 0);
    //Evaluate all question
    int score = 0;
    for(int j = 0; j < num_question; j++){
        if(answer[j] - 'A' + 1 == (*question_list)[j].ans) score++;
    }
    //Send score to client
    send(sockfd, &score, sizeof(score), 0);
    //Free memory
    mysql_free_result(res);
}

//!TODO: Get user statistic
void getUserStatistic(int sockfd, char *buffer){

}
//!####### END ADVANCED FUNCTION #######


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