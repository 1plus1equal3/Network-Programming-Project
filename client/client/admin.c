/*
1. Quan ly cau hoi
    1.1. Tim kiem cau hoi
    1.2. Them cau hoi
    1.3. Sua cau hoi
    1.4. Xoa cau hoi
2. Quan ly bai ktra
    2.1. Tim kiem bai ktra
    2.2. Them bai ktra
    2.3. Sua bai ktra
    2.4. Xoa bai ktra
3. Quan ly phong thi
    3.1. Tao phong thi(Ma Phong Thi(Tao tu dong), Ten, Mon(subject), Topic, So luong thi sinh toi da, Thoi gian bat dau, Thoi gian ket thuc)
    3.2. Thêm user vào phòng thi (user cũng có thể là admin)
    3.3. Quan ly user trong phòng thi (Xem danh sách user, add user, xóa user)
    
4. Xem KQ dua tren phong thi hoac bai ktra
5. Xem thong tin User dua tren User_id
6. Cap quyen cho User thanh Admin
*/

#include "../data.h"
#include "client.h"
#include "utils.h"

void showMainAppMenuAdmin(int client_sockfd);
void manageQuestion(int sockfd);
void manageExam(int sockfd);
void manageExamRoom(int sockfd);
void getExamRoomResult(int sockfd);
void getUserInfo(int sockfd);
void approveAdminRequest(int sockfd);
void searchQuestion(int sockfd);
void searchQuestionById(int sockfd);
void searchQuestionByContent(int sockfd);
void advanceQuestionSearch(int sockfd);
void addQuestion(int sockfd);
void deleteQuestion(int sockfd);
void showQuestionManagementInterface();
void showExamManagementInterface();
void searchExam(int sockfd);
void createRandomExam(int sockfd);
void createExamByQuestion(int sockfd);
void createExam(int sockfd);
void createRoom(int client_sockfd);
void deleteRoom(int client_sockfd);
void showYourRoom(int client);


void showMainAppMenuAdmin(int client_sockfd){
    printf("\nMain application system!\n");
    printf("1. Manage question database(Search, Add, Edit, Delete)\n");
    printf("2. Manage exam database(Search, Add, Edit, Delete)\n");
    printf("3. Manage exam room\n");
    printf("4. See user information\n");
    printf("5. Approve admin request\n");
    printf("6. Log out\n");
    printf("Please choose your option: ");
    int option;
    scanf(" %1d", &option);
    __fpurge(stdin);
        switch(option){
        case 1:
            manageQuestion(client_sockfd);
            return;
        case 2:
            manageExam(client_sockfd);
            return;
        case 3:
            manageExamRoom(client_sockfd);
            return;
        case 4:
            getUserInfo(client_sockfd);
            return;
        case 5:
            approveAdminRequest(client_sockfd);
            return;
        case 6:
            printf("Log out successfully!\n");
            showLoginMenu(client_sockfd);
            return;
        default:
            printf("Invalid option!\n");
            break;
        }
    showMainAppMenuAdmin(client_sockfd);
}

void showQuestionManagementInterface(){
    printf("\nQuestion management interface!\n");
    printf("1. Search question\n");
    printf("2. Add question\n");
    printf("3. Delete question\n");
    printf("4. Back\n");
    printf("Please choose your option: ");
}

//!TODO: Manage question
void searchQuestion(int sockfd);
// void addQuestion(int sockfd);
// void editQuestion(int sockfd);
// void deleteQuestion(int sockfd);
void manageQuestion(int sockfd){
    showQuestionManagementInterface();
    int option;
    scanf(" %1d", &option);
    __fpurge(stdin);
    switch(option){
        case 1:
            searchQuestion(sockfd);
            return;
        case 2:
            addQuestion(sockfd);
            return;
        case 3:
            deleteQuestion(sockfd);
            return;
        case 4:
            showMainAppMenuAdmin(sockfd);
            return;
        default:
            printf("Invalid option!\n");
            break;
    }
    manageQuestion(sockfd);
}

//############## SEARCH QUESTION #################

void searchQuestionById(int sockfd){
    printf("\nSearch question by id!\n");
    printf("Please enter question id: ");
    // Get question id
    char question_id[255];
    scanf(" %[^\n]255s", question_id);
    __fpurge(stdin);
    search_question_by_id question;
    question.opcode = 601;
    strcpy(question.question_id, question_id);
    question.next_page = 0;
    // Send question id to server
    send(sockfd, &question, sizeof(question), 0);
    // Receive number of found question
    int numOfFoundQuestion;
    recv(sockfd, &numOfFoundQuestion, sizeof(question_data), 0);
    // If no question found
    if(numOfFoundQuestion == 0){
        printf("No question found!\n");
        searchQuestion(sockfd);
        return;
    }else{ // If found question
        int numCurrentQuestion = 0;
        while(numCurrentQuestion < numOfFoundQuestion){
            printf("Found %d question(s)!\n", numOfFoundQuestion);
            question_data **question_list;
            question_list = malloc(sizeof(question_data*));
            *question_list = malloc(sizeof(question_data)*10);
            send(sockfd, &numCurrentQuestion, 4, 0); // Send the begin index to search question
            // Receive 10 question from server
            int num_question_recv = 0;
            recv(sockfd, &num_question_recv, 4, 0);  
            printf("num_question_recv: %d\n", num_question_recv);
            for(int i = 0; i < num_question_recv; i++){
                send(sockfd, "OK", sizeof("OK"), 0);
                recv(sockfd, (*(question_list)+i), sizeof(question_data), 0);
                printf("Question %d: %s\n", i+1, (*(question_list)+i)->content);
            }
            numCurrentQuestion += 10;
            printf("\n1. Next page\n");
            printf("2. Back\n");
            printf("Please choose your option: ");
            int option;
            scanf(" %1d", &option);
            __fpurge(stdin);
            switch(option){
                case 1:
                    question.next_page = 1;
                    send(sockfd, &question, sizeof(question), 0);
                    break;
                case 2:
                    searchQuestion(sockfd);
                    return;
                default:
                    searchQuestion(sockfd);
                    return;
            }
        }
    }
    //searchQuestion(sockfd);
}

