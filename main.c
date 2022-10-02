#include "cbbhtml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char bbcode[][1024] = {"aaa[b]bbb[/b]ccc",		   "aaa[i]bbb[/i]ccc",			 "aaa[u]bbb[/u]ccc",
						   "aaa[s]bbb[/s]ccc",		   "aaa[center]bbb[/center]ccc", "aaa[left]bbb[/left]ccc",
						   "aaa[right]bbb[/right]ccc", "aaa[quote]bbb[/quote]ccc",	 "aaa[spoiler]bbb[/spoiler]ccc",
						   "aaa[url]bbb[/url]ccc",	   "aaa[code]bbb[/code]ccc",	 "aaa[img]bbb[/img]ccc"};
	for (int i = 0; i < 12; i++) {
		char *buf = NULL;
		printf("\n---\n");
		bbcodetohtml_simple(bbcode[i], &buf);
		printf("Ori:%s mem:%d\n", bbcode[i]);
		printf("Str:%s mem:%d\n", buf, (void *)&buf);
		printf("\n---\n");
		free(buf);
	}
}