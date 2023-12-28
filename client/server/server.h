#ifndef SERVER_H
#define SERVER_H
#include <mysql/mysql.h>
extern MYSQL *conn;
void extract_error(MYSQL *conn);
MYSQL_RES *make_query(char query[]);
void saveHistory(char username[], int test_id, int number_of_right_ans, int number_of_questions);
int getDifferentTime(const char* date_str1, const char* date_str2);
#endif