void searchQuestionByContent(int sockfd){
    printf("\nSearch question by content!\n");
    printf("Please enter question content: ");
    // Get question content
    char question_content[3000];
    scanf(" %[^\n]3000s", question_content);
    __fpurge(stdin);
    search_question_by_content question;
    question.opcode = 602;
    question.next_page = 0;
    strcpy(question.question_content, question_content);
    // Send question content to server
    send(sockfd, &question, sizeof(question), 0);
    // Receive number of found question
    int numOfFoundQuestion;
    recv(sockfd, &numOfFoundQuestion, sizeof(question_data), 0);
    // If no question found
    if(numOfFoundQuestion == 0){
        printf("No question found!\n");
        searchQuestion(sockfd);
        return;
    }else{ // If found question
        int numCurrentQuestion = 0;
        while(numCurrentQuestion < numOfFoundQuestion){
            printf("Found %d question(s)!\n", numOfFoundQuestion);
            question_data **question_list;
            question_list = malloc(sizeof(question_data*));
            *question_list = malloc(sizeof(question_data)*10);
            send(sockfd, &numCurrentQuestion, 4, 0); // Send the begin index to search question
            // Receive 10 question from server
            int num_question_recv = 0;
            recv(sockfd, &num_question_recv, 4, 0);
            for(int i = 0; i < num_question_recv; i++){
                send(sockfd, "OK", sizeof("OK"), 0);
                recv(sockfd, (*(question_list)+i), sizeof(question_data), 0);
                printf("Question %d: %s\n", i+1, (*(question_list)+i)->content);
            }
            numCurrentQuestion += 10;
            printf("\n1. Next page\n");
            printf("2. Back\n");
            printf("Please choose your option: ");
            int option;
            scanf(" %1d", &option);
            __fpurge(stdin);
            switch(option){
                case 1:
                    question.next_page = 1;
                    send(sockfd, &question, sizeof(question), 0);
                    break;
                case 2:
                    searchQuestion(sockfd);
                    return;
                default:
                    searchQuestion(sockfd);
                    return;
            }
        }
    }
    //searchQuestion(sockfd);
}

void advanceQuestionSearch(int sockfd){
    printf("\nAdvance question search!\n");
    char question_id[255];
    char question_content[3000];
    printf("Please enter question id: ");
    scanf(" %[^\n]255s", question_id);
    __fpurge(stdin);
    printf("Please enter question content: ");
    scanf(" %[^\n]3000s", question_content);
    __fpurge(stdin);
    char topic[255];
    printf("Please choose a subject: \n");
    for(int i = 0; i<19; i+=2){
        printf("%d. %s\t", i+1, subject_list[i]);
        printf("%d. %s\n", i+2, subject_list[i+1]);
    }
    printf("%d. %s\n", 21, subject_list[20]);
    int subject;
    scanf(" %d", &subject);
    __fpurge(stdin);
    if(subject < 1 || subject > 21){
        printf("Invalid option!\n");
        subject = 0;
    }
    printf("Please enter a topic: ");
    scanf(" %[^\n]255s", topic);
    __fpurge(stdin);
    advance_search_question question;
    question.opcode = 603;
    strcpy(question.question_id, question_id);
    strcpy(question.question_content, question_content);
    strcpy(question.subject, subject_list[subject-1]);
    strcpy(question.topic, topic);
    // Send question content to server
    send(sockfd, &question, sizeof(question), 0);
    // Receive number of found question
    int numOfFoundQuestion;
    recv(sockfd, &numOfFoundQuestion, sizeof(question_data), 0);
    // If no question found
    if(numOfFoundQuestion == 0){
        printf("No question found!\n");
        searchQuestion(sockfd);
        return;
    }else{ // If found question
        int numCurrentQuestion = 0;
        while(numCurrentQuestion < numOfFoundQuestion){
            printf("Found %d question(s)!\n", numOfFoundQuestion);
            question_data **question_list;
            question_list = malloc(sizeof(question_data*));
            *question_list = malloc(sizeof(question_data)*10);
            send(sockfd, &numCurrentQuestion, 4, 0); // Send the begin index to search question
            // Receive 10 question from server
            int num_question_recv = 0;
            recv(sockfd, &num_question_recv, 4, 0);
            for(int i = 0; i < num_question_recv; i++){
                recv(sockfd, (*(question_list)+i), sizeof(question_data), 0);
                send(sockfd, "OK", sizeof("OK"), 0);
                printf("Question %d: %s\n", i+1, (*(question_list)+i)->content);
            }
            numCurrentQuestion += 10;
            printf("\n1. Next page\n");
            printf("2. Back\n");
            printf("Please choose your option: ");
            int option;
            scanf(" %1d", &option);
            __fpurge(stdin);
            switch(option){
                case 1:
                    question.next_page = 1;
                    send(sockfd, &question, sizeof(question), 0);
                    break;
                case 2:
                    searchQuestion(sockfd);
                    return;
                default:
                    searchQuestion(sockfd);
                    return;
            }
        }
    }
}


