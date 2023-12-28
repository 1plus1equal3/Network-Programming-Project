#include "../data.h"
#include "client.h"
#include "utils.h"
#include "show_image.c"

exam_data** getExamList(int client_sockfd);
void printExamInfo(exam_data* exam);
void send_answer(int client_sockfd, exam_data *exam);
void take_exam(int client_sockfd, exam_data* exam);
void seeExamHistrory(int client_socket);
void seeUserInfo(int client_sockfd);
void request_admin(int client_sockfd);
void showMainAppMenu(int client_sockfd);
void printExamList(exam_data** exams, int number_of_exam);
void showExamList(int client_sockfd);
void processAnswer(char *str);
void showAdvancedFeaturesMenu(int sockfd);


exam_data** getExamList(int client_sockfd){
    request_exam_list request;
    request.opcode = 203;
    request.number_of_exam = 10;
    request.user_id = data.user_id;
    send(client_sockfd, &request, sizeof(request), 0);
    int number_of_exam;
    recv(client_sockfd, &number_of_exam, sizeof(number_of_exam), 0);
    printf("Number of exam: %d\n", number_of_exam);
    exam_data** exams;
    exams = malloc(sizeof(exam_data*));
    *exams = malloc(sizeof(exam_data)*number_of_exam);
    for(int i = 0; i < number_of_exam; i++){
        recv(client_sockfd, (*(exams)+i), sizeof(exam_data), 0);
        send(client_sockfd, "OK", sizeof("OK"), 0);
    }
    return exams;
}

void printExamInfo(exam_data* exam){
    printf("Exam info:\n");
    printf("Exam id: %d\n", exam->exam_id);
    printf("Exam title: %s\n", exam->title);
    printf("Number of question: %d\n", exam->number_of_question);
}

void send_answer(int client_sockfd, exam_data *exam){
    char answer[exam->number_of_question+1];
    while(1){
            printf("Enter your answer: ");
            scanf(" %s", answer);
            processAnswer(answer); //Process answer
            int flag = 1;
            for(int i = 0; i < strlen(answer); i++){
                if(!( answer[i] >= 'A' && answer[i] <= 'Z')){
                    flag = 0;
                }
            }
            if(strlen(answer) == exam->number_of_question && flag == 1){
                break;
            }
            else{
                printf("Enter your answer: (must be A, B, C or D and number of answers equal to number of questions)\n");
            }
        }

        printf("Your answer: \n");
        for(int i = 0; i < exam->number_of_question; i++){
            printf("Question %d: %c\n", i+1, answer[i]);
        }
        //!TODO Check answer validation and send to server for evaluation
        char option;
        printf("Do you want to submit the answer? (y/n)\n");
        scanf(" %1c", &option);
        if(option == 'y'){
            submit_ans ans;
            ans.opcode = 205;
            ans.exam_id = exam->exam_id;
            ans.user_id = data.user_id;
            strcpy(ans.username, data.username);
            strcpy(ans.submit_ans, answer);
            ans.number_of_question = exam->number_of_question;
            send(client_sockfd, &ans, sizeof(submit_ans), 0); //Send the answer to server
        }else{
            send_answer(client_sockfd, exam);
        }
}

void take_exam(int client_sockfd, exam_data* exam){
    printExamInfo(exam);
    printf("Do you want to take this exam?(y/n): ");
    char option;
    scanf(" %1c", &option);
    __fpurge(stdin);
    if(option == 'n'){
        showExamList(client_sockfd);
        return;
    }
    else if(option == 'y'){
        printf("\n\n##########################################\n\n");
        printf("Exam: %s\n\n", (exam->title));
        //!TODO: User taking exam
        request_question_list request_question_list;
        request_question_list.opcode = 204;
        request_question_list.user_id = data.user_id;
        request_question_list.exam_id = exam->exam_id;
        request_question_list.number_of_question = exam->number_of_question;
        send(client_sockfd, &request_question_list, sizeof(request_question_list), 0);
        //Receive question list
        question_data **question_list;
        question_list = malloc(sizeof(question_data*));
        *question_list = malloc(sizeof(question_data)*(exam->number_of_question));
        for(int i=0; i<exam->number_of_question; i++){
            recv(client_sockfd, (*(question_list)+i), sizeof(question_data), MSG_WAITALL);
            printf("Question %d: %s\n", i+1, (*(question_list)+i)->content);
            for(int j=0; j<4; j++){
                printf("%c. %s\n", j+65, (*(question_list)+i)->op[j]);
            }
            send(client_sockfd, "OK", sizeof("OK"), 0);
        }
        send_answer(client_sockfd, exam);
        int result;
        recv(client_sockfd, &result, sizeof(result), 0);
        printf("\nYour score: %d/%d\n", result, exam->number_of_question);
        showExamList(client_sockfd);
    }
    else{
        printf("Invalid option!\n");
        take_exam(client_sockfd, exam);
        return;
    }
}

