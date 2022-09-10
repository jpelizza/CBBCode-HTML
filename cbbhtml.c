#include "cbbhtml.h"

#include <assert.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	regex_t regex;
	regmatch_t m[2];

	char bb_tokens[][16] = {"0", "b", "i", "u", "s", "url", "quote", "code"};
	char html_tokens_begin[][16] = {"0",	 "<strong>",	  "<em>",		  "<ins>",
									"<del>", "<a href\"#\">", "<blockquote>", "<pre>"};
	char html_tokens_end[][16] = {"0", "</strong>", "</em>", "</ins>", "</del>", "</a>", "</blockquote>", "</pre>"};
	// REGEX: \[(b|i|u|s|url|quote|code)\].*?\[\/\1\]
	const char *reg_str = "\\[(b|i|u|s|url|quote|code)\\].*?\\[\\/\\1\\]";

	if (buffer_size == -1) {
		*buffer = (char *)malloc((sizeof(char) * strlen(bbcode)) + 1);
		buffer_size = strlen(bbcode) + 1;
	} else if (buffer_size < strlen(bbcode)) { // broken
		*buffer = (char *)realloc(*buffer, (sizeof(char) * strlen(bbcode)) + 1);
		buffer_size = strlen(bbcode) + 1;
	}
	if (*buffer == NULL || reg_str == NULL)
		return 1;

	strcpy(*buffer, bbcode);

	if (regcomp(&regex, reg_str, REG_EXTENDED) != 0) {
		return 2;
	}

	// Search for all instances of regex from left to right
	while (regexec(&regex, *buffer, 2, m, 0) != REG_NOMATCH) {
		int symbol;
		char *subgroup = (char *)malloc(sizeof(char) * (m[1].rm_eo - m[1].rm_so + 1));
		memset(subgroup, '\0', m[1].rm_eo - m[1].rm_so + 1);
		strncpy(subgroup, *buffer + m[1].rm_so, m[1].rm_eo - m[1].rm_so);

		for (symbol = 1; symbol < 8; symbol++) {
			if (!strcmp(subgroup, bb_tokens[symbol]))
				break;
		}
		int bbt_len = strlen(bb_tokens[symbol]);
		str_replace(buffer, &buffer_size, *buffer + m[0].rm_eo - (bbt_len + 3), bbt_len + 3, html_tokens_end[symbol]);
		str_replace(buffer, &buffer_size, *buffer + m[0].rm_so, bbt_len + 2, html_tokens_begin[symbol]);
	}
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
char *str_replace(char **buf, unsigned int *buf_size, const char *ptr, size_t ptr_len, const char *substr) {
	char *prestr = (char *)malloc(sizeof(char) * (strlen(*buf) + ptr_len + strlen(substr)));
	char *poststr = (char *)malloc(sizeof(char) * strlen(*buf));
	memset(prestr, '\0', sizeof(strlen(*buf)));
	memset(poststr, '\0', sizeof(strlen(*buf)));

	strncpy(prestr, *buf, (size_t)(ptr - *buf));
	strcpy(poststr, *buf + (size_t)(ptr - *buf + ptr_len));

	strcat(prestr, substr);
	strcat(prestr, poststr);

	if (*buf_size < strlen(prestr)) {
		*buf = (char *)realloc(*buf, (sizeof(char *) * strlen(prestr)) + 1);
		*buf_size = strlen(prestr) + 1;
	}

	strcpy(*buf, prestr);
	free(prestr);
	free(poststr);

	return *buf;
}