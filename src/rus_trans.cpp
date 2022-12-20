
#include "../include/common.h"
#include "../include/rus_trans.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


const char *RusTranslate(const char *str)
{
    ERROR_CHECK(str == NULL, NULL);

    printf("enter translate\n");
    size_t str_len = strlen(str);

    char *new_str = (char *) calloc(str_len * MAX_LETTER_LEN, sizeof(char));
    ERROR_CHECK(new_str == NULL, NULL);
    new_str[0] = '\0';

    if ((str[0] >= 'A' && str[0] <= 'Z') ||
        (str[0] >= 'a' && str[0] <= 'z'))
    {
        strcpy(new_str, str);
        return new_str;
    }

    for (unsigned int i = 0; i < str_len; i++)
    {
        if (isupper(str[i]))
        {
            if (str[i] == '¨')
                strcat(new_str, HIGH_YO);
            else
                strcat(new_str, RUS_HIGH_TRANSLIT[str[i] - 'À']);
            
        }
        else if (islower(str[i]) || str[i] == 'ÿ')
        {
            if (str[i] == '¸')
                strcat(new_str, LOW_YO);
            else
                strcat(new_str, RUS_LOW_TRANSLIT[str[i] - 'à']);
        }

        else
        {
            char symb[2] = {0};
            symb[0] = str[i];
            symb[1] = '\0';
            strcat(new_str, symb);
        }
    }

    return new_str;
}