/*
void take_exam(int client_sockfd, exam_data* exam){
    char signal[8];
    recv(client_sockfd, &signal, sizeof(signal), 0);
    if(strcmp(signal, "NOT_OK") == 0){
        printf("Cannot take the exam in this time!");
    }
    else{
    printExamInfo(exam);
    printf("Do you want to take this exam?(y/n): ");
    char option;
    scanf(" %1c", &option);
    __fpurge(stdin);
    if(option == 'n'){
        showExamList(client_sockfd);
        return;
    }
    else if(option == 'y'){
        printf("\n\n##########################################\n\n");
        printf("Exam: %s\n\n", (exam->title));
        //!TODO: User taking exam
        request_question_list request_question_list;
        request_question_list.opcode = 204;
        request_question_list.exam_id = exam->exam_id;
        request_question_list.number_of_question = exam->number_of_question;
        send(client_sockfd, &request_question_list, sizeof(request_question_list), 0);
        //Receive question list
        question_data **question_list;
        question_list = malloc(sizeof(question_data*));
        *question_list = malloc(sizeof(question_data)*(exam->number_of_question));
        for(int i=0; i<exam->number_of_question; i++){
            recv(client_sockfd, (*(question_list)+i), sizeof(question_data), 0);
            send(client_sockfd, "OK", sizeof("OK"), 0);
            printf("Question %d: %s\n", i+1, (*(question_list)+i)->content);
            for(int j=0; j<4; j++){
                printf("%c. %s\n", j+65, (*(question_list)+i)->op[j]);
            }
        }
        //char answer[exam->number_of_question+1];
        //printf("Enter your answer: ");
        //scanf(" %s", answer);
        /*
        while(1){
            processAnswer(answer); //Process answer
            int flag = 1;
            for(int i = 0; i < strlen(answer); i++){
                if(!( answer[i] >= 'A' && answer[i] <= 'Z')){
                    flag = 0;
                }
            }
            if( strlen(answer) == exam->number_of_question && flag == 1){
                break;
            }
            else{
                printf("Enter your answer: (must be A, B, C or D and number of answers equal to number of questions)\n");
                scanf("%s", answer);
            }
        }

        printf("Your answer: \n");
        for(int i = 0; i < exam->number_of_question; i++){
            printf("Question %d: %c\n", i+1, answer[i]);
        }
        //!TODO Check answer validation and send to server for evaluation
        printf("Do you want to submit the answer? (y/n)\n");
        scanf("%s", &option);
        if(option == 'y'){
        submit_ans ans;
        ans.opcode = 205;
        ans.exam_id = exam->exam_id;
        strcpy(ans.username, data.username);
        strcpy(ans.submit_ans, answer);
        ans.number_of_question = exam->number_of_question;
        
        send(client_sockfd, &ans, sizeof(submit_ans), 0); //Send the answer to server
        }
        else{

        }
        send_answer(client_sockfd, exam);
        int result;
        recv(client_sockfd, signal, sizeof(signal), 0);
        if(strcmp(signal, "NOT_OK") == 0){
            printf("You cannot submit at this time !\n");
        }
        else{
        recv(client_sockfd, signal, sizeof(signal), 0);
        if(strcmp(signal, "NOT_OK") == 0){
            printf("Cheating detected !!!\n");
        }
        else{
        recv(client_sockfd, &result, sizeof(result), 0);
        printf("\nYour score: %d/%d\n", result, exam->number_of_question);
        showExamList(client_sockfd);
        }
        }
    
    }
    else{
        printf("Invalid option!\n");
        take_exam(client_sockfd, exam);
        return;
    }
}
}

*/

void seeExamHistrory(int client_socket){
    request_user_history request;
    request.opcode = 207;
    request.user_id = data.user_id;
    strcpy(request.username, data.username);
    send(client_socket, &request, sizeof(request), 0);
    int number_of_history_records;
    char oke_signal[OKE_SIGNAL_LEN] = "OK";
    recv(client_socket, &number_of_history_records, sizeof(int), 0);
    send(client_socket, oke_signal, sizeof(oke_signal), 0);
    exam_result **history_list = malloc(sizeof(exam_result*));
    *history_list = malloc(sizeof(exam_result)*number_of_history_records);
    for(int i=0; i<number_of_history_records; i++){
        recv(client_socket, (*history_list)+i, sizeof(exam_result), 0);
        send(client_socket, oke_signal, sizeof(oke_signal), 0);
    }

    // recv(client_socket, oke_signal, OKE_SIGNAL_LEN, 0);
    // if(strcmp(oke_signal, "OKE") != 0){
    //    RAISE ERROR
    // }

    printf("EXAM HISTORY OF %s\n\n", data.username);
    for(int i=0; i<number_of_history_records; i++){
        printf("%d. ", i+1);
        printf("Test: %s\tDate: %s\n", (*history_list)[i].title, (*history_list)[i].date);
        printf("Score: %s\n\n", (*history_list)[i].score);
    }
    //free(history_list);
    showMainAppMenu(client_socket);
}

