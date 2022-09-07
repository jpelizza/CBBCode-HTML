#include "cbbhtml.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <assert.h>

return_buffer[4096];

int simple_regex_example(){
    regex_t regex;
    regmatch_t m;
    
    const char *reg_str = "\\[b\\][^\\[]*?\\[\\/b\\]";//(?:(\[b\].*\[\/b\])+)+?
    char *data = (char*)malloc(sizeof(char)*1024);
    strcpy(data,"asd[b]this is bb code[/b]fghjkl[b]this is bb code[/b]bnm");

    if (regcomp(&regex,reg_str,REG_EXTENDED) == 0){
        printf("Regular expression compiled successfully.");
    }
    else{
        printf("Compilation error.");
    }
    while(1){
        int reg_result = regexec(&regex, data,1,&m,0);
        if(reg_result==0){
            strcpy(data,str_replace(data,data+m.rm_eo-4,4,"</strong>"));
            strcpy(data,str_replace(data,data+m.rm_so,3,"<strong>"));
        }else{
            break;
        }
    }
    printf("%s\n",data);
    return 0;
}

/**
 * @brief Replaces on str, from ptr1 to ptr1+len with ptr2
 *
 * @param str String to have text replaced
 * @param ptr pointer to where replacing will take place
 * @param ptr_len lenght of word to be replaced
 * @param substr substring to take prt1's place
 */
char *str_replace(const char *str, const char *ptr, size_t ptr_len, const char *substr) {
    char *prestr = (char *)malloc(sizeof(char) * strlen(str));
    char *poststr = (char *)malloc(sizeof(char) * strlen(str));
    memset(prestr,0,sizeof(strlen(str)));
    memset(poststr,0,sizeof(strlen(str)));
    memset(return_buffer,0,sizeof(return_buffer));

    strncpy(prestr, str, (size_t)(ptr-str));
    prestr[(size_t)(ptr-str)]='\0'; // stupid fix for strncpy
    printf("%d - %d - %s\n", (size_t)(ptr-str),strlen(prestr),prestr);

    strcpy(poststr, str + (size_t)(ptr - str + ptr_len));

    strcat(prestr, substr);
    strcat(prestr, poststr);

    strcpy(return_buffer,prestr);
    free(prestr);
    free(poststr);

    return return_buffer;
}