void searchQuestion(int sockfd){
    printf("\nSearch question!\n");
    printf("1. Search by question id\n");
    printf("2. Search by question content\n");
    printf("3. Advance question search\n");
    printf("4. Back\n");
    printf("Please choose your option: ");
    int option;
    scanf(" %1d", &option);
    __fpurge(stdin);
    switch(option){
        case 1:
            searchQuestionById(sockfd);
            return;
        case 2:
            searchQuestionByContent(sockfd);
            return;
        case 3:
            advanceQuestionSearch(sockfd);
            return;
        case 4:
            manageQuestion(sockfd);
            return;
        default:
            printf("Invalid option!\n");
            break;
    }
    searchQuestion(sockfd);
}


//############## END SEARCH FUNCTION #################

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//@ “If opportunity doesn’t knock, build a door.” ―Kurt Cobain

$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

//############## ADD/DELETE QUESTION #################
void addQuestion(int sockfd){
    printf("\nAdd new question to question database!\n");
    add_question question;
    char question_id[51], question_content[3000], subject[30], topic[150];
    char op[4][300];
    int ans;

    //Get question information
    printf("Please enter question id: ");
    scanf(" %51s", question_id);
    printf("Please enter question content: ");
    scanf(" %[^\n]1000", question_content);
    printf("Please enter subject: \n");
    scanf(" %[^\n]30s", subject);
    printf("Please enter topic: ");
    scanf(" %[^\n]150s", topic);
    printf("Please enter 4 options: \n");
    for(int i = 0; i < 4; i++){
        printf("Option %d: ", i+1);
        scanf(" %[^\n]300s", op[i]);
    }
    printf("Please enter the correct answer: ");
    scanf(" %d", &ans);

    //Send question information to server
    question.opcode = 604;
    strcpy(question.question_id, question_id);
    strcpy(question.question_content, question_content);
    strcpy(question.subject, subject);
    strcpy(question.topic, topic);
    for(int i = 0; i < 4; i++){
        strcpy(question.op[i], op[i]);
    }
    question.ans = ans;
    printf("Are you sure to add this question? (y/n)\n");
    char opt;
    scanf(" %c", &opt);
    if(opt == 'n'){
        manageQuestion(sockfd);
        return;
    }
    send(sockfd, &question, sizeof(question), 0);
    int reply;
    recv(sockfd, &reply, sizeof(int), 0);
    if(reply == 1){
        printf("Add question successfully!\n");
    }
    else{
        printf("Cannot add question!\n");
    }
    manageQuestion(sockfd);
}

void deleteQuestion(int sockfd){
    printf("\nDelete question from question database!\n");
    char question_id[51];
    printf("Please enter question id: ");
    scanf(" %51s", question_id);
    question_data question;
    question.opcode = 605;
    strcpy(question.question_id, question_id);
    printf("Are you sure to delete question with id %s? (y/n)\n", question_id);
    char opt;
    scanf(" %c", &opt);
    if(opt == 'n'){
        manageQuestion(sockfd);
        return;
    }
    send(sockfd, &question, sizeof(question), 0);
    int reply;
    recv(sockfd, &reply, sizeof(int), 0);
    if(reply == 1){
        printf("Delete question successfully!\n");
    }
    else{
        printf("Cannot delete question!\n");
    }
    manageQuestion(sockfd);
}


//############## END ADD/DELETE QUESTION #################

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//@ “Do what you can, with what you have, where you are.” ― Theodore Roosevelt

$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/


//?############## MANAGE EXAM #################


//!TODO: Manage exam
void showExamManagementInterface(){
    printf("\nExam management interface!\n");
    printf("1. Search Exam\n");
    printf("2. Create new Exam\n");
    printf("3. Edit Existing Exam\n");
    //printf("4. Delete exam\n");
    printf("4. Back\n");
    printf("Please choose your option: ");
}

void searchExam(int sockfd){
    int exam_id;
    printf("Enter the id of the exam: ");
    scanf(" %d", &exam_id);
    __fpurge(stdin);
    search_exam_by_id search_request;
    search_request.opcode = 701;
    search_request.exam_id = exam_id;
    send(sockfd, &search_request, sizeof(search_request), 0); //Send request to the server to get info of exam
    int numberOfExam;
    recv(sockfd, &numberOfExam, sizeof(int), 0); // Receive number of exam
    if(numberOfExam == 0){
        printf("Cannot find the exam with id %d\n", exam_id);
        searchExam(sockfd);
        return ;
    }
    else{ 
        exam_data exam;
        //Show information of exam
        recv(sockfd, &exam, sizeof(exam_data), 0);
        send(sockfd, "OK", sizeof("OK"), 0);
        printf("Information of exam %d\n", exam.exam_id);
        printf("Title: %s\n", exam.title);
        printf("Number of question: %d\n", exam.number_of_question);
        printf("Author: %d\n", exam.admin_id);
        printf("Do you want to see in detail? (y/n)\n");
        char opt;
        scanf(" %c", &opt);
        if(opt == 'n') return;
        if(opt == 'y'){
            request_question_list request_question;    
            request_question.opcode = 204;
            request_question.exam_id = exam_id;
            request_question.number_of_question = exam.number_of_question;
            send(sockfd, &request_question, sizeof(request_question_list), 0);  //Send request to get question from the server to show for admin

            question_data **question_list;
            question_list = malloc(sizeof(question_data*));
            *question_list = malloc(sizeof(question_data)*(request_question.number_of_question));
            for(int i=0; i<request_question.number_of_question; i++){
                recv(sockfd, (*(question_list)+i), sizeof(question_data), 0); // Receive the question from server
                send(sockfd, "OK", sizeof("OK"), 0);
                printf("Question %d: %s\n", i+1, (*(question_list)+i)->content);
            for(int j=0; j<4; j++){
                printf("%c. %s\n", j+65, (*(question_list)+i)->op[j]);
            }
            }
        }
    }
    manageExam(sockfd);
}