void seeUserInfo(int client_sockfd){
    request_user_info info;
    info.opcode = 103;
    info.user_id = data.user_id;
    strcpy(info.username, data.username);
    send(client_sockfd, &info, sizeof(info), 0);
    int check;
    recv(client_sockfd, &check, sizeof(int), 0);
    send(client_sockfd, "OK", sizeof("OK"), 0);
    register_data user_info;
    recv(client_sockfd, &user_info, sizeof(user_info), 0);
    //Print user info
    printUserInfo(user_info);
    showMainAppMenu(client_sockfd);
}

void request_admin(int client_sockfd){
    printf("Request admin previlege!\n");
    request_admin_privilege request;
    request.opcode = 301;
    request.user_id = data.user_id;
    strcpy(request.username, data.username);
    send(client_sockfd, &request, sizeof(request), 0);
    char result[16];
    recv(client_sockfd, result, sizeof(result), 0);
    if(strcmp(result, "OK") == 0){
        printf("Your request has been sent!\nPlease wait for admin to approve!\n");
    }
    else if(strcmp(result, "DUPLICATED") == 0){
        printf("You have sent the request before!\nPlease wait for approval!\n");
    }
    else{
        printf("Request failed!\n");
    }
    showMainAppMenu(client_sockfd);
}


//##### Room operation #####

//1-user
void user_do_test(int client_sockfd, int r_id){
    int opcode = 833;
    int oke_signal = 1;
    int num_of_test_in_room;
    int test_id;

    
    send(client_sockfd, &opcode, sizeof(int), 0);

    recv(client_sockfd, &oke_signal, sizeof(int), 0);
    send(client_sockfd, &r_id, sizeof(int), 0);



    //một hàm để show test from bank
    {
        recv(client_sockfd, &num_of_test_in_room, sizeof(int), 0);

        //trường hợp ko có test nào trong room
        if(num_of_test_in_room == 0){
            printf("There is no available test in this room!\n");
            return;
        }
        send(client_sockfd, &oke_signal, sizeof(int), 0);
    }

    show_test_in_room test_info;

    int list_test_id[num_of_test_in_room];

    printf("---List of tests in room---\n");
    printf("%-10s%-30s%-10s%-30s%-30s%-20s\n", "test_id", "test_name", "num_of_question", "open_time", "close_time", "duration(minutes)");
    for(int i=0; i<num_of_test_in_room; i++){
        memset(&test_info, 0, sizeof(show_test_in_room));
        recv(client_sockfd, &test_info, sizeof(show_test_in_room), 0);
        send(client_sockfd, &oke_signal, sizeof(int), 0);
        printf("%-10d%-30s%-10d%-30s%-30s%-20d\n", test_info.test_id, test_info.test_name, test_info.num_of_question, test_info.open_time, test_info.close_time, test_info.duration);
        list_test_id[i] = test_info.test_id;
    }

    recv(client_sockfd, &oke_signal, sizeof(int), 0);

    {

        int chosen_test_id;
        while(1){
            printf("Enter the id of the test you want to do: ");
            scanf("%d", &chosen_test_id);
            __fpurge(stdin);
            int valid = 0;
            for(int i=0; i<num_of_test_in_room; i++){
                if(chosen_test_id == list_test_id[i]){
                    valid = 1;
                    break;
                }
            }
            if(valid == 0){
                printf("Invalid test_id!\n");
            }
            else{
                int flag;
                // 1 : thời gian lấy bài kiểm ra nằm trong khoảng từ open_time -> close_time
                // -1 nếu current_timestamp >= close_time
                // -2 nếu current_timestamp < open_time
                send(client_sockfd, &chosen_test_id, sizeof(int), 0);
                recv(client_sockfd, &flag, sizeof(int), 0);
                if(flag == 1){
                    test_id = chosen_test_id;
                    break;
                }
                else if(flag == -1){
                    printf("It's past time to take this test!n");
                }
                else if(flag == -2){
                    printf("It's not time to take this test yet");
                }
                else{
                    printf("Something went wrong at user_do_test\n");
                }
            }

        }
    }

    //send the info of test for user
    {   
        memset(&test_info, 0, sizeof(show_test_in_room));
        send(client_sockfd, &test_id, sizeof(int), 0);
        recv(client_sockfd, &test_info, sizeof(show_test_in_room), 0);
    }
    printf("Test information:\n");
    printf("\ttest_id: %d\n", test_info.test_id);
    printf("\ttest_name: %s\n", test_info.test_name);
    printf("\tnumber of questions: %d\n", test_info.num_of_question);
    // printf("\tduration: %d\n", test_info.duration);

    while(1){
        int ready;
        printf("Ready to do the test? (0 to get back, 1 for ready): ");
        scanf("%d", &ready);
        __fpurge(stdin);
        if(ready != 0 && ready != 1){
            printf("Invalid option!\n");
        }
        else{
            send(client_sockfd, &ready, sizeof(int), 0);
            if(ready == 0){
                recv(client_sockfd, &oke_signal, sizeof(int), 0);
                printf("You have cancelled operation!\n");
                return;
            }
            else{
                break;
            }
        }
    }

    printf("\n#################\n");
    printf("Test_content:\n\n");
    //receive question
    transfer_question question;
    for(int i=0; i<test_info.num_of_question; i++){
        memset(&question, 0, sizeof(transfer_question));
        recv(client_sockfd, &question, sizeof(transfer_question), 0);
        send(client_sockfd, &oke_signal, sizeof(int), 0);
        printf("Question %d: %s\n", i+1, question.content);
        printf("A. %s\n", question.opa);
        printf("B. %s\n", question.opb);
        printf("C. %s\n", question.opc);
        printf("D. %s\n", question.opd);
        printf("\n");
    }
    
    char answer[test_info.num_of_question + 50];
    while(1){
        printf("Answer (only space and ABCD allowed): ");
        fgets(answer, test_info.num_of_question + 10, stdin);
        __fpurge(stdin);
        answer[strlen(answer) - 1] = '\0';
        
        int valid_char = 1;
        for(int i=0; i<strlen(answer); i++){
            if(answer[i] == ' ' || (answer[i] >= 'A' && answer[i] <= 'D') || (answer[i] >= 'a' && answer[i] <= 'd')){
                
            }
            else{
                valid_char = 0;
                break;
            }
        }
        if(valid_char == 0){
            printf("Invalid char in answer!\n");
        }
        else{
            processAnswer(answer);
            if(strlen(answer) != test_info.num_of_question){
                printf("The number of answers is not the same as the number of question!\n");
            }
            else{
                break;
            }
        }
    }

    
    recv(client_sockfd, &oke_signal, sizeof(int), 0);
    send(client_sockfd, answer, strlen(answer) + 1, 0);

    char score[10];
    memset(score, 0, 10);
    recv(client_sockfd, score, 10, 0);

    printf(">>>> Your score: %s\n\n", score);

    send(client_sockfd, &(data.user_id), sizeof(int), 0);
    recv(client_sockfd, &oke_signal, sizeof(int), 0);
}

