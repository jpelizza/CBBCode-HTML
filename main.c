#include "cbbhtml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char bbcode[1024] = "vaaa[b]bbb[code]ccc[/b]d[/b]dd[/code]"
						"[img]https://IMGURL.com[/img]"
						"[url=https://THISISURL.com/image.jpg]hyperlink[/url]"
						"[color=#00AA00]hyperlink[/color]";
	char *buf;
	printf("%d\n", bbcodetohtml_simple(bbcode, &buf, -1));
	printf("\nstr:%s mem:%d\n", buf, (void *)&buf);
	printf("\nstr_size:%d \n", strlen(buf));
	printf("\nOri:%s mem:%d\n\n", bbcode);
}