void createRandomExam(int sockfd){
    int reply;
    create_random_exam request;
    request.opcode = 702;
    request.user_id = data.user_id;
    printf("Enter the number of question: \n");
    scanf("%d", &request.number_of_question);
    printf("Enter the title of exam: \n");
    scanf("%s", request.title);
    request.admin_id = data.user_id;
    send(sockfd, &request, sizeof(request), 0); // Send request to create random exam
    recv(sockfd, &reply, sizeof(int), 0); // Receive reply from server ( 1 is exam is created successfully)
    if(reply == 1){
        printf("The exam is created\n");
    }
    else{
        printf("Cannot create the exam\n");
    }
    manageExam(sockfd);
}

void createExamByQuestion(int sockfd){
    create_exam request;
    char question_id[51];
    request.opcode = 703;
    int reply;
    printf("Enter the name of the exam: \n");
    scanf("%s", request.title);
    printf("Enter the number of question in the exam: \n");
    scanf("%d", &request.number_of_ques);
    send(sockfd, &request, sizeof(create_exam), 0); // Send request to server to create exam- opcode = 703
    int num_of_question;
    recv(sockfd, &num_of_question, sizeof(int), 0); // Receive number of question from server
    char question_ids[num_of_question][51];
    for(int i = 0; i < request.number_of_ques; i++){
        printf("Enter the question id to add: \n");
        scanf(" %s", question_ids[i]);
    }
    send(sockfd, question_ids, sizeof(question_ids), 0); // Send the signal to end recv in server
    recv(sockfd, &reply, sizeof(int), 0); //Receive reply from server ( 1 if the exam was created)
    if(reply == 1){
        printf("The exam has been created\n");
    }
    else{
        printf("Cannot create the exam\n");
    }
    manageExam(sockfd);
}

void createExam(int sockfd){
    int option;
    printf("1. Create a random exam\n");
    printf("2. Create exam from chosen question\n");
    printf("3. Back\n");
    printf("Choose an option to start create!\n");
    scanf("%d",&option);
    switch(option){
        case 1:
            createRandomExam(sockfd);
            break;
        case 2:
            createExamByQuestion(sockfd);
            break;
        case 3:
            manageExam(sockfd);
            break;
        default:
            createExam(sockfd);
            break;
    }
}

// void deleteExam(int sockfd){
//     int exam_id;
//     printf("Enter the id of the exam: ");
//     scanf(" %d", &exam_id);
//     __fpurge(stdin);
//     request_edit request;
//     request.opcode = 704;
//     request.exam_id = exam_id;
//     request.type = 0;
//     send(sockfd, &request, sizeof(request_edit), 0); // Send request to delete exam
//     int reply;
//     recv(sockfd, &reply, sizeof(int), 0); // Receive reply from server ( 1 is exam is deleted successfully)
//     if(reply == 1){
//         printf("The exam is deleted\n");
//     }
//     else{
//         printf("Cannot delete the exam\n");
//     }
//     manageExam(sockfd);
// }

void manageExam(int sockfd){
    showExamManagementInterface();
    int option;
    scanf(" %1d", &option);
    __fpurge(stdin);
    switch(option){
        case 1:
            searchExam(sockfd);
            return;
        case 2:
            createExam(sockfd);
            return;
        case 3:
            //editExam(sockfd);
            return;
        case 4:
            showMainAppMenuAdmin(sockfd);
            return;
        default:
            printf("Invalid option!\n");
            break;
    }
    manageExam(sockfd);
}

//?############## END MANAGE EXAM #################


/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//@ “If everything seems to be under control, you’re not going fast enough.” ―Mario Andretti

$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/


//?############## MANAGE EXAM ROOM #################
//Day la bat dau Room nhe
//!TODO: Manage exam room
void manageExamRoom(int client_sockfd){
    while(1){
        printf("MANAGE ROOM MENU!\n");
        printf("1. Create a room\n");
        printf("2. Delete a room\n");
        printf("3. Show your rooms\n");
        printf("4. Back\n");
        printf("Please choose your option: ");
        int option;
        scanf(" %1d", &option);
        __fpurge(stdin);
        switch(option){
        case 1:
            createRoom(client_sockfd);
            break;
        case 2:
            deleteRoom(client_sockfd);
            break;
        case 3:
            showYourRoom(client_sockfd);
            break;
        case 4:
            showMainAppMenuAdmin(client_sockfd);
            return;
        default:
            printf("Invalid option!\n");
            break;
        }
    }
}

void createRoom(int client_sockfd){
    //opcode = 801
    char r_name[128];
    printf("Create a room!\n");
    printf("Enter the room's name: ");
    fgets(r_name, 127, stdin);
    r_name[strlen(r_name)-1] = '\0';
    __fpurge(stdin);

    room_create_del room;
    room.opcode = 801;
    strcpy(room.r_name, r_name);
    strcpy(room.username, data.username);

    send(client_sockfd, &room, sizeof(room), 0);

    int oke_signal;
    recv(client_sockfd, &oke_signal, sizeof(int), 0);
    if(oke_signal == -1){
        printf("The room's name '%s' is already existed\n\n", room.r_name);
    }
    else{
        printf("Room created successfully\n\n");
    }
}