//2-user
void User_showSubmittedRecordOfTest_2(int client_sockfd, int r_id){
    //opcode = 831
    int opcode = 831;
    send(client_sockfd, &opcode, sizeof(int), 0);

    room_history history_data;
    int number_of_history_records;
    int number_of_tests_in_room;
    int test_id;

    int oke_signal = 1;
    test_in_room test_data;

    //send r_id
    {
        recv(client_sockfd, &oke_signal, sizeof(int), 0);
        send(client_sockfd, &r_id, sizeof(int), 0);
    }

    //send user_name
    {
        recv(client_sockfd, &oke_signal, sizeof(int), 0);
        send(client_sockfd, data.username, strlen(data.username)+1, 0);
    }

    //receive number of tests in 
    {
        recv(client_sockfd, &number_of_tests_in_room, sizeof(int), 0);
        if(number_of_tests_in_room == 0){
            printf("There is no test in this room\n");
            return;
        }
        send(client_sockfd, &oke_signal, sizeof(int), 0);
    }

    int list_test_id[number_of_tests_in_room];

    //receive list_test
    {   
        printf("---List of test in room with id = %d---\n", r_id);
        printf("%-10s%-50s%-10s\n", "test_id", "test_name", "number_of_questions");
        for(int i=0; i<number_of_tests_in_room; i++){
            memset(&test_data, 0, sizeof(test_in_room));
            recv(client_sockfd, &test_data, sizeof(test_in_room), 0);
            send(client_sockfd, &oke_signal, sizeof(int), 0);
            printf("%-10d%-50s%-10d\n", test_data.test_id, test_data.test_name, test_data.num_of_question);
            list_test_id[i] = test_data.test_id;
        }
    }

    //input  test_id
    while(1){
        printf("Enter the id of the test you want to see records (enter 0 to cancel): ");
        scanf("%d", &test_id);
        __fpurge(stdin);

        if(test_id == 0){
            break;
        }
        
        int valid = 0;
        for(int i=0; i<number_of_tests_in_room; i++){
            if(test_id == list_test_id[i]){
                valid = 1;
                break;
            }
        }
        if(valid == 1){
            break;
        }
        else{
            printf("Invalid test_id, please try again\n");
        }
    }

    

    //send test_id
    {   
        recv(client_sockfd, &oke_signal, sizeof(int), 0);
        send(client_sockfd, &test_id, sizeof(int), 0);
        if(test_id == 0){
            recv(client_sockfd, &oke_signal, sizeof(int), 0);
            printf("You have cancelled the operation\n");
            return;
        }
    }

    //receive number_of_history_records
    {
        recv(client_sockfd, &number_of_history_records, sizeof(int), 0);
        if(number_of_history_records == 0){
            printf("You have no history record in this test\n");
            return;
        }
        send(client_sockfd, &oke_signal, sizeof(int), 0);

    }

    //show records -- HERE
    {   
        printf("---Your history record of test with test_id = %d in this room---\n", test_id);
        printf("%-10s%-10s%-10s%-10s%-10s%-50s\n", "id", "r_id", "test_id", "user_id", "score", "submitted_time");
        for(int i=0; i<number_of_history_records; i++){
            memset(&history_data, 0, sizeof(room_history));
            recv(client_sockfd, &history_data, sizeof(room_history), 0);
            send(client_sockfd, &oke_signal, sizeof(int), 0);
            printf("%-10d%-10d%-10d%-10d%-10s%-50s\n",history_data.id, history_data.r_id, history_data.test_id, history_data.user_id, history_data.score, history_data.submitted_time);
        }
        recv(client_sockfd, &oke_signal, sizeof(int), 0);       
    }

}

