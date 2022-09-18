#include "cbbhtml.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

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
int bbcodetohtml_simple(const char *bbcode, char **buffer, int buffer_size) {
	pcre2_code *regex;
	pcre2_match_data *matches = pcre2_match_data_create(16, NULL);

	PCRE2_SIZE erroffset, *m;
	int errorcode;

	// REGEX: \[(b|i|u|s|url|quote|code)\].*?\[\/\1\]
	// REGEX IMAGE: \[img\](.*?)\[\/img\]
	// REGEX URL= and COLOR=: \[((url|color)=)(#.+?|https?:\/\/.+?)\](.*)?\[\/\5\]
	PCRE2_SPTR pattern =
		"\\[(b|i|u|s|center|left|right|quote|spoiler|url|img|code|pre|youtube)\\].*?\\[\\/\\1\\]"; // 2,3
	//"\\[((url|color)=)(#[0-9A-F]{6}?|https?:\\/\\/.+?)\\](.*?)\\[\\/\\5\\]"; // 8,9,10,11,12,13,14,15

	const char bb_tokens[][16] = {"b", "i", "u", "s", "url", "quote", "code", "img", "url=", "color="};
	const char html_tokens_begin_open[][16] = {
		"<strong>",		"<em>",	 "<ins>",	   "<del>",		 "<a href\"#\">",
		"<blockquote>", "<pre>", "<img src='", "<a href=\"", "<div style=\"color:"};
	const char html_tokens_begin_close[][16] = {"", "", "", "", "", "", "", "/>", "\">", "\">"};
	const char html_tokens_end[][16] = {"</strong>",	 "</em>",  "</ins>", "</del>", "</a>",
										"</blockquote>", "</pre>", "",		 "</a>",   "</div>"};

	// MALLOC BUFFER
	{
		if (buffer_size == -1) {
			*buffer = (char *)malloc((sizeof(char) * strlen(bbcode)) + 1);
			buffer_size = strlen(bbcode) + 1;
		} else if (buffer_size < strlen(bbcode)) { // broken
			*buffer = (char *)realloc(*buffer, (sizeof(char) * strlen(bbcode)) + 1);
			buffer_size = strlen(bbcode) + 1;
		}
		if (*buffer == NULL || regex == NULL)
			return 1;
		strcpy(*buffer, bbcode);
	}
	// REGEX COMPILE
	regex = pcre2_compile(pattern, -1, 0, &errorcode, &erroffset, NULL);
	if (regex == NULL) {
		return 2;
	}

	// Search for all instances of regex from left to right
	while (pcre2_match(regex, *buffer, -1, 0, 0, matches, NULL) > 0) {
		m = pcre2_get_ovector_pointer(matches);

		int symbol, sp;
		char *subgroup;

		for (sp = 2; sp < 10; sp += 2) {
			if (m[sp] != -1) {
				subgroup = (char *)malloc(sizeof(char) * (m[sp + 1] - m[sp] + 1));
				memset(subgroup, '\0', m[sp + 1] - m[sp] + 1);
				strncpy(subgroup, *buffer + m[sp], m[sp + 1] - m[sp]);
				symbol = (sp / 2) - 1;
				break;
			}
		}

		size_t OPEN_SIZE = strlen(html_tokens_begin_open[symbol]);
		size_t CLOSE_SIZE = strlen(html_tokens_begin_close[symbol]);
		size_t END_SIZE = strlen(html_tokens_end[symbol]);
		size_t TAG_SIZE = strlen(bb_tokens[symbol]);
		// '5' comes from '[]'+'[/]' that are always present
		// 2*TAG since tag both opens and closes
		signed long REPLACE_SIZE = OPEN_SIZE + CLOSE_SIZE + END_SIZE - (5 + (2 * TAG_SIZE));
		// tmp_replacer_size = len(group_1)+REPLACE_SIZE
		int tmp_replacer_size = (m[1] - m[0]) + REPLACE_SIZE;
		char *tmp_replacer = (char *)malloc(sizeof(char) * (tmp_replacer_size));

		memset(tmp_replacer, '\0', tmp_replacer_size);
		strcpy(tmp_replacer, html_tokens_begin_open[7]);
		strncat(tmp_replacer, *buffer + m[4] + 4, (m[7] - 6) - m[5]);
		strcat(tmp_replacer, html_tokens_begin_close[7]);

		str_replace(buffer, &buffer_size, *buffer + m[1] - (TAG_SIZE + OPEN_SIZE + CLOSE_SIZE), TAG_SIZE + 3, "");
		str_replace(buffer, &buffer_size, *buffer + m[0], (TAG_SIZE + 2) + ((m[7] - 6) - m[5]), tmp_replacer);

		free(tmp_replacer);
		free(subgroup);
	}
	pcre2_match_data_free(matches);
	pcre2_code_free(regex);
	return 0;
}

/**
 * @brief Replaces on str, from ptr1 to ptr1+len with ptr2
 *
 * @param buf buffer string to have text replaced
 * @param ptr pointer to where replacing will take place
 * @param ptr_len lenght of word to be replaced
 * @param substr substring to take prt1's place
 */
void str_replace(char **buf, unsigned int *buf_size, const char *ptr, size_t ptr_len, const char *substr) {
	printf("%d %d\n", strlen(*buf), strlen(substr));
	char *prestr = (char *)malloc(sizeof(char) * (strlen(*buf) + strlen(substr)));
	char *poststr = (char *)malloc(sizeof(char) * strlen(*buf));
	memset(prestr, '\0', sizeof(strlen(*buf) + strlen(substr)));
	memset(poststr, '\0', sizeof(strlen(*buf)));

	strncpy(prestr, *buf, (size_t)(ptr - *buf));
	memset(prestr + (size_t)(ptr - *buf), '\0', 1);
	strcpy(poststr, *buf + (size_t)(ptr - *buf + ptr_len));

	strcat(prestr, substr);
	strcat(prestr, poststr);
	if (*buf_size <= strlen(prestr)) {
		*buf = (char *)realloc(*buf, (sizeof(char *) * strlen(prestr)) + 1); // I'm not sure I'm doing this correctly
		*buf_size = strlen(prestr);
	}
	strcpy(*buf, prestr);
	free(prestr);
	free(poststr);
}