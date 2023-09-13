#ifndef FIRST_H
#define FIRST_H

void run_cmd(char*** args);

char*** parse_cmd(char* arg);

int pipes_num(char* str);

int tokens_num(char* str);

void parse_tokens(char** args, char* str);

#endif // FIRST_H