//3-user
void User_showAllSubmittedRecord_2(int client_sockfd, int r_id){
    //opcode = 832
    int opcode = 832;
    send(client_sockfd, &opcode, sizeof(int), 0);

    room_history history_data;
    int number_of_history_records;
    int oke_signal = 1;

    //send r_id
    {
        recv(client_sockfd, &oke_signal, sizeof(int), 0);
        send(client_sockfd, &r_id, sizeof(int), 0);
    }

    //send user_name
    {
        recv(client_sockfd, &oke_signal, sizeof(int), 0);
        send(client_sockfd, data.username, strlen(data.username)+1, 0);
    }

    //receive number_of_history_records
    {
        recv(client_sockfd, &number_of_history_records, sizeof(int), 0);
        if(number_of_history_records == 0){
            printf("There is no history record in this room\n");
            return;
        }
        send(client_sockfd, &oke_signal, sizeof(int), 0);
        
    }

    

    //show records -- HERE
    {   
        printf("---Your history record in room with id = %d---\n", r_id);
        printf("%-10s%-10s%-10s%-10s%-10s%-50s\n", "id", "r_id", "test_id", "user_id", "score", "submitted_time");

        for(int i=0; i<number_of_history_records; i++){
            memset(&history_data, 0, sizeof(room_history));
            recv(client_sockfd, &history_data, sizeof(room_history), 0);
            send(client_sockfd, &oke_signal, sizeof(int), 0);
            printf("%-10d%-10d%-10d%-10d%-10s%-50s\n",history_data.id, history_data.r_id, history_data.test_id, history_data.user_id, history_data.score, history_data.submitted_time);
        }
        recv(client_sockfd, &oke_signal, sizeof(int), 0);       
    }
}


void enterRoom(int client_sockfd, int r_id){
    while(1){
        int opt;
        printf("1. Do a test\n");
        printf("2. See the result of an exam in room \n");
        printf("3. See result of all exams\n");
        printf("4. Back to the main menu\n");
        printf("Choose the option: ");
        scanf("%d", &opt);
        __fpurge(stdin);
        switch (opt)
        {
        case 1:
            user_do_test(client_sockfd, r_id);
            break;
        case 2:
            User_showSubmittedRecordOfTest_2(client_sockfd, r_id);
            break;
        case 3:
            User_showAllSubmittedRecord_2(client_sockfd, r_id);
            break;
        case 4:
            showMainAppMenu(client_sockfd);
            return;
        default:
            printf("Invalid option\n");
            break;
        }
    }
}


