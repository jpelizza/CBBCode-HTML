#include "cbbhtml.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <assert.h>

/**
 * @brief Writes translated bbcode into buffer;
 * 
 * @param bbcode bbcode string to translate
 * @param buffer buffer to write translated bbcodfe to html to
 * @param buffer_size send buffer size, -1 if not allocated memory
 * @return int return error code 0 if successeful
 * 1 if allocation error
 * 2 if regex compilation error
 */
int bbcodetohtml(const char *bbcode,char *buffer, int buffer_size){
    regex_t regex;
    regmatch_t m;

    const char *simple_bb_tokens[] = {"b", "i", "u", "s","url","quote", "code"};
    const char *simple_html_tokens_begin[] = {"<strong>", "<em>", "<ins>", "<del>","<a href\"#\">","<blockquote>", "<pre>"};
    const char *simple_html_tokens_end[] = {"</strong>", "</em>", "</ins>", "</del>","</a>","</blockquote>", "</pre>"};
    
    // const char *reg_str = "\\[b\\][^\\[]*?\\[\\/b\\]";
    char *reg_str = (char*)malloc(sizeof(char)*64); //arbitrary number as of now, make sure to write code that takes largest possible string and apply size;
    printf("%d - passou\n",1);
    memset(reg_str,'\0',64);
    const char *simple_reg_str_first = "\\[";
    const char *simple_reg_str_second = "\\].*?\\[\\/";
    const char *simple_reg_str_third = "\\]";
    printf("%d - passou\n",2);

    if(buffer_size==-1){
        buffer = (char*)malloc((sizeof(char)*strlen(bbcode))+1);
    }else if(buffer_size<strlen(bbcode)){
        buffer = (char*)realloc(buffer,(sizeof(char)*strlen(bbcode))+1);
    }
    if(buffer==NULL){
        return 1;
    }
    strcpy(buffer,bbcode);

    printf("%d - passou\n",3);

    for (unsigned int symbol=0;symbol<sizeof(simple_bb_tokens)/sizeof(*simple_bb_tokens);symbol++){
        strcat(reg_str,simple_reg_str_first);
        strcat(reg_str,simple_bb_tokens[symbol]);
        strcat(reg_str,simple_reg_str_second);
        strcat(reg_str,simple_bb_tokens[symbol]);
        strcat(reg_str,simple_reg_str_third);
        printf("Regex: %s\n",reg_str);
        //Regex compilation
        if (regcomp(&regex,reg_str,REG_EXTENDED) != 0){
            return 2;
        }else{
            printf("Compiled successefully\n");
        }

        //Search for all instances of regex from left to right
        //Therefore to avoid problems, search should start from the last position on the previous match
        while(1){
            int reg_result = regexec(&regex, buffer,1,&m,0);
            if(reg_result!=REG_NOMATCH){
                int bbt_len = strlen(simple_bb_tokens[symbol]);
                printf("==============\n");
                printf("%s",reg_str);
                printf("%d %d :",m.rm_so,m.rm_eo);
                printf("%d\n",bbt_len);
                printf("%s\n",buffer);
                //Copy to buffer, the result of replacing bb_code with html, lenght is [¹\²BBT_LEN]³, 3 characters for ending token, 2 for beg([¹]²).
                str_replace(buffer,buffer+m.rm_eo-(bbt_len+3),bbt_len+3,simple_html_tokens_end[symbol]);
                str_replace(buffer,buffer+m.rm_so,bbt_len+2,simple_html_tokens_begin[symbol]);
                printf("=====RESULT ==\n");
                printf("%s",reg_str);
                printf("%d %d :",m.rm_so,m.rm_eo);
                printf("%s\n",buffer);
            }else{
                break;
            }
            //ASD[b]ASD[b]ASD[/b]asSDAAS
        }
        memset(reg_str,'\0',64);
        printf("loop_end");
    }
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
char *str_replace(char *str, const char *ptr, size_t ptr_len, const char *substr) {
    char *prestr = (char *)malloc(sizeof(char) * (strlen(str)+ptr_len+strlen(substr)));
    char *poststr = (char *)malloc(sizeof(char) * strlen(str));
    memset(prestr,'\0',sizeof(strlen(str)));
    memset(poststr,'\0',sizeof(strlen(str)));
    strncpy(prestr, str, (size_t)(ptr-str));
    strcpy(poststr, str + (size_t)(ptr - str + ptr_len));

    strcat(prestr, substr);
    strcat(prestr, poststr);

    strcpy(str,prestr);
    free(prestr);
    free(poststr);
    return NULL;
}