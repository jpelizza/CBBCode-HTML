#include "cbbhtml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char bbcode[1024] = "ASD[b]ASD[code]ASD[/b]asSDAAS[/code]";
    char *buf;
    bbcodetohtml(bbcode,*buf,-1);
}