void showUserRoom(int client_sockfd){
    // printf("You are in show all your room\n");
    int opcode = 804;
    int num_of_room; // Number of room
    show_your_room your_info;
    room_info room; 
    your_info.opcode = opcode;
    strcpy(your_info.username, data.username);
    send(client_sockfd, &your_info, sizeof(show_your_room), 0); //Send request to get room info to server
    // printf("Client send oke?\n");
    recv(client_sockfd, &num_of_room, sizeof(int), 0); // Receive number of room from server

    // printf("after received\n");

    if(num_of_room <= 0){
        printf("There is no room!\n\n");
        showMainAppMenu(client_sockfd);
        return;
    }

    char oke_signal[OKE_SIGNAL_LEN] = "OK";
    send(client_sockfd, oke_signal, OKE_SIGNAL_LEN, 0);
    
    int r_id_list[num_of_room];
    char r_admin_name_list[num_of_room][256];
    printf("%-15s%-30s%-30s%-20s\n", "room_id", "room_name", "create_account", "create_date");
        
    for(int i=0; i<num_of_room; i++){
        memset(&room, 0, sizeof(room_info));
        recv(client_sockfd, &room, sizeof(room_info), 0);
        r_id_list[i] = room.r_id;
        strcpy(r_admin_name_list[i], room.admin_name);
        printf("%-15d%-30s%-30s%-20s\n", room.r_id, room.r_name, room.admin_name, room.create_date);
        send(client_sockfd, oke_signal, OKE_SIGNAL_LEN, 0);
    }

    recv(client_sockfd, oke_signal, OKE_SIGNAL_LEN, 0);

    int go_r_id;
    int valid;
    while(1){
        valid = 0; //valid = 1 -> you are admin in that room 
                        //valid = 2 -> you are user in that room
        printf("Enter the id of the room you want to go in (enter 0 to get back): ");
        scanf("%d", &go_r_id);
        __fpurge(stdin);
        if(go_r_id == 0){
            return;
        }
        for(int i=0; i<num_of_room; i++){
            if(go_r_id == r_id_list[i]){
                valid = 1;
            }
        }
        if(valid != 0){
            break;
        }
        printf("Invalid Option!\n");
    }
    
    enterRoom(client_sockfd, go_r_id);
    showMainAppMenu(client_sockfd);
}

//##### End of room operation #####

//##### Advanced features #####
//!TODO: Implement exam with images
void takeImageEmbeddedExam(int sockfd){
    image_exam request;
    request.opcode = 1001;
    request.user_id = data.user_id;
    printf("Enter number of questions: ");
    scanf(" %d", &request.number_of_question);
    __fpurge(stdin);
    //Send request to server
    send(sockfd, &request, sizeof(request), 0);
    int number_of_question_available;
    recv(sockfd, &number_of_question_available, sizeof(number_of_question_available), 0);
    for(int i = 0; i < number_of_question_available; i++){
        send(sockfd, "OK", sizeof("OK"), 0);
        char image_name[255];
        recv(sockfd, image_name, sizeof(image_name), 0);
        if(strcmp(image_name, "ERROR") == 0){
            printf("Cannot get image name!\n");
            continue;
        }
        char image_full_path[1000];
        sprintf(image_full_path, "/home/duy/NetworkProgramming/Project/main/main_reconstructx2/client/source/image/%s", image_name);
        printf("Image path: %s\n", image_full_path);
        //Open image file for writing
        FILE *image_file = fopen(image_full_path, "wb");
        if(image_file == NULL){
            printf("Cannot open image file!\n");
            send(sockfd, "ERROR", sizeof("ERROR"), 0);
            return;
        }
            send(sockfd, "OK", sizeof("OK"), 0);
        while(1){
            char buffer[2048];
            int read_size = recv(sockfd, buffer, sizeof(buffer), 0);
            if(strcmp(buffer, "END") == 0 || read_size <= 0){
                break;
            }
            fwrite(buffer, read_size, 1, image_file);
            send(sockfd, "OK", sizeof("OK"), 0);
            memset(buffer, 0, sizeof(buffer));
        }
    //Close image file
    if(image_file != NULL) fclose(image_file);
    }
    //Get question list
    recv(sockfd, &number_of_question_available, sizeof(number_of_question_available), 0);
    question_image **question_list = malloc(sizeof(question_image*));
    *question_list = malloc(sizeof(question_image)*number_of_question_available);
    for(int i = 0; i < number_of_question_available; i++){
        send(sockfd, "OK", sizeof("OK"), 0);
        recv(sockfd, (*question_list)+i, sizeof(question_image), 0);
    }
    //Print question list
    for(int i=0; i<number_of_question_available; i++){
    printf("Question %d: %s\n", i+1, (*(question_list)+i)->question_content);
        for(int j=0; j<4; j++){
            if(strlen((*(question_list)+i)->op[j]) > 0)
                printf("%c. %s\n", j+65, (*(question_list)+i)->op[j]);
        }
    }
    //Open image viewer
    getInput(question_list, number_of_question_available, 0);
    startImageQuestionInterface();
    //Show option
    int opt;
    do{
    //Print question list
    for(int i=0; i<number_of_question_available; i++){
    printf("Question %d: %s\n", i+1, (*(question_list)+i)->question_content);
        for(int j=0; j<4; j++){
            if(strlen((*(question_list)+i)->op[j]) > 0)
                printf("%c. %s\n", j+65, (*(question_list)+i)->op[j]);
        }
    }
    printf("\n 1. Reopen image viewer\n");
    printf(" 2. Submit answer\n");
    printf("Please choose your option: ");
    scanf(" %d", &opt);
    __fpurge(stdin);
    char answer[number_of_question_available+1];
    switch(opt){
        case 1:
            if(viewerOpen == 0){
                getInput(question_list, number_of_question_available, 0);
                startImageQuestionInterface();
                printf("Image viewer has been reopened!\n");
            } 
            break;
        case 2:
            printf("Enter your answer: ");
            scanf(" %s", answer);
            processAnswer(answer);
            printf("Your answer: %s\n", answer);
            printf("Do you want to submit the answer? (y/n)\n");
            char option;
            scanf(" %1c", &option);
            if(option == 'y'){
                send(sockfd, answer, sizeof(answer), 0); //Send the answer to server
                //Get result
                int result;
                recv(sockfd, &result, sizeof(result), 0);
                printf("\nYour score: %d/%d\n", result, number_of_question_available);
                //destroyWindow();
                //Delete image files
                deleteImageAfterUsed();
                showAdvancedFeaturesMenu(sockfd);
                return;
            }else{
                printf("Your answer has not been submitted!\n");
                break;
            }
            break;
        default:
            printf("Invalid option!\n");
            break;
        }
    }while(1);
}

