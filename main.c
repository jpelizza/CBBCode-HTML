#include "cbbhtml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char bbcode[1024] = "ASsdfsdfD[b]ASDasdasd[code]ASDasdasdasdasd[/b]asSDAdsfsdfsAS[/code]";
	char *buf;
	printf("%d\n", bbcodetohtml_simple(bbcode, &buf, -1));
	printf("\nstr:%s mem:%d\n\n", buf, (void *)&buf);
}