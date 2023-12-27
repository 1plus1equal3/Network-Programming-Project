#ifndef UTILS_H
#define UTILS_H
#include "../data.h"
void printUserInfo(register_data user_info){
    printf("\nUser info:\n");
    printf("Username: %s\n", user_info.username);
    printf("Password: %s\n", "**********");
    printf("Email: %s\n", user_info.email);
    printf("User previlege: User\n");
    printf("\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n");
    //printf("Id: %d\n", user_info.id);
}
#endif