//!TODO: Implement exam with sounds
void takeSoundEmbeddedExam(int sockfd){
    sound_exam request;
    request.opcode = 1002;
    request.user_id = data.user_id;
    printf("Enter number of questions: ");
    scanf(" %d", &request.number_of_question);
    __fpurge(stdin);
    //Send request to server
    send(sockfd, &request, sizeof(request), 0);
    int number_of_question_available;
    recv(sockfd, &number_of_question_available, sizeof(number_of_question_available), 0);
    for(int i = 0; i < number_of_question_available; i++){
        send(sockfd, "OK", sizeof("OK"), 0);
        char audio_name[255];
        recv(sockfd, audio_name, sizeof(audio_name), 0);
        if(strcmp(audio_name, "ERROR") == 0){
            printf("Cannot get audio file name!\n");
            continue;
        }
        char audio_full_path[1000];
        sprintf(audio_full_path, "/home/duy/NetworkProgramming/Project/main/main_reconstructx2/client/source/soundtrack/%s", audio_name);
        printf("Audio path: %s\n", audio_full_path);
        //Open audio file for writing
        FILE *audio_file = fopen(audio_full_path, "wb");
        if(audio_file == NULL){
            printf("Cannot open audio file!\n");
            send(sockfd, "ERROR", sizeof("ERROR"), 0);
            return;
        }
            send(sockfd, "OK", sizeof("OK"), 0);
        while(1){
            char buffer[2048];
            int read_size = recv(sockfd, buffer, sizeof(buffer), 0);
            if(strcmp(buffer, "END") == 0 || read_size <= 0){
                break;
            }
            fwrite(buffer, read_size, 1, audio_file);
            send(sockfd, "OK", sizeof("OK"), 0);
            memset(buffer, 0, sizeof(buffer));
        }
    //Close image file
    if(audio_file != NULL) fclose(audio_file);
    }
    //Get question list
    recv(sockfd, &number_of_question_available, sizeof(number_of_question_available), 0);
    question_image **question_list = malloc(sizeof(question_image*));
    *question_list = malloc(sizeof(question_image)*number_of_question_available);
    for(int i = 0; i < number_of_question_available; i++){
        send(sockfd, "OK", sizeof("OK"), 0);
        recv(sockfd, (*question_list)+i, sizeof(question_image), 0);
    }
    //Print question list
    for(int i=0; i<number_of_question_available; i++){
    printf("Question %d: %s\n", i+1, (*(question_list)+i)->question_content);
        for(int j=0; j<4; j++){
            if(strlen((*(question_list)+i)->op[j]) > 0)
                printf("%c. %s\n", j+65, (*(question_list)+i)->op[j]);
        }
    }
    //Open audio listener
    getInput(question_list, number_of_question_available, 1);
    startSoundQuestionInterface();
    //Show option
    int opt;
    do{
    //Print question list
    for(int i=0; i<number_of_question_available; i++){
    printf("Question %d: %s\n", i+1, (*(question_list)+i)->question_content);
        for(int j=0; j<4; j++){
            if(strlen((*(question_list)+i)->op[j]) > 0)
                printf("%c. %s\n", j+65, (*(question_list)+i)->op[j]);
        }
    }
    printf("\n 1. Reopen audio listener\n");
    printf(" 2. Submit answer\n");
    printf("Please choose your option: ");
    scanf(" %d", &opt);
    __fpurge(stdin);
    char answer[number_of_question_available+1];
    switch(opt){
        case 1:
            if(viewerOpen == 0){
                getInput(question_list, number_of_question_available, 1);
                startSoundQuestionInterface();
                printf("Image viewer has been reopened!\n");
            } 
            break;
        case 2:
            printf("Enter your answer: ");
            scanf(" %s", answer);
            processAnswer(answer);
            printf("Your answer: %s\n", answer);
            printf("Do you want to submit the answer? (y/n)\n");
            char option;
            scanf(" %1c", &option);
            if(option == 'y'){
                send(sockfd, answer, sizeof(answer), 0); //Send the answer to server
                //Get result
                int result;
                recv(sockfd, &result, sizeof(result), 0);
                printf("\nYour score: %d/%d\n", result, number_of_question_available);
                //destroyWindow();
                //Delete audio file
                deleteSoundAfterUsed();
                showAdvancedFeaturesMenu(sockfd);
                return;
            }else{
                printf("Your answer has not been submitted!\n");
                break;
            }
            break;
        default:
            printf("Invalid option!\n");
            break;
        }
    }while(1);
}

