#include "../data.h"
#include "server.h"
#define MAX_QUERY_LEN 3000
char start_time_exam[22];
char end_time_exam[22];
int key_arr[1000];

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
    sprintf(query, "INSERT INTO Test(title, num_of_question, admin_id) VALUES ('%s', %d, %d);", title, number_of_question, admin_id);
    MYSQL_RES *res = make_query(query);
    //Get test_id
    memset(query, 0, sizeof(query));
    char select[] = "SELECT test_id FROM Test where title = '";
    sprintf(query, "%s%s';", select, title);
    //Free memory
    mysql_free_result(res);
    res = make_query(query);
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


// May quá, Việt để cho mình
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

void addTestFromBank(int socket, char client_message[]){
    //opcode = 813
    char query[MAX_QUERY_LEN];
    MYSQL_RES *res;
    MYSQL_ROW row;
    int num_of_test_in_bank;
    int oke_signal = 1;
    int test_id;

    {
        sprintf(query, "SELECT COUNT(*) FROM Test_private;");
        res = make_query(query);
        row = mysql_fetch_row(res);
        num_of_test_in_bank = atoi(row[0]);
        mysql_free_result(res);

        send(socket, &num_of_test_in_bank, sizeof(int), 0);
        recv(socket, &oke_signal, sizeof(int), 0);

        sprintf(query, "SELECT * FROM Test_private;");
        res = make_query(query);

        test_in_bank test_info;

        for(int i=0; i<num_of_test_in_bank; i++){
            row = mysql_fetch_row(res);
            memset(&test_info, 0, sizeof(test_in_bank));
            test_info.test_id = atoi(row[0]);
            strcpy(test_info.test_name, row[1]);
            test_info.number_of_question = atoi(row[2]);

            send(socket, &test_info, sizeof(test_in_bank), 0);
            recv(socket, &oke_signal, sizeof(int), 0);
        }

        mysql_free_result(res);
    }

    room_test test_data;
    memset(&test_data, 0, sizeof(room_test));
    send(socket, &oke_signal, sizeof(int), 0);
    char check_signal[10];
    recv(socket, check_signal, 10, 0);
    if(strcmp(check_signal, "OK") != 0) return;
    send(socket, &oke_signal, sizeof(int), 0);
    recv(socket, &test_data, sizeof(room_test), 0);

    sprintf(query, "INSERT INTO Room_test VALUES (%d, %d, '%s', '%s', %d);", test_data.r_id, test_data.test_id, test_data.open_time, test_data.close_time, test_data.duration);
    make_query(query);
    send(socket, &oke_signal, sizeof(int), 0);

}


void createTest(int socket, char client_message[]){
    //opcode = 814
    int flag =0;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[MAX_QUERY_LEN];
    int oke_signal = 1;
    char test_name[256];
    char admin_name[64];
    int num_of_question;
    int admin_id;

    send(socket, &oke_signal, sizeof(int), 0);
    //check test_name to make sure no duplicate
    while(1){
        
        recv(socket, test_name, 256, 0);
        sprintf(query, "SELECT * FROM Test_private WHERE test_name = '%s';", test_name);
        res = make_query(query);
        row = mysql_fetch_row(res);
        if(row == NULL){
            flag = 1;
        }
        else{
            flag = -1;
        }
        mysql_free_result(res);
        send(socket, &flag, sizeof(int), 0);
        if(flag == 1){
            break;
        }
    }


    //receive the num_of_question
    recv(socket, &num_of_question, sizeof(int), 0);

    //receive admin_name to get the admin_id
    send(socket, &oke_signal, sizeof(int), 0);
    recv(socket, admin_name, 64, 0);

    //make query to get admin_id
    {
        sprintf(query, "SELECT user_id FROM User_info WHERE username = '%s';", admin_name);
        res = make_query(query);
        row = mysql_fetch_row(res);
        admin_id = atoi(row[0]);
        mysql_free_result(res);
    }
    
    //make query to create new test
    {   
        
        sprintf(query, "INSERT INTO Test_private(test_name, num_of_question, admin_id) VALUES ('%s', %d, %d);", test_name, num_of_question, admin_id);
        make_query(query);

        sprintf(query, "SELECT question_id FROM Question ORDER BY RAND() LIMIT %d;", num_of_question);
        res = make_query(query);

        char list_question_id[num_of_question][300];
        for(int i=0; i<num_of_question; i++){
            row = mysql_fetch_row(res);
            strcpy(list_question_id[i], row[0]);
        }
        mysql_free_result(res);

        int test_id;
        sprintf(query, "SELECT test_id FROM Test_private WHERE test_name = '%s';", test_name);
        res = make_query(query);
        row = mysql_fetch_row(res);
        test_id = atoi(row[0]);
        mysql_free_result(res);


        for(int i=0; i<num_of_question; i++){
            sprintf(query, "INSERT INTO r_Test_question VALUES (%d, '%s')", test_id, list_question_id[i]);
            make_query(query);
        }

    }

    send(socket, &oke_signal, sizeof(int), 0);
}



