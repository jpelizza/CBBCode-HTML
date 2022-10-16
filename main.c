#include "cbbhtml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char bbcode[][1024] = {"aaa[b]bbb[/b]ccc",
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
						   "aaa[color=#00AA00]green[/color]ccc",
						   "aaa[size=11]size 11 px[/size]ccc"};
	char bbcode_huge[]="aaa[b]bbb[/b]ccc\n"
						   "aaa[i]bbb[/i]ccc\n"
						   "aaa[u]bbb[/u]ccc\n"
						   "aaa[s]bbb[/s]ccc\n"
						   "aaa[center]bbb[/center]ccc\n"
						   "aaa[left]bbb[/left]ccc\n"
						   "aaa[right]bbb[/right]ccc\n"
						   "aaa[quote]bbb[/quote]ccccc\n"
						   "aaa[spoiler]bbb[/spoiler]ccc\n"
						   "aaa[code]bbb[/code]ccc\n"
						   "aaa[url=https://link.com]bbb[/url]ccc\n"
						   "aaa[url]bbb[/url]ccc\n"
						   "aaa[img]https://static.stacker.com/s3fs-public/styles/slide_desktop/s3/2019-03/106.webp[/img]ccc\n"
						   "aaa[img 120x320]https://static.stacker.com/s3fs-public/styles/slide_desktop/s3/2019-03/106.webp[/img]ccc\n"
						   "aaa[img width=320 height=120]https://static.stacker.com/s3fs-public/styles/slide_desktop/s3/2019-03/106.webp[/img]ccc\n"
						   "aaa[color=green]green[/color]ccc\n"
						   "aaa[color=#00AA00]green[/color]ccc\n"
						   "aaa[size=11]size 11 px[/size]ccc\n"
						   "aaa[b]bbb[/b]ccc\n"
						   "aaa[i]bbb[/i]ccc\n"
						   "aaa[u]bbb[/u]ccc\n"
						   "aaa[s]bbb[/s]ccc\n"
						   "aaa[center]bbb[/center]ccc\n"
						   "aaa[left]bbb[/left]ccc\n"
						   "aaa[right]bbb[/right]ccc\n"
						   "aaa[quote]bbb[/quote]ccccc\n"
						   "aaa[spoiler]bbb[/spoiler]ccc\n"
						   "aaa[code]bbb[/code]ccc\n"
						   "aaa[url=https://link.com]bbb[/url]ccc\n"
						   "aaa[url]bbb[/url]ccc\n"
						   "aaa[img]https://image.com/img.jpg[/img]ccc\n"
						   "aaa[img 120x320]https://image.com/img.jpg[/img]ccc\n"
						   "aaa[img width=320 height=120]https://image.com/img.jpg[/img]ccc\n"
						   "aaa[color=green]green[/color]ccc\n"
						   "aaa[color=#00AA00]green[/color]ccc\n"
						   "aaa[size=11]size 11 px[/size]ccc\n"
						   "aaa[b]bbb[/b]ccc\n"
						   "aaa[i]bbb[/i]ccc\n"
						   "aaa[u]bbb[/u]ccc\n"
						   "aaa[s]bbb[/s]ccc\n"
						   "aaa[center]bbb[/center]ccc\n"
						   "aaa[left]bbb[/left]ccc\n"
						   "aaa[right]bbb[/right]ccc\n"
						   "aaa[quote]bbb[/quote]ccccc\n"
						   "aaa[spoiler]bbb[/spoiler]ccc\n"
						   "aaa[code]bbb[/code]ccc\n"
						   "aaa[url=https://link.com]bbb[/url]ccc\n"
						   "aaa[url]bbb[/url]ccc\n"
						   "aaa[img]https://image.com/img.jpg[/img]ccc\n"
						   "aaa[img 120x320]https://image.com/img.jpg[/img]ccc\n"
						   "aaa[img width=320 height=120]https://image.com/img.jpg[/img]ccc\n"
						   "aaa[color=green]green[/color]ccc\n"
						   "aaa[color=#00AA00]green[/color]ccc\n"
						   "aaa[size=11]size 11 px[/size]ccc\n";

		printf("\n\n\n\n\n\n\n");
		char *buf = NULL;
		printf("-%d-\n", bbcodetohtml_simple(bbcode_huge, &buf));
		printf("Str:%s mem:%d\n", buf, (void *)&buf);
		printf("buf size: %d\n",strlen(buf));
		free(buf);
		return;
		for (int i = 0; i < 18; i++) {
			printf("-%d-\n", bbcodetohtml_simple(bbcode[i], &buf));
			printf("Ori:%s mem:%d\n", bbcode[i]);
			printf("Str:%s mem:%d\n", buf, &buf);
			free(buf);
	}
}