//!TODO: Implement see study statistics
void seeStudyStatistic(int sockfd){
    
}

//##### End of advanced features #####


void showAdvancedFeaturesMenu(int sockfd){
    printf("\nAdvanced features!\n");
    printf("1. Exam with images\n");
    printf("2. Exam with sounds\n");
    printf("3. See study statistics\n");
    printf("4. Back to main menu\n");
    printf("Please choose your option: ");
    int option;
    scanf(" %1d", &option);
    __fpurge(stdin);
    switch(option){
        case 1:
            takeImageEmbeddedExam(sockfd);
            return;
        case 2:
            takeSoundEmbeddedExam(sockfd);
            return;
        case 3:
            seeStudyStatistic(sockfd);
            return;
        case 4:
            showMainAppMenu(sockfd);
            return;
        default:
            printf("Invalid option!\n");
            break;
    }
    showAdvancedFeaturesMenu(sockfd);
}

void showMainAppMenu(int client_sockfd){
    printf("Main application system!\n");
    printf("1. Take an exam\n");
    printf("2. See examination history\n");
    printf("3. See user information\n");
    printf("4. Request admin previlege\n");
    printf("5. Show private room\n");
    printf("6. Advanced features\n");
    printf("7. Log out\n");
    printf("Please choose your option: ");
    int option;
    scanf(" %1d", &option);
    __fpurge(stdin);
        switch(option){
        case 1:
            showExamList(client_sockfd);
            return;
        case 2:
            seeExamHistrory(client_sockfd);
            return;
        case 3:
            seeUserInfo(client_sockfd);
            return;
        case 4:
            request_admin(client_sockfd);
            return;
        case 5:
            showUserRoom(client_sockfd);
            return;
        case 6:
            showAdvancedFeaturesMenu(client_sockfd);
            return;
        case 7:
            printf("Log out successfully!\n");
            showLoginMenu(client_sockfd);
            return;
        default:
            printf("Invalid option!\n");
            break;
        }
    showMainAppMenu(client_sockfd);
}
void printExamList(exam_data** exams, int number_of_exam){
    printf("\nExam list:\n");
    for(int i = 0; i < number_of_exam; i++){
        printf("%d. %s\t%d questions\n", i+1, (*(exams)+i)->title, (*(exams)+i)->number_of_question);
    }
}

void showExamList(int client_sockfd){
    exam_data** exams = getExamList(client_sockfd);
    int number_of_exam = 10;
    printExamList(exams, number_of_exam);
    printf("\n##############################\n");
    printf("Please choose your option(0 for back to main menu): ");
    int option;
    scanf(" %d", &option);
    __fpurge(stdin);
    if(option == 0){
        showMainAppMenu(client_sockfd);
        return;
    }else if(option < 0 || option > number_of_exam){
        printf("Invalid option!\n");
        showExamList(client_sockfd);
        return;
    }
    take_exam(client_sockfd, (*(exams)+option-1));
}

void processAnswer(char *str)
{
    // To keep track of non-space character count
    int count = 0;
    // Traverse the provided string. If the current character is not a space,
    //move it to index 'count++'.
    for (int i = 0; str[i]; i++)
        if (str[i] != ' ')
            str[count++] = str[i]; // here count is incremented
    str[count] = '\0';
    for(int i = 0; i < strlen(str); i++){
        if( str[i] >= 'a' && str[i] <= 'z'){
            str[i] = str[i] + 'A' - 'a';
        }
    }
}