void deleteRoom(int client_sockfd){
    //opcode = 802
    char r_name[128];
    printf("Delete a room!\n");
    printf("Enter the room's name: ");
    fgets(r_name, 127, stdin);
    r_name[strlen(r_name)-1] = '\0';
    __fpurge(stdin);

    room_create_del room;
    room.opcode = 802;
    strcpy(room.r_name, r_name);
    strcpy(room.username, data.username);

    send(client_sockfd, &room, sizeof(room), 0);

    int oke_signal;
    recv(client_sockfd, &oke_signal, sizeof(int), 0);
    // 1 -> success
    // -1 -> that room does not exist
    // -2 -> you don't have permission to delete it
    if(oke_signal == -1){
        printf("The room '%s' does not exist\n\n", room.r_name);
    }
    else if(oke_signal == -2){
        printf("You don't have permission to delete room '%s'\n\n", room.r_name);
    }
    else{
        printf("Room deleted successfully\n\n");
    }
}



//1
void addUserToRoom(int client_sockfd, int r_id){
    int flag;
    request_add_remove_student request;
    while(1){
        printf("\nEnter the user_id of student to you want to add to this room (enter 0 to get back): ");
        scanf("%d", &(request.user_id));
        __fpurge(stdin);

        if(request.user_id == 0){
            return;
        }

        request.opcode = 811;
        request.r_id = r_id;
        
        send(client_sockfd, &request, sizeof(request_add_remove_student), 0); //Send request to add student in a room
        recv(client_sockfd, &flag, sizeof(int), 0); //Receive the reply message from server\

        switch (flag)
        {
        case 1:
            printf("Add successfully: user's id = %d\n to room's id = %d\n", request.user_id, request.r_id);
            break;
        case -2:
            printf("Add fail: no user has id = %d\n", request.user_id);
            break;
        case -3:
            printf("Add fail: user with id = %d has been added to room_id %d before\n", request.user_id, request.r_id);
            break;
        case -4:
            printf("Add fail: user with id = %d is you!\nYou cannot add yourself to the room you created\n", request.user_id);
            break;
        default:
            printf("Add failt: Something went wrong at addUserToRoom()\n");
            break;
        }
    }
}

//2
void deleteUserFromRoom(int client_sockfd, int r_id){
    int flag;
    request_add_remove_student request;
    while(1){
        printf("\nEnter the user-id of student you want to delete from the room (enter 0 to get back): ");

        scanf("%d", &(request.user_id));
        __fpurge(stdin);

        if(request.user_id == 0){
            return;
        }

        request.opcode = 812;
        request.r_id = r_id;
        
        send(client_sockfd, &request, sizeof(request_add_remove_student), 0); //Send request to add student in a room
        recv(client_sockfd, &flag, sizeof(int), 0); //Receive the reply message from server
        if(flag == 1){
            printf("Delete successfully: User with id %d has been removed from this room!\n", request.user_id);
        }
        else if (flag = -2){
            printf("Delete fail: user with id = %d is not in this room\n", request.user_id);
        }
        else{
            printf("Something went wrong in deleteUserFromRoom()\n");
        }
    }
}

//3

void addTestFromBank(int client_sockfd, int r_id){
    //opcode = 813
    int opcode = 813;
    int num_of_test_in_bank;
    int oke_signal = 1;
    test_in_bank test_info;
    room_test test_data;

    send(client_sockfd, &opcode, sizeof(int), 0);

    //một hàm để show test from bank
    {
        recv(client_sockfd, &num_of_test_in_bank, sizeof(int), 0);
        send(client_sockfd, &oke_signal, sizeof(int), 0);
    }

    int list_test_id[num_of_test_in_bank];

    printf("%-10s%-40s%-10s\n", "test_id", "test_name", "num_of_question");
    for(int i=0; i<num_of_test_in_bank; i++){
        memset(&test_info, 0, sizeof(test_in_bank));
        recv(client_sockfd, &test_info, sizeof(test_in_bank), 0);
        send(client_sockfd, &oke_signal, sizeof(int), 0);
        printf("%-10d%-40s%-10d\n", test_info.test_id, test_info.test_name, test_info.number_of_question);
        
        list_test_id[i] = test_info.test_id;
    }

    int chosen_test_id;
    while(1){
        printf("Enter the id of the test you want to add to room: ");
        scanf("%d", &chosen_test_id);
        __fpurge(stdin);
        int valid = 0;
        for(int i=0; i<num_of_test_in_bank; i++){
            if(chosen_test_id == list_test_id[i]){
                valid = 1;
                break;
            }
        }
        if(valid == 0){
            printf("Invalid test_id!\n");
        }
        if(chosen_test_id == 0){
            recv(client_sockfd, &oke_signal, sizeof(int), 0);
            send(client_sockfd, "STOP", sizeof(5), 0);
            return;
        }
        else{
            break;
        }

    }
    recv(client_sockfd, &oke_signal, sizeof(int), 0);
    send(client_sockfd, "OK", sizeof("OK"), 0);
    recv(client_sockfd, &oke_signal, sizeof(int), 0);
    memset(&test_data, 0, sizeof(room_test));

    test_data.r_id = r_id;
    test_data.test_id = chosen_test_id;

    printf("Enter the open time (in form yyyy-mm-dd hh:mm:ss): ");
    fgets(test_data.open_time, 100, stdin);
    test_data.open_time[strlen(test_data.open_time)-1] = '\0';

    printf("Enter the close time (in form yyyy-mm-dd hh:mm:ss): ");
    fgets(test_data.close_time, 100, stdin);
    test_data.close_time[strlen(test_data.close_time)-1] = '\0';

    printf("Enter the duration (in minutes): ");
    scanf("%d", &(test_data.duration));

    send(client_sockfd, &test_data, sizeof(room_test), 0);
    recv(client_sockfd, &oke_signal, sizeof(int), 0);

}


//4 - hàm này sẽ tạo test, cho vào bank
//rồi hỏi luôn người đó là có muốn cho test này vào room luôn ko?

