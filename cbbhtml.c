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

	const char bb_tokens[][16] = {"b", "i", "u", "s", "url", "quote", "code"};
	const char html_tokens_begin[][16] = {"<strong>",	   "<em>",		   "<ins>", "<del>",
										  "<a href\"#\">", "<blockquote>", "<pre>"};
	const char html_tokens_end[][16] = {"</strong>", "</em>", "</ins>", "</del>", "</a>", "</blockquote>", "</pre>"};
	// REGEX: \[(b|i|u|s|url|quote|code)\].*?\[\/\1\]c
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

		for (symbol = 0; symbol < 7; symbol++) {
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
void str_replace(char **buf, unsigned int *buf_size, const char *ptr, size_t ptr_len, const char *substr) {
	char *prestr = (char *)malloc(sizeof(char) * (strlen(*buf) + (strlen(substr))));
	char *poststr = (char *)malloc(sizeof(char) * strlen(*buf));
	memset(prestr, '\0', sizeof((strlen(*buf) + (strlen(substr)))));
	memset(poststr, '\0', sizeof(strlen(*buf)));

	strncpy(prestr, *buf, (size_t)(ptr - *buf));
	memset(prestr + (size_t)(ptr - *buf), '\0', 1);
	strcpy(poststr, *buf + (size_t)(ptr - *buf + ptr_len));

	strcat(prestr, substr);
	strcat(prestr, poststr);
	if (*buf_size <= strlen(prestr)) {
		/*
			R: 75 -- Reallocs to 75 correctly
			R: 80 -- Reallocs to 80 correctly
			R: 112 -- DOES NOT Reallocs to 112 correctly :(
		 */
		*buf = (char *)realloc(*buf, (sizeof(char *) * strlen(prestr)) + 1); // I'm not sure I'm doing this correctly
		*buf_size = strlen(prestr);
		// ERROR: mremap_chunk(): invalid pointer
	}

	strcpy(*buf, prestr);
	free(prestr);
	free(poststr);
}