#include "cbbhtml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char bbcode[1024] =
		"ASsdfsdfD[b]ASDasdasd[code]ASDasdasdasdasd[/b]asSDAdsfsdfsAS[/code]"
		"[img]https://IMAGEURL.jpg[/img]"
		"[color=#00AA00]https://IMAGEURL.jpg[/color]"
		"[url=https://external-content.duckduckgo.com/iu/"
		"?u=https%3A%2F%2Ftse1.mm.bing.net%2Fth%3Fid%3DOIP.ljYL65wDRmi1J9CBWthjxAHaE8%26pid%3DApi&f=1]asdasdasd[/url]";
	char *buf;
	printf("%d\n", bbcodetohtml_simple(bbcode, &buf, -1));
	printf("\nstr:%s mem:%d\n", buf, (void *)&buf);
	printf("\nOri:%s mem:%d\n\n", bbcode);
}