void createTest(int client_sockfd, int r_id){
    //opcode = 814
    int opcode = 814;
    int flag;
    int oke_signal = 1;
    char test_name[256];
    int num_of_question;

    //send the opcode
    send(client_sockfd, &opcode, sizeof(int), 0);
    recv(client_sockfd, &oke_signal, sizeof(int), 0);

    //get the test_name
    while(1){
        printf("Enter the test_name: ");
        fgets(test_name, 100, stdin);
        __fpurge(stdin);
        test_name[strlen(test_name)-1] = '\0';
        if(strlen(test_name) > 50){
            printf("Please input a test_name with less than or equal to 50 characters\n");
            continue;
        }
        else{
            send(client_sockfd, test_name, strlen(test_name)+1, 0);
            recv(client_sockfd, &flag, sizeof(int), 0);
            //flag = 1 -> success
            //flag = -1 -> duplicated
            if(flag == 1){
                break;
            }
            else if(flag == -1){
                printf("This test_name has existed already, please enter new test_name\n");
            }
            else{
                printf("Something went wrong\n");
            }
        }
    }

    //get the number of question
    while(1){
        printf("Enter the number of question: ");
        scanf("%d", &num_of_question);
        __fpurge(stdin);
        if(num_of_question <= 0){
            printf("Invalid input. Please re-enter!\n");
        }
        else{
            break;
        }
    }

    //send the num_of_question
    send(client_sockfd, &num_of_question, sizeof(int), 0);
    recv(client_sockfd, &oke_signal, sizeof(int), 0);

    //send admin_name
    send(client_sockfd, data.username, 64, 0);
    

    recv(client_sockfd, &oke_signal, sizeof(int), 0);

    printf("Create new test successfully\n");

}


/*
void createTest(int client_sockfd, int r_id){
    //opcode = 814
    int reply;
    createPrivateTest request;
    request.opcode = 814;
    printf("Enter the title of the test: \n");
    scanf("%s", request.test_name);
    printf("Enter the author's id: \n");
    scanf("%d", &request.admin_id);
    printf("Enter the number of question in this test: \n");
    scanf("%d", &request.number_of_question);
    send(client_sockfd, &request, sizeof(createPrivateTest), 0);//Send request to server
    recv(client_sockfd, &reply, sizeof(int), 0);
    if(reply == 1){
        printf("Creating successfully !\n");
    }
    else{
        printf("Cannot create an random test in this room\n");
    }
}
*/

/*
void createTestByQuestion(int client_sockfd, int r_id){
    //opcode = 815
    int reply;
    createPrivateTest request;
    int number_of_question;
    request.opcode = 815;
    printf("Enter the title of the test: \n");
    scanf("%s", request.test_name);
    printf("Enter the number of the question in this test: \n");
    scanf("%d", &request.number_of_question);
    send(client_sockfd, &request, sizeof(createPrivateTest), 0); // Send request to server
    recv(client_sockfd, &number_of_question, sizeof(int), 0);// Receive the number of question from server
    char question_ids[number_of_question][255];
    for(int i = 0; i < number_of_question; i++){
        printf("Enter the id-question you want to add: \n");
        scanf("%s", question_ids[i]);
    }
    send(client_sockfd, question_ids, sizeof(question_ids), 0);
    recv(client_sockfd, &reply, sizeof(int), 0);
    if(reply == 1){
        printf("Creating successfully !\n");
    }
    else{
        printf("Cannot create this test in this room\n");
    }
}
//5
*/

/*
void addTestFromBank(int client_sockfd, int r_id){
    //opcode = 813
    int reply;
    add_test_room request;
    request.opcode = 813;
    request.r_id = r_id;
    char open_time[22];
    char close_time[22];
    printf("Enter the test_id you want to add: \n");
    scanf("%d", &request.test_id);
    printf("Enter the duration of this test (in minute): \n");
    scanf("%d", &request.duration);
    printf("Enter the open time for this test: (in form yyyy-mm-dd hh:mm:ss)\n");
    scanf("%s", open_time);
    strcpy(request.open_time, open_time);
    printf("Enter the close time for this test: (in form yyyy-mm-dd hh:mm:ss)\n");
    scanf("%s", close_time);
    strcpy(request.close_time, close_time);
    send(client_sockfd, &request, sizeof(add_test_room), 0);
    recv(client_sockfd, &reply, sizeof(int), 0);
    if(reply == 1){
        printf("Adding test successfully !\n");
    }
    else{
        printf("Cannot add this test to the room !\n");
    }
}
*/

void deleteTestFromRoom(int client_sockfd, int r_id){
    //opcode = 815

}

//6
void Admin_showSubmittedRecordOfTest(int client_sockfd, int r_id){
    //opcode = 816
    int opcode = 816;
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

    //receive number of tests in room
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
            if(test_id = list_test_id[i]){
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
            printf("This test has no history record\n");
            return;
        }
        send(client_sockfd, &oke_signal, sizeof(int), 0);

    }

    //show records -- HERE
    {   
        printf("---History record of test with test_id = %d in this room---\n", test_id);
        printf("%-10s%-10s%-10s%-10s%-10s%-50s", "id", "r_id", "test_id", "user_id", "score", "submitted_time");
        for(int i=0; i<number_of_history_records; i++){
            memset(&history_data, 0, sizeof(room_history));
            recv(client_sockfd, &history_data, sizeof(room_history), 0);
            send(client_sockfd, &oke_signal, sizeof(int), 0);
            printf("%-10d%-10d%-10d%-10d%-10s%-50s",history_data.id, history_data.r_id, history_data.test_id, history_data.user_id, history_data.score, history_data.submitted_time);
        }
        recv(client_sockfd, &oke_signal, sizeof(int), 0);       
    }


}

