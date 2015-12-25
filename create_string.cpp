#include <stdio.h>
#include <iostream>
#include <string.h>
#include <limits.h>

char *create_string(char *result_string, char *str1, char *str2)
{
    strcpy(result_string, "/media/");
    strcat(result_string, str1);
    strcat(result_string, "/");
    strcat(result_string, str2);
    return result_string;
}
