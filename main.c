#include "cbbhtml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char bbcode[1024] = "aaa[b]bbb[/b]ccc"
						"aaa[code]bbb[/code]ccc";
	char *buf;
	printf("%d\n", bbcodetohtml_simple(bbcode, &buf));
	printf("\nstr:%s mem:%d\n", buf, (void *)&buf);
	printf("\nstr_size:%d \n", strlen(buf));
	printf("\nOri:%s mem:%d\n\n", bbcode);
}