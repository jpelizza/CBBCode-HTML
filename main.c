#include "cbbhtml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char *test;
    test = (char *)malloc(sizeof(char) * 50);
    strcpy(test, "asd[b]Text[/b]asd[i]Text[/i]\n");

    struct bb_config conf;
    bb_to_html(test, NULL);
    return 0;
}