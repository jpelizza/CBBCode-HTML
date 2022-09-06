#include "cbbhtml.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int NUM_TAGS = 7;

char *bb_to_html(char *html, struct bb_config *conf) {

    const char *bb_tokens_begin[10] = {"[b]", "[i]", "[u]", "[s]", "[code]"};
    const char *bb_tokens_end[10] = {"[/b]", "[/i]", "[/u]", "[/s]", "[/code]"};
    const char *html_tokens_begin[10] = {"<strong>", "<em>", "<ins>", "<del>", "<pre>"};
    const char *html_tokens_end[10] = {"</strong>", "</em>", "</ins>", "</del>", "</pre>"};

    printf("%s\n", html);
    char *token_begin = (char *)malloc(sizeof(char) * 1028);
    char *token_end = (char *)malloc(sizeof(char) * 1028);
    char *delim_begin = (char *)malloc(sizeof(char) * 32);
    char *delim_end = (char *)malloc(sizeof(char) * 32);

    // first parse
    for (unsigned int tag = 0; tag < NUM_TAGS; tag++) {
        // setup delimitadores
        strcpy(delim_begin, bb_tokens_begin[tag]);
        strcpy(delim_end, bb_tokens_end[tag]);
        // get first token location, resulting token is strin up until

        token_begin = strstr(html, delim_begin);
        token_end = strstr(html + (int)strcspn(html, delim_begin), delim_end);
        while (token_begin != NULL && token_end != NULL) {
            printf("i- %s\n", token_begin);
            printf("e- %s\n", token_end);
            strcpy(html, str_replace(html, token_begin, strlen(delim_begin), html_tokens_begin[tag]));
            strcpy(html, str_replace(html, token_end, strlen(delim_end), html_tokens_end[tag]));
            token_begin = strstr(html, delim_begin);
            token_end = strstr(html + (int)strcspn(html, delim_begin), delim_end);
            break;
        }
    }
    printf("%s\n", html);

    return html;
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

    strncpy(prestr, str, (size_t)(ptr - str));
    strcpy(poststr, str + (size_t)(ptr - str + ptr_len));

    printf("- %s\n", prestr);
    printf("- %s\n", substr);
    printf("- %s\n", poststr);

    strcat(prestr, substr);
    strcat(prestr, poststr);
    printf("+ %s\n", prestr);

    return prestr;
}
