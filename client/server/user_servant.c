#include "../data.h"
#include "server.h"
#define MAX_QUERY_LEN 3000


//!####### USER FUNCTIONS #######
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
        else{
            printf("Error: Can't send exam list\n");
            return;
        }
    }
    printf("Send exam list successfully\n");
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
    while((row = mysql_fetch_row(res)) != NULL){
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

// int getDifferentTime(const char* date_str1, const char* date_str2) {
//     struct tm time1, time2;

//     // Parse date_str1
//     sscanf(date_str1, "%d-%d-%d %d:%d:%d", &time1.tm_year, &time1.tm_mon, &time1.tm_mday, &time1.tm_hour, &time1.tm_min, &time1.tm_sec);
//     time1.tm_year -= 1900; // Adjust for year starting from 1900
//     time1.tm_mon -= 1;     // Adjust for month starting from 0

//     // Parse date_str2
//     sscanf(date_str2, "%d-%d-%d %d:%d:%d",&time2.tm_year, &time2.tm_mon, &time2.tm_mday, &time2.tm_hour, &time2.tm_min, &time2.tm_sec);
//     time2.tm_year -= 1900;
//     time2.tm_mon -= 1;

//     // Convert struct tm to time_t
//     time_t time_seconds1 = mktime(&time1);
//     time_t time_seconds2 = mktime(&time2);

//     time_t diffInSeconds = difftime(time_seconds2, time_seconds1);

//     // Convert the time difference to minutes
//     int diffInMinutes = (int)(diffInSeconds / 60);

//     return diffInMinutes;
// }

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
    printf("Point: %d/%d\n", point, num_question);
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
    MYSQL_ROW row = mysql_fetch_row(res);
    int check = 1;
    if(row == NULL){
        check = 0;
        send(socket, &check, sizeof(int), 0);
        return;
    }
    send(socket, &check, sizeof(int), 0);
    register_data *user = malloc(sizeof(register_data));
    user->opcode = 102;
    strcpy(user->username, row[1]);
    strcpy(user->password, "********");
    strcpy(user->email, row[3]);
    user->previlege = atoi(row[4]);
    //Send user info to client
    char oke_signal[10];
    recv(socket, oke_signal, sizeof(oke_signal), 0);
    send(socket, user, sizeof(register_data), 0);
    
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
        sprintf(image_full_path, "/home/duy/NetworkProgramming/Project/main/main_reconstructx2/server/source/image/%s", (*question_list)[j].image_path);
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
        sprintf(audio_full_path, "/home/duy/NetworkProgramming/Project/main/main_reconstructx2/server/source/soundtrack/%s", (*question_list)[j].image_path);
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