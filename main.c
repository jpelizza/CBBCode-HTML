#include "cbbhtml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char bbcode[][1024] = {"aaa[b]bbb[/b]cccccc",
						   "aaa[i]bbb[/i]ccc",
						   "aaa[u]bbb[/u]ccc",
						   "aaa[s]bbb[/s]ccc",
						   "aaa[center]bbb[/center]ccc",
						   "aaa[left]bbb[/left]ccc",
						   "aaa[right]bbb[/right]ccc",
						   "aaa[quote]bbb[/quote]ccccc",
						   "aaa[spoiler]bbb[/spoiler]ccc",
						   "aaa[code]bbb[/code]ccc",
						   "aaa[url=https://link.com]bbb[/url]ccc",
						   "aaa[url]bbb[/url]ccc",
						   "aaa[img]https://image.com/img.jpg[/img]ccc",
						   "aaa[img 120x320]https://image.com/img.jpg[/img]ccc",
						   "aaa[img width=320 height=120]https://image.com/img.jpg[/img]ccc",
						   "aaa[color=green]green[/color]ccc",
						   "aaa[color=#00AA00]green[/color]ccc"};

	printf("\n\n\n\n\n\n\n");
	for (int i = 15; i < 18; i++) {
		char *buf = NULL;
		printf("-%d-\n", bbcodetohtml_simple(bbcode[i], &buf));
		printf("Ori:%s mem:%d\n", bbcode[i]);
		printf("Str:%s mem:%d\n", buf, (void *)&buf);
		free(buf);
	}
}