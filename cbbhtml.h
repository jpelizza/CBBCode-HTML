#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct bb_config {
    char *emoticons_text;
    char *emoticons_url;
};

char *bb_to_html(char *html, struct bb_config *conf);
char *str_replace(const char *str, const char *ptr, size_t ptr_len, const char *substr);