//7
void Admin_showAllSubmittedRecord(int client_sockfd, int r_id){
    //opcode = 817
    int opcode = 817;
    send(client_sockfd, &opcode, sizeof(int), 0);

    room_history history_data;
    int number_of_history_records;
    int oke_signal = 1;

    //send r_id
    {
        recv(client_sockfd, &oke_signal, sizeof(int), 0);
        send(client_sockfd, &r_id, sizeof(int), 0);
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
        printf("---History record in room with id = %d---\n", r_id);
        printf("%-10s%-10s%-10s%-10s%-10s%-50s", "id", "r_id", "test_id", "user_id", "score", "submitted_time");

        for(int i=0; i<number_of_history_records; i++){
            memset(&history_data, 0, sizeof(room_history));
            recv(client_sockfd, &history_data, sizeof(room_history), 0);
            send(client_sockfd, &oke_signal, sizeof(int), 0);
            printf("%-10d%-10d%-10d%-10d%-10s%-50s",history_data.id, history_data.r_id, history_data.test_id, history_data.user_id, history_data.score, history_data.submitted_time);
        }
        recv(client_sockfd, &oke_signal, sizeof(int), 0);       
    }

}

void goIntoRoomYouAreAdmin(int client_sockfd, int r_id){
    while(1){
        printf("\nADMIN ROOM MENU !\n");
        printf("1. Add user\n");
        printf("2. Delete user\n");
        printf("3. Add a test from the bank\n");
        printf("4. Create a new random test\n");
        printf("5. Delete a test from room\n");
        printf("6. Show submitted record of a test\n"); //provide test's name
        printf("7. Show all submitted record\n");
        printf("8. Get back\n");
        printf("Please choose your option: ");
        int option;
        scanf(" %1d", &option);
        __fpurge(stdin);
            switch(option){
            case 1:
                addUserToRoom(client_sockfd, r_id);
                break;
            case 2:
                deleteUserFromRoom(client_sockfd, r_id);
                break;
            case 3:
                addTestFromBank(client_sockfd, r_id); //813
                return;
            case 4: 
                createTest(client_sockfd, r_id); //814
                break;
            case 5:
                deleteTestFromRoom(client_sockfd, r_id);//815
                break;
            case 6:
                Admin_showSubmittedRecordOfTest(client_sockfd, r_id);
                break;
            case 7:
                Admin_showAllSubmittedRecord(client_sockfd, r_id);
                break;
            case 8: 
                printf("Exit ROOM MENU\n\n");
                return;
            default:
                printf("Invalid option!\n");
                break;
            }
    }
}



void admin_processAnswer(char *str)
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

//1-user
void doATestInRoom(int client_sockfd, int r_id){
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
            admin_processAnswer(answer);
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
void User_showSubmittedRecordOfTest(int client_sockfd, int r_id){
    //opcode = 831
    int opcode = 831;
    send(client_sockfd, &opcode, sizeof(int), 0);

    room_history history_data;
    int number_of_history_records;
    int number_of_tests_in_room;
    int test_id;

    int oke_signal = 1;
    test_in_room test_data;
    int r_id_1 = r_id;
    //send r_id
    {
        recv(client_sockfd, &oke_signal, sizeof(int), 0);
        send(client_sockfd, &r_id_1, sizeof(int), 0);
    }

    //send user_name
    {
        recv(client_sockfd, &oke_signal, sizeof(int), 0);
        send(client_sockfd, data.username, strlen(data.username)+1, 0);
    }

    //receive number of tests in room
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
            if(test_id = list_test_id[i]){
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
        printf("%-10s%-10s%-10s%-10s%-10s%-50s", "id", "r_id", "test_id", "user_id", "score", "submitted_time");
        for(int i=0; i<number_of_history_records; i++){
            memset(&history_data, 0, sizeof(room_history));
            recv(client_sockfd, &history_data, sizeof(room_history), 0);
            send(client_sockfd, &oke_signal, sizeof(int), 0);
            printf("%-10d%-10d%-10d%-10d%-10s%-50s",history_data.id, history_data.r_id, history_data.test_id, history_data.user_id, history_data.score, history_data.submitted_time);
        }
        recv(client_sockfd, &oke_signal, sizeof(int), 0);       
    }

}

//3-user
void User_showAllSubmittedRecord(int client_sockfd, int r_id){
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
        printf("%-10s%-10s%-10s%-10s%-10s%-50s", "id", "r_id", "test_id", "user_id", "score", "submitted_time");

        for(int i=0; i<number_of_history_records; i++){
            memset(&history_data, 0, sizeof(room_history));
            recv(client_sockfd, &history_data, sizeof(room_history), 0);
            send(client_sockfd, &oke_signal, sizeof(int), 0);
            printf("%-10d%-10d%-10d%-10d%-10s%-50s",history_data.id, history_data.r_id, history_data.test_id, history_data.user_id, history_data.score, history_data.submitted_time);
        }
        recv(client_sockfd, &oke_signal, sizeof(int), 0);       
    }
}


void goIntoRoomYouAreUser(int client_sockfd, int r_id){
    while(1){
        printf("\nUSER ROOM MENU !\n");
        printf("1. Do a test\n");
        printf("2. Show submitted record of a test\n");
        printf("3. Show all submitted record\n");
        printf("4. Get back\n");
        printf("Please choose your option: ");
        int option;
        scanf(" %1d", &option);
        __fpurge(stdin);
            switch(option){
            case 1:
                doATestInRoom(client_sockfd, r_id);
                break;
            case 2:
                User_showSubmittedRecordOfTest(client_sockfd, r_id);
                break;
            case 3:
                User_showAllSubmittedRecord(client_sockfd, r_id);
                break;
            case 4: 
                printf("Exit USER ROOM MENU\n\n");
                return;
            default:
                printf("Invalid option!\n");
                break;
            }
    }
}

