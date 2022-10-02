#include "cbbhtml.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#define DEBUG 0

/**
 * @brief Writes translated bbcode into buffer;
 *
 * @param bbcode bbcode string to translate
 * @param buffer Address of an char array to write html
 * @param buffer_size send buffer size, -1 if not allocated memory
 * @return int return error code 0 if successeful
 * 1 if allocation error
 * 2 if regex compilation error
 */
int bbcodetohtml_simple(const char *bbcode, char **buffer) {
	pcre2_code *regex = NULL;
	pcre2_match_data *matches = pcre2_match_data_create(16, NULL);
	PCRE2_SIZE buffer_size = (*buffer == NULL) ? -1 : (PCRE2_SIZE)strlen(*buffer);
	PCRE2_SIZE erroffset, *m;
	int errorcode;
	if (DEBUG)
		printf("-2\n");
	// REGEX: \[(b|i|u|s|url|quote|code)\].*?\[\/\1\]
	// REGEX IMAGE: \[img\](.*?)\[\/img\]
	// REGEX URL= and COLOR=: \[((url|color)=)(#.+?|https?:\/\/.+?)\](.*)?\[\/\5\]
	PCRE2_SPTR pattern =
		"\\[(b|i|u|s|center|left|right|quote|spoiler|url|code|img)\\](.*?)\\[\\/\\1\\]|" // 2,3
		"\\[((url|color)=)(#[0-9A-F]{6}?|https?:\\/\\/.+?)\\](.*?)\\[\\/\\5\\]";		 // 8,9,10,11,12,13,14,15

	const char BB_TAGS[][16] = {"b",	 "i",	  "u",		 "s",	"center", "left",
								"right", "quote", "spoiler", "url", "code",	  "img"};
	const char HTML_OPEN[][64] = {"<strong>",
								  "<em>",
								  "<ins>",
								  "<del>",
								  "<div style=\"text-align:center\">",
								  "<div style=\"text-align:left\">",
								  "<div style=\"text-align:right\">",
								  "<quoteblock>",
								  "<span class=\"spoiler\">",
								  "<a href=\"#\">",
								  "<code>",
								  "<img src="};
	const char HTML_CLOSE[][16] = {"", "", "", "", "", "", "", "", "", "", "", ">"};
	const char HTML_END[][16] = {"</strong>", "</em>",		   "</ins>",  "</del>", "</div>",  "</div>",
								 "</div>",	  "</quoteblock>", "</span>", "</a>",	"</code>", ""};
	if (DEBUG)
		printf("-1.5\n");
	// MALLOC BUFFER
	if (buffer_size == -1) {
		if (DEBUG)
			printf("buffer = malloc(%d)\n", (sizeof(char) * (strlen(bbcode) + 1)));
		*buffer = (char *)malloc(sizeof(char) * (strlen(bbcode) + 1));
		buffer_size = strlen(bbcode);
	}
	if (DEBUG)
		printf("-1.75\n");
	// Sanity check

	if (*buffer == NULL)
		return 1;
	if (DEBUG)
		printf("-1\n");
	// Copies bbcode to buffer for editing;
	strcpy(*buffer, bbcode);

	// REGEX COMPILE
	// printf("*buffer=%s, strlen:%d\n", *buffer, strlen(*buffer));
	regex = pcre2_compile(pattern, -1, 0, &errorcode, &erroffset, NULL);
	// printf("*buffer=%s, strlen:%d\n ", *buffer, strlen(*buffer));
	if (regex == NULL) {
		return 2;
	}
	if (DEBUG)
		printf("0\n");

	// While there is at least a match of the regex on buffer
	while (pcre2_match(regex, *buffer, -1, 0, 0, matches, NULL) > 0) {
		m = pcre2_get_ovector_pointer(matches);
		int symbol, sp;
		char *subgroup;

		// Search for all instances of regex from left to right
		for (sp = 2; sp < 24; sp += 2) {
			if (m[sp] != -1) {
				subgroup = (char *)malloc(sizeof(char) * (m[sp + 1] - m[sp] + 1));
				memset(subgroup, '\0', m[sp + 1] - m[sp] + 1);
				strncpy(subgroup, *buffer + m[sp], m[sp + 1] - m[sp]);
				for (symbol = 0; symbol < 12; symbol++) {
					if (DEBUG)
						printf("%s == %s\n", BB_TAGS[symbol], subgroup);
					if (!strcmp(BB_TAGS[symbol], subgroup))
						break;
				}
				break;
			}
		}
		if (DEBUG)
			printf("1\n");
		size_t OPEN_SIZE = strlen(HTML_OPEN[symbol]);
		size_t CLOSE_SIZE = strlen(HTML_CLOSE[symbol]);
		size_t END_SIZE = strlen(HTML_CLOSE[symbol]);
		size_t TAG_SIZE = strlen(BB_TAGS[symbol]);
		// '5' comes from '[]'+'[/]' that are always present
		// 2*TAG since tag both opens and closes
		signed long REPLACE_SIZE = OPEN_SIZE + CLOSE_SIZE + END_SIZE + 5 - (2 * TAG_SIZE);
		// tmp_replacer_size = len(group_1)+REPLACE_SIZE
		int tmp_replacer_size = (m[1] - m[0]) + REPLACE_SIZE;
		printf("m[1]=%d m[2]=%d REPLACE_SIZE=%d\n", m[1], m[0], REPLACE_SIZE);
		char *tmp_replacer = (char *)malloc(sizeof(char) * (tmp_replacer_size));

		// REPLACING FOR SIMPLE
		// TO ADAPT THIS SIMPLY CHANGE [2] and [3]
		// TO [RE_MULT] and [RE_MULT+1]
		// WHERE RE_MULT is on a for loop looking for which m[X] != -1;
		memset(tmp_replacer, '\0', tmp_replacer_size);
		strcpy(tmp_replacer, HTML_OPEN[symbol]);
		printf("m[2]:%d\n", m[2]);
		printf("str: %s\n", *buffer + m[2]);
		strncat(tmp_replacer, *buffer + m[2] + TAG_SIZE, (m[3] - m[2]) - (TAG_SIZE));
		strncat(tmp_replacer, *buffer + m[3] + 1, m[1] - (m[3] + TAG_SIZE + 4));
		strcat(tmp_replacer, HTML_CLOSE[symbol]);
		strcat(tmp_replacer, HTML_END[symbol]);

		if (DEBUG)
			printf("2, tmp_replacer: %s\n, tmp_reaplcer_size: %d\n", tmp_replacer, tmp_replacer_size);
		PCRE2_UCHAR output[1024] = "";
		PCRE2_SIZE outlen = sizeof(output) / sizeof(PCRE2_UCHAR);

		if (DEBUG)
			printf("2.5\n");
		printf("%s | %d | %d\n", *buffer, strlen(*buffer), buffer_size);
		printf("%s | %d | %d\n", tmp_replacer, strlen(tmp_replacer), tmp_replacer_size);
		pcre2_substitute(regex,						// code
						 *buffer,					// subject string
						 PCRE2_ZERO_TERMINATED,		// subject string len
						 0,							// starting offset
						 PCRE2_SUBSTITUTE_EXTENDED, // options
						 NULL,						// match data
						 NULL,						// mcontext
						 tmp_replacer,				// string to replace matches
						 PCRE2_ZERO_TERMINATED,		// size of strin to replace
						 output,					// buffer
						 &outlen					// buffer size
		);
		if (DEBUG)
			printf("3\n");
		strcpy(*buffer, output);
		memset(output, '\0', outlen);
		free(tmp_replacer);
		free(subgroup);
		if (DEBUG)
			printf("4\n");
	}
	if (DEBUG)
		printf("5\n");
	// pcre2_code_free(regex);
	if (DEBUG)
		printf("5.5\n");
	pcre2_match_data_free(matches);
	if (DEBUG)
		printf("6\n");
	return 0;
}