// void addTestFromBank(int socket, char client_message[]){
//     //opcode = 813
// }

void createRandomPrivateTest(int socket, char client_message[]){
    //Convert client message to createPrivateTest
    createPrivateTest *request = (createPrivateTest*)client_message;
    char *test_name = request->test_name;
    int number_of_question = request->number_of_question;
    int admin_id = request->admin_id;
    //Create new private Exam
    char query[MAX_QUERY_LEN];
    sprintf(query, "INSERT INTO Test_private(test_name, num_of_question, admin_id) VALUES ('%s', %d, %d);", test_name, number_of_question, admin_id);
    make_query(query);
    //Get test_id

    memset(query, 0, sizeof(query));
    char select[] = "SELECT test_id FROM Test_private where test_name = '";
    sprintf(query, "%s%s';", select, test_name);
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

    //Insert question to list to r_Test_question
    for(int j = 0; j < number_of_question; j++){
        memset(query, 0, sizeof(query));
        sprintf(query, "INSERT INTO r_Test_question(test_id, question_id) VALUES (%d, '%s');", test_id, question_ids[j]);
        make_query(query);
    }
    //Send create exam success signal to client
    int oke_signal = 1;
    send(socket, &oke_signal, sizeof(oke_signal), 0);
}

void createPrivateTestByQuestion(int socket, char client_message[]){
    createPrivateTest *request = (createPrivateTest*)client_message;
    char *test_name = request->test_name;
    int number_of_question = request->number_of_question;
    send(socket, &number_of_question, sizeof(number_of_question), 0);
    char question_ids[number_of_question][255];
    recv(socket, question_ids, sizeof(question_ids), 0);
    //Create new exam
    char query[MAX_QUERY_LEN];
    sprintf(query, "INSERT INTO Test_private(test_name, number_of_question) VALUES ('%s', %d);", test_name, number_of_question);
    MYSQL_RES *res = make_query(query);
    /*
    if(res == NULL){
        int reply = 0;
        send(socket, &reply, sizeof(reply), 0);
        return;
    }
    */

    //Get test id
    memset(query, 0, sizeof(query));
    char select[] = "SELECT test_id FROM Test_private where test_name = '";
    sprintf(query, "%s%s';", select, test_name);
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
        sprintf(query, "INSERT INTO r_Test_question(test_id, question_id) VALUES (%d, '%s');", test_id, question_ids[j]);
        make_query(query);
    }
    int oke_signal = 1;
    send(socket, &oke_signal, sizeof(oke_signal), 0);
}

/*
void addTestToRoom(int socket, char client_message[]){
    add_test_to_room *request = (add_test_to_room*)client_message;
    int test_id = request->test_id;
    int room_id = request->r_id;
    int duration = request->duration;
    char open_time[22];
    char close_time[22];
    int oke_signal = 1;
    strcpy(open_time, request->open_time);
    strcpy(close_time, request->close_time);
    //Add test to the room
    char query[MAX_QUERY_LEN];
    sprintf(query, "INSERT INTO Room_test(r_id, test_id, open_time, close_time, duration) VALUES (%d, %d, '%s', '%s', %d);", room_id, test_id, open_time, close_time, duration);
    make_query(query);
    send(socket, &oke_signal, sizeof(int), 0); // Send successfull message to client.
}
*/