// Việt
void showAllYourRoom(int client_sockfd, int opcode){
    // printf("You are in show all your room\n");
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
                {
                    if(strcmp(r_admin_name_list[i], data.username) == 0){
                        valid = 1;
                    }
                    else{
                        valid = 2;
                    }
                    break;
                }
            }
        }
        if(valid != 0){
            break;
        }
        printf("Invalid Option!\n");
    }
    if(valid == 1){
        goIntoRoomYouAreAdmin(client_sockfd, go_r_id);
    }
    else if(valid == 2){
        goIntoRoomYouAreUser(client_sockfd, go_r_id);
    }
    else{
        printf("Something went wrong in showYourRoom()\n");
    }
}

void showYourRoom(int client_sockfd){
    while(1){
        printf("YOUR ROOM MENU!\n");
        printf("1. Show rooms you created\n");
        printf("2. Show rooms you were added\n");
        printf("3. Show all your rooms\n");
        printf("4. Get back\n");
        printf("Please choose your option: ");
        int option;
        scanf(" %1d", &option);
        __fpurge(stdin);
            switch(option){
            case 1:
                showAllYourRoom(client_sockfd, 803);
                break;
            case 2:
                showAllYourRoom(client_sockfd, 804);
                break;
            case 3:
                showAllYourRoom(client_sockfd, 805);
                break;
            case 4:
                return;
            default:
                printf("Invalid option!\n");
                break;
            }
    }
}

//?############## END MANAGE EXAM ROOM #################





//?############## END MANAGE EXAM ROOM #################


/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//@ “The only difference between ordinary and extraordinary is that little extra.” ―Jimmy Johnson

$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/


//?############## GET EXAM ROOM RESULT #################

//!TODO: Get exam room result
void getExamRoomResult(int sockfd){

}

//?############## END GET EXAM ROOM RESULT #################

//!TODO: Get user info
void getUserInfo(int sockfd){
    request_user_info info;
    info.opcode = 103;
    info.user_id = data.user_id;
    char username[30];
    printf("Enter student's name: ");
    scanf(" %s", username);
    strcpy(info.username, username);
    send(sockfd, &info, sizeof(info), 0);
    int check;
    recv(sockfd, &check, sizeof(int), 0);
    if(check == 1){
        printf("Get user info successfully!\n");
        send(sockfd, "OK", sizeof("OK"), 0);
        register_data user_info;
        recv(sockfd, &user_info, sizeof(user_info), 0);
        //Print user info
        printUserInfo(user_info);
        showMainAppMenuAdmin(sockfd);
    }
    else{
        printf("Cannot get user info!\n");
        showMainAppMenuAdmin(sockfd);
        return;
    }
}

//!TODO: Approve admin request
void approveAdminRequest(int sockfd){
    check_admin_previlege_request request;
    request.opcode = 302;
    request.next_page = 0;
    send(sockfd, &request, sizeof(request), 0);
    int numOfRequest;
    recv(sockfd, &numOfRequest, sizeof(int), 0);
    if(numOfRequest == 0){
        printf("No request found!\n");
        showMainAppMenuAdmin(sockfd);
        return;
    }
    else{
        int numCurrentRequest = 0;
        while(numCurrentRequest < numOfRequest){
            printf("Found %d request(s)!\n", numOfRequest);
            admin_request **request_list;
            request_list = malloc(sizeof(admin_request*));
            *request_list = malloc(sizeof(admin_request)*10);
            send(sockfd, &numCurrentRequest, 4, 0); // Send the begin index to search question
            // Receive 10 question from server
            int num_request_recv = 0;
            recv(sockfd, &num_request_recv, 4, 0);
            for(int i = 0; i < num_request_recv; i++){
                send(sockfd, "OK", sizeof("OK"), 0);
                recv(sockfd, (*(request_list)+i), sizeof(admin_request), 0);
                printf("Request %d: %s\tDate: %s\n", (*(request_list)+i)->id, (*(request_list)+i)->username, (*(request_list)+i)->date);
            }
            numCurrentRequest += 10;
            printf("\n1. Approve request\n");
            printf("2. Next page\n");
            printf("3. Back\n");
            printf("Please choose your option: ");
            int option;
            scanf(" %1d", &option);
            __fpurge(stdin);
            switch(option){
                case 1:
                    do{
                    char username[30];
                    printf("Please enter username to approve request: ");
                    scanf(" %s", username);
                    __fpurge(stdin);
                    approve_admin_request approve_request;
                    approve_request.opcode = 304;
                    strcpy(approve_request.username, username);
                    send(sockfd, &approve_request, sizeof(approve_request), 0);
                    int reply;
                    recv(sockfd, &reply, sizeof(int), 0);
                    if(reply == 1){
                        printf("Approve request successfully!\n");
                    }
                    else{
                        printf("Cannot approve request!\n");
                    }
                    printf("Do you want to approve another request? (y/n)\n");
                    char opt;
                    scanf(" %c", &opt);
                    __fpurge(stdin);
                    if(opt != 'y'){
                        showMainAppMenuAdmin(sockfd);
                        return;
                    }}while(1);
                case 2:
                    request.next_page = 1;
                    send(sockfd, &request, sizeof(request), 0);
                    break;
                default:
                    showMainAppMenuAdmin(sockfd);
                    return;
            }
        }
    }
}