//Function for shuffle question in random order
void Shuffle(int arr[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        // Swap arr[i] and arr[j]
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}


//Function for get the current date
char *getDate(char cur[]){
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  sprintf(cur, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  return cur;
}

//Function to compare 2 time string
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

//Function to get the different of 2 time string
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



// void showTestInRoom(int socket, char client_message[]){
//     request_get_test_in_room *request = (request_get_test_in_room*)client_message;
//     int room_id = request->room_id;
//     char get_test
// }



void getQuestionInRoom(int socket, char client_message[]){
    char cur[20];
    srand(time(NULL));

    request_question_list *request = (request_question_list*)client_message;
    int test_id = request->exam_id;
    int num_question = request->number_of_question;
    //Get question for the exam from database
    char get_test[] = "SELECT * FROM Room_test WHERE test_id =";
    char qr[MAX_QUERY_LEN];
    sprintf(qr, "%s %d;", get_test, test_id);
    MYSQL_RES *res = make_query(qr);
    MYSQL_ROW row;
    char start_time[22];
    char end_time[22];
    while(row = mysql_fetch_row(res)){
        strcpy(start_time, row[2]);
        strcpy(end_time, row[3]);
    }
    getDate(cur);
    //Check if the current time in the allowed domain of time
    if(!(compareDateTime(cur, start_time) == 1 && compareDateTime(cur, end_time) == -1)){
        char signal[] = "NOT_OK";
        send(socket, signal, sizeof(signal)+1, 0); //Send signal for client to inform not in open time 
    }
     else{//Request question
        char signal[] = "OK";
        send(socket, signal, sizeof(signal)+1, 0); 
    char query[MAX_QUERY_LEN];
    char select[] = "SELECT * FROM r_Test_question where test_id =";
    sprintf(query, "%s %d;", select, test_id);
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row;
    int i = 0;
    char question_ids[num_question][255];
    while((row = mysql_fetch_row(res))){
        strcpy(question_ids[i], row[0]);
        i++;
    }  
    question_data **question_list;
    question_list = malloc(sizeof(question_data*));
    *question_list = malloc(sizeof(question_data)*num_question);
    
    int random_ques[num_question];
    for(int i = 0; i < num_question; i++){
        random_ques[i] = i + 1;
    }
    
    Shuffle(random_ques, num_question);

    for(int j = 0; j < num_question; j++){
        char query[MAX_QUERY_LEN];
        char select[] = "SELECT * FROM Question where question_id =";
        sprintf(query, "%s '%s';", select, question_ids[random_ques[j]]);
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
    for(int i = 0; i < num_question; i++){
        key_arr[i] = random_ques[i];
    } 
    getDate(start_time_exam);
}
}

void evaluateExamInRoom(int socket, char client_message[]){
    getDate(end_time_exam);
    submit_ans *ans = (submit_ans*)client_message;
    int test_id = ans->exam_id;

    char get_test[] = "SELECT * FROM Room_test WHERE test_id =";
    char qr[MAX_QUERY_LEN];
    sprintf(qr, "%s %d;", get_test, test_id);
    MYSQL_RES * res = make_query(qr);
    MYSQL_ROW row;
    int duration;
    char end_time[22];

    while((row = mysql_fetch_row(res))){
        strcpy(end_time, row[3]);
        duration = atoi(row[4]);
    }
    if(compareDateTime(end_time_exam, end_time) == 1){
        char signal[] = "NOT_OK";
        send(socket, signal, sizeof(signal)+1, 0); //Send signal for client to inform that submit the exam late.
    }
     else{
        char signal[] = "OK";
        send(socket, signal, sizeof(signal)+1, 0);
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
    char select[] = "SELECT * FROM r_Test_question where test_id =";
    sprintf(query, "%s %d;", select, test_id);
    MYSQL_RES *res = make_query(query);
    MYSQL_ROW row;
    int i = 0;
    char question_ids[num_question][255];
    while((row = mysql_fetch_row(res))){
        strcpy(question_ids[i], row[0]);
        i++;
    }  
    //Query database to get correct answer
    char correct_ans[num_question+1];
    for(int j = 0; j < num_question; j++){
        char query[MAX_QUERY_LEN];
        char select[] = "SELECT * FROM Answer where question_id =";
        sprintf(query, "%s '%s';", select, question_ids[key_arr[j]]);
        MYSQL_RES *res = make_query(query);
        MYSQL_ROW row;
        while((row = mysql_fetch_row(res))){
            correct_ans[j] = atoi(row[1]) + 'A' - 1;
        }
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
    int doing_time = getDifferentTime(start_time_exam, end_time_exam);
    if(doing_time < duration * 0.5 && point / num_question > 0.8){
        char signal[] = "NOT_OK";
        send(socket, signal, sizeof(signal)+1, 0); //Send signal for client to inform maybe cheating
    }
    else{
        char signal[] = "OK";
        send(socket, signal, sizeof(signal)+1, 0);
    }
    send(socket, &point, sizeof(point), 0);

    //Saved user result to database
    saveHistory(ans->username, test_id, point, num_question);
}
}


void deleteTestFromRoom(int socket, char client_message[]){
    //opcode = 815

}


void Admin_showSubmittedRecordOfTest(int socket, char client_message[]){
    //opcode = 816
    char query[MAX_QUERY_LEN];
    MYSQL_RES *res;
    MYSQL_ROW row;
    int number_of_history_records;
    int number_of_tests_in_room;
    int oke_signal = 1;
    int r_id;
    int test_id;
    room_history history_data;
    test_in_room test_data;
    
    
    //receive r_id
    {
        send(socket, &oke_signal, sizeof(int), 0);
        recv(socket, &r_id, sizeof(int), 0);
        printf("r_id = %d\n", r_id);
    }


    //send number of tests in room
    {
        sprintf(query, "SELECT COUNT(*) FROM Room_test WHERE r_id = %d;", r_id);
        res = make_query(query);
        row = mysql_fetch_row(res);
        number_of_tests_in_room = atoi(row[0]);
        
        send(socket, &number_of_tests_in_room, sizeof(int), 0);
        if(number_of_history_records == 0){
            return;
        }
        recv(socket, &oke_signal, sizeof(int), 0);
    }
    mysql_free_result(res);
    //send list_test
    {
        sprintf(query, "SELECT r.test_id, t.test_name, t.num_of_question \
                        FROM Room_test as r, Test_private as t \
                        WHERE r.r_id = %d and t.test_id = r.test_id;", r_id);
        res = make_query(query);
        for(int i=0; i<number_of_tests_in_room; i++){
            memset(&test_data, 0, sizeof(test_in_room));
            row = mysql_fetch_row(res);
            test_data.test_id = atoi(row[0]);
            strcpy(test_data.test_name, row[1]);
            test_data.num_of_question = atoi(row[2]);

            send(socket, &test_data, sizeof(test_in_room), 0);
            recv(socket, &oke_signal, sizeof(int), 0);
        }
        mysql_free_result(res);
    }

    //receive test_id
    {
        send(socket, &oke_signal, sizeof(int), 0);
        recv(socket, &test_id, sizeof(int), 0);
        if(test_id == 0){
            send(socket, &oke_signal, sizeof(int), 0);
            return;
        }
    }
    
    //send number of history_records
    {
        sprintf(query, "SELECT COUNT(*) FROM Room_history WHERE r_id = %d AND test_id = %d;", r_id, test_id);
        res = make_query(query);
        row = mysql_fetch_row(res);
        number_of_history_records = atoi(row[0]);
        mysql_free_result(res);
        
        send(socket, &number_of_history_records, sizeof(int), 0);

        if(number_of_history_records == 0){
            return;
        }

        recv(socket, &oke_signal, sizeof(int), 0);
    }

    //send records
    {
        sprintf(query, "SELECT * FROM Room_history WHERE r_id = %d AND test_id = %d;", r_id, test_id);
        res = make_query(query);
        for(int i=0; i<number_of_history_records; i++){
            memset(&history_data, 0, sizeof(room_history));
            row = mysql_fetch_row(res);
            history_data.id = atoi(row[0]);
            history_data.r_id = atoi(row[1]);
            history_data.test_id = atoi(row[2]);
            history_data.user_id = atoi(row[3]);
            strcpy(history_data.score, row[4]);
            strcpy(history_data.submitted_time, row[5]);

            
            send(socket, &history_data, sizeof(room_history), 0);
            recv(socket, &oke_signal, sizeof(int), 0);
        }

        send(socket, &oke_signal, sizeof(int), 0);
    }
}

void Admin_showAllSubmittedRecord(int socket, char client_message[]){
    //opcode = 817
    char query[MAX_QUERY_LEN];
    MYSQL_RES *res;
    MYSQL_ROW row;
    int number_of_history_records;
    int oke_signal = 1;
    int r_id;
    room_history history_data;

    //receive r_id
    {
        send(socket, &oke_signal, sizeof(int), 0);
        recv(socket, &r_id, sizeof(int), 0);
    }
    
    //send number of history_records
    {
        sprintf(query, "SELECT COUNT(*) FROM Room_history WHERE r_id = %d;", r_id);
        res = make_query(query);
        row = mysql_fetch_row(res);
        number_of_history_records = atoi(row[0]);
        
        
        send(socket, &number_of_history_records, sizeof(int), 0);
        if(number_of_history_records == 0){
            return;
        }
        recv(socket, &oke_signal, sizeof(int), 0);
    }
    mysql_free_result(res);
    //send records
    {
        sprintf(query, "SELECT * FROM Room_history WHERE r_id = %d;", r_id);
        res = make_query(query);
        for(int i=0; i<number_of_history_records; i++){
            memset(&history_data, 0, sizeof(room_history));
            row = mysql_fetch_row(res);
            history_data.id = atoi(row[0]);
            history_data.r_id = atoi(row[1]);
            history_data.test_id = atoi(row[2]);
            history_data.user_id = atoi(row[3]);
            strcpy(history_data.score, row[4]);
            strcpy(history_data.submitted_time, row[5]);

            
            send(socket, &history_data, sizeof(room_history), 0);
            recv(socket, &oke_signal, sizeof(int), 0);
        }
        mysql_free_result(res);
        send(socket, &oke_signal, sizeof(int), 0);
    }
}



//Admin becomes user in room:
void User_showSubmittedRecordOfTest(int socket, char client_message[]){
    //opcode = 831
    char query[MAX_QUERY_LEN];
    MYSQL_RES *res;
    MYSQL_ROW row;
    int number_of_history_records;
    int number_of_tests_in_room;
    int oke_signal = 1;
    int r_id;
    int test_id;
    int user_id;
    char username[64];
    room_history history_data;
    test_in_room test_data;
    
    
    //receive r_id
    {
        send(socket, &oke_signal, sizeof(int), 0);
        recv(socket, &r_id, sizeof(int), 0);
    }

    //receive username
    {
        send(socket, &oke_signal, sizeof(int), 0);
        memset(username, 0, 64);
        recv(socket, username, 64, 0);
    }


    //get user_id of username
    {
        sprintf(query, "SELECT user_id FROM User_info WHERE username = '%s';", username);
        res = make_query(query);
        row = mysql_fetch_row(res);
        user_id = atoi(row[0]);
        mysql_free_result(res);
    }

    //send number of tests in room
    {
        sprintf(query, "SELECT COUNT(*) FROM Room_test WHERE r_id = %d;", r_id);
        res = make_query(query);
        row = mysql_fetch_row(res);
        number_of_tests_in_room = atoi(row[0]);
        mysql_free_result(res);

        send(socket, &number_of_tests_in_room, sizeof(int), 0);
        if(number_of_tests_in_room == 0){
            return;
        }
        recv(socket, &oke_signal, sizeof(int), 0);
    }

    //send list_test
    {
        sprintf(query, "SELECT r.test_id, t.test_name, t.num_of_question \
                        FROM Room_test as r, Test_private as t \
                        WHERE r.r_id = %d AND t.test_id = r.test_id;", r_id);
        res = make_query(query);
        for(int i=0; i<number_of_tests_in_room; i++){
            memset(&test_data, 0, sizeof(test_in_room));
            row = mysql_fetch_row(res);
            test_data.test_id = atoi(row[0]);
            strcpy(test_data.test_name, row[1]);
            test_data.num_of_question = atoi(row[2]);

            send(socket, &test_data, sizeof(test_in_room), 0);
            recv(socket, &oke_signal, sizeof(int), 0);
        }
        mysql_free_result(res);
    }

    //receive test_id
    {
        send(socket, &oke_signal, sizeof(int), 0);
        recv(socket, &test_id, sizeof(int), 0);
        if(test_id == 0){
            send(socket, &oke_signal, sizeof(int), 0);
            return;
        }
    }
    
    //send number of history_records
    {
        sprintf(query, "SELECT COUNT(*) FROM Room_history WHERE r_id = %d AND test_id = %d AND user_id = %d;", r_id, test_id, user_id);
        res = make_query(query);
        row = mysql_fetch_row(res);
        number_of_history_records = atoi(row[0]);
        mysql_free_result(res);
        
        send(socket, &number_of_history_records, sizeof(int), 0);

        if(number_of_history_records == 0){
            return;
        }

        recv(socket, &oke_signal, sizeof(int), 0);
    }

    //send records
    {
        sprintf(query, "SELECT * FROM Room_history WHERE r_id = %d AND test_id = %d and user_id = %d;", r_id, test_id, user_id);
        res = make_query(query);
        for(int i=0; i<number_of_history_records; i++){
            memset(&history_data, 0, sizeof(room_history));
            row = mysql_fetch_row(res);
            history_data.id = atoi(row[0]);
            history_data.r_id = atoi(row[1]);
            history_data.test_id = atoi(row[2]);
            history_data.user_id = atoi(row[3]);
            strcpy(history_data.score, row[4]);
            strcpy(history_data.submitted_time, row[5]);

            
            send(socket, &history_data, sizeof(room_history), 0);
            recv(socket, &oke_signal, sizeof(int), 0);
        }
        mysql_free_result(res);
        send(socket, &oke_signal, sizeof(int), 0);
    }
}

void User_showAllSubmittedRecord(int socket, char client_message[]){
    //opcode = 832
    char query[MAX_QUERY_LEN];
    MYSQL_RES *res;
    MYSQL_ROW row;
    int number_of_history_records;
    int oke_signal = 1;
    int r_id;
    int user_id;
    char username[64];
    room_history history_data;

    //receive r_id
    {
        send(socket, &oke_signal, sizeof(int), 0);
        recv(socket, &r_id, sizeof(int), 0);
    }
    
    //receive username
    {
        send(socket, &oke_signal, sizeof(int), 0);
        memset(username, 0, 64);
        recv(socket, username, 64, 0);
    }


    //get user_id of username
    {
        sprintf(query, "SELECT user_id FROM User_info WHERE username = '%s';", username);
        res = make_query(query);
        row = mysql_fetch_row(res);
        user_id = atoi(row[0]);
        mysql_free_result(res);
    }


    //send number of history_records
    {
        sprintf(query, "SELECT COUNT(*) FROM Room_history WHERE r_id = %d AND user_id = %d;", r_id, user_id);
        res = make_query(query);
        row = mysql_fetch_row(res);
        number_of_history_records = atoi(row[0]);
        mysql_free_result(res);
        
        send(socket, &number_of_history_records, sizeof(int), 0);
        if(number_of_history_records == 0){
            return;
        }
        recv(socket, &oke_signal, sizeof(int), 0);
    }

    //send records
    {
        sprintf(query, "SELECT * FROM Room_history WHERE r_id = %d AND user_id = %d;", r_id, user_id);
        res = make_query(query);
        for(int i=0; i<number_of_history_records; i++){
            memset(&history_data, 0, sizeof(room_history));
            row = mysql_fetch_row(res);
            history_data.id = atoi(row[0]);
            history_data.r_id = atoi(row[1]);
            history_data.test_id = atoi(row[2]);
            history_data.user_id = atoi(row[3]);
            strcpy(history_data.score, row[4]);
            strcpy(history_data.submitted_time, row[5]);

            printf("%-10d%-10d%-10d%-10d%-10s%-50s\n",history_data.id, history_data.r_id, history_data.test_id, history_data.user_id, history_data.score, history_data.submitted_time);

            
            send(socket, &history_data, sizeof(room_history), 0);
            recv(socket, &oke_signal, sizeof(int), 0);
        }
        mysql_free_result(res);
        send(socket, &oke_signal, sizeof(int), 0);
    }
}


int user_do_test_checking(int r_id, int test_id){
    //return:
    // 1 : thời gian lấy bài kieermt ra nằm trong khoảng từ open_time -> close_time
    // -1 nếu current_timestamp >= close_time
    // -2 nếu current_timestamp < open_time
    
    char query[MAX_QUERY_LEN];
    MYSQL_RES *res;
    MYSQL_ROW row;
    int flag;

    //check -1
    {
        sprintf(query, "SELECT CURRENT_TIMESTAMP() < (SELECT close_time FROM Room_test WHERE r_id = %d and test_id = %d);", r_id, test_id);
        res = make_query(query);
        row = mysql_fetch_row(res);
        flag = atoi(row[0]);
        mysql_free_result(res);
        if(flag != 1){
            return -1;
        }
    }

    //check -2
    {
        sprintf(query, "SELECT CURRENT_TIMESTAMP() >= (SELECT open_time FROM Room_test WHERE r_id = %d and test_id = %d);", r_id, test_id);
        res = make_query(query);
        row = mysql_fetch_row(res);
        flag = atoi(row[0]);
        mysql_free_result(res);
        if(flag != 1){
            return -2;
        }
    }

    return 1;
}

void user_do_test(int socket, char client_message[]){
    //opcode = 833
    char query[MAX_QUERY_LEN];
    MYSQL_RES *res;
    MYSQL_ROW row;
    int oke_signal = 1;
    int r_id;
    int num_of_test_in_room;
    int test_id;
    int num_of_question_in_test;

    //receive r_id
    {
        send(socket, &oke_signal, sizeof(int), 0);
        recv(socket, &r_id, sizeof(int), 0);
    }

    //get tests info in room
    {
        sprintf(query, "SELECT COUNT(*) FROM Room_test WHERE r_id = %d;", r_id);
        res = make_query(query);
        row = mysql_fetch_row(res);
        num_of_test_in_room = atoi(row[0]);
        mysql_free_result(res);

        send(socket, &num_of_test_in_room, sizeof(int), 0);

        //trường hợp ko có test nào trong room
        if(num_of_test_in_room == 0){
            return;
        }
        recv(socket, &oke_signal, sizeof(int), 0);

        //gửi thông tin test
        sprintf(query, "SELECT r.test_id, t.test_name, t.num_of_question, r.open_time, r.close_time, r.duration \
                        FROM Room_test as r, Test_private as t  \
                        WHERE r.r_id = %d AND r.test_id = t.test_id;", r_id);
        res = make_query(query);

        show_test_in_room test_info;

        for(int i=0; i<num_of_test_in_room; i++){
            row = mysql_fetch_row(res);
            memset(&test_info, 0, sizeof(show_test_in_room));
            test_info.test_id = atoi(row[0]);
            strcpy(test_info.test_name, row[1]);
            test_info.num_of_question = atoi(row[2]);
            strcpy(test_info.open_time, row[3]);
            strcpy(test_info.close_time, row[4]);
            test_info.duration = atoi(row[5]);

            send(socket, &test_info, sizeof(show_test_in_room), 0);
            recv(socket, &oke_signal, sizeof(int), 0);
        }

        mysql_free_result(res);
    }

    send(socket, &oke_signal, sizeof(int), 0);


    //xử lí flag
    while(1){
        recv(socket, &test_id, sizeof(int), 0);
        // int flag = user_do_test_checking(r_id, test_id);
        int flag = 1;
        send(socket, &flag, sizeof(int), 0);
        if(flag == 1){
            break;
        }
    }

    
    {   
        recv(socket, &test_id, sizeof(int), 0);
        show_test_in_room test_info;
        memset(&test_info, 0, sizeof(show_test_in_room));
        sprintf(query, "SELECT r.test_id, t.test_name, t.num_of_question, r.open_time, r.close_time, r.duration \
                        FROM Room_test as r, Test_private as t  \
                        WHERE r.r_id = %d AND r.test_id = %d AND r.test_id = t.test_id;", r_id, test_id);
        res = make_query(query);
        row = mysql_fetch_row(res);

        test_info.test_id = atoi(row[0]);
        strcpy(test_info.test_name, row[1]);
        test_info.num_of_question = atoi(row[2]);
        num_of_question_in_test = test_info.num_of_question;
        test_info.duration = atoi(row[5]);
        mysql_free_result(res);

        printf("Test information:\n");
        printf("\ttest_id: %d\n", test_info.test_id);
        printf("\ttest_name: %s\n", test_info.test_name);
        printf("\tnumber of questions: %d\n", test_info.num_of_question);
        printf("\tduration: %d\n", test_info.duration);

        
        send(socket, &test_info, sizeof(show_test_in_room), 0);
    }

    //ready flag
    // 0->end, 1->ready
    {
        int flag;
        recv(socket, &flag, sizeof(int), 0);
        
        if(flag == 0){
            send(socket, &oke_signal, sizeof(int), 0);
            return;
        }
    }

    //send question:
    int right_answer[num_of_question_in_test];
    {
        int random_index[num_of_question_in_test];
        for(int i = 0; i < num_of_question_in_test; i++){
            random_index[i] = i;
        }
        Shuffle(random_index, num_of_question_in_test);
        transfer_question question[num_of_question_in_test];
        sprintf(query, "SELECT question_content, opa, opb, opc, opd, ans \
                        FROM Question as q, r_Test_question as t, Answer as a \
                        WHERE t.test_id = %d \
                        AND t.question_id = q.question_id \
                        AND q.question_id = a.question_id;", test_id);
        res = make_query(query);
        
        printf("num_of_question_in_test = %d\n", num_of_question_in_test);
        for(int i=0; i<num_of_question_in_test; i++){
            row = mysql_fetch_row(res);
            //memset(&question, 0, sizeof(transfer_question));
            strcpy(question[random_index[i]].content, row[0]);
            strcpy(question[random_index[i]].opa, row[1]);
            strcpy(question[random_index[i]].opb, row[2]);
            strcpy(question[random_index[i]].opc, row[3]);
            strcpy(question[random_index[i]].opd, row[4]);
            right_answer[random_index[i]] = atoi(row[5]);
        }
        for(int i = 0; i < num_of_question_in_test; i++){
            send(socket, &question[i], sizeof(transfer_question), 0);
            recv(socket, &oke_signal, sizeof(int), 0);
        }
        mysql_free_result(res);
    }

    //evaluate(answer);
    char answer[num_of_question_in_test+5];
    send(socket, &oke_signal, sizeof(int), 0);
    recv(socket, answer, num_of_question_in_test+5, 0);

    int point = 0;
    for(int i=0; i<num_of_question_in_test; i++){
        if(answer[i] - 'A' + 1 == right_answer[i]){
            point++;
        }
    }

    char score[10];
    sprintf(score,"%d/%d", point, num_of_question_in_test);
    send(socket, score, 10, 0);


    //save to Room_history
    
    int user_id;
    recv(socket, &user_id, sizeof(int) , 0);
    send(socket, &oke_signal, sizeof(int), 0);

    sprintf(query, "INSERT INTO Room_history (r_id, test_id, user_id, score, submitted_time) \
                    VALUES (%d, %d, %d, '%s', CURRENT_TIMESTAMP());", r_id, test_id, user_id, score);
    make_query(query);
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