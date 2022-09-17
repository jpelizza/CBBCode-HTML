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
	regmatch_t m[10];

	const char bb_tokens[][16] = {"b", "i", "u", "s", "url", "quote", "code", "img"};
	const char html_tokens_begin[][16] = {"<strong>",	   "<em>",		   "<ins>", "<del>",
										  "<a href\"#\">", "<blockquote>", "<pre>", "<img src='"};
	const char html_tokens_end[][16] = {"</strong>", "</em>",		  "</ins>", "</del>",
										"</a>",		 "</blockquote>", "</pre>", "' />"};
	// REGEX: \[(b|i|u|s|url|quote|code)\].*?\[\/\1\]
	// REGEX IMAGE: \[img\](.*?)\[\/img\]
	// REGEX URL=: \[url=(https?:\/\/.+?)\](.*?)\[\/url\]
	const char *reg_str = "\\[(b|i|u|s|url|quote|code)\\].*?\\[\\/\\1\\]|" // matches 1
						  "\\[(img)\\].*?\\[(\\/img)\\]|"				   // matches 2 and 3
						  "\\[url=(https?:\\/\\/.+?)\\](.*)?\\[\\/url\\]"; // matches 4 and 5

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
	while (regexec(&regex, *buffer, 10, m, 0) != REG_NOMATCH) {
		int symbol;
		char *subgroup;
		for (int i = 1; i < 10; i++) {
			if (m[i].rm_eo != -1) {
				subgroup = (char *)malloc(sizeof(char) * (m[i].rm_eo - m[i].rm_so + 1));
				memset(subgroup, '\0', m[i].rm_eo - m[i].rm_so + 1);
				strncpy(subgroup, *buffer + m[i].rm_so, m[i].rm_eo - m[i].rm_so);
			}
		}

		for (symbol = 0; symbol < 8; symbol++) {
			int bbt_len = strlen(bb_tokens[symbol]);
			// REPLACES FOR: "b", "i", "u", "s", "url", "quote", "code"
			if (!strcmp(subgroup, bb_tokens[symbol])) {
				str_replace(buffer, &buffer_size, *buffer + m[0].rm_eo - (bbt_len + 3), bbt_len + 3,
							html_tokens_end[symbol]);
				str_replace(buffer, &buffer_size, *buffer + m[0].rm_so, bbt_len + 2, html_tokens_begin[symbol]);
				break;
			}
			// REPLACES FOR: "img"
			else if (!strcmp(subgroup, bb_tokens[symbol])) {
				int tmp_replacer_size =
					(m[1].rm_eo - m[1].rm_so + strlen(html_tokens_begin[7]) + strlen(html_tokens_end[7]) + 1);
				char *tmp_replacer = (char *)malloc(sizeof(char) * tmp_replacer_size);
				memset(tmp_replacer, '\0', tmp_replacer_size);
				strcpy(tmp_replacer, html_tokens_begin[7]);
				strncat(tmp_replacer, *buffer + m[2].rm_so + 4, (m[3].rm_eo - 6) - m[2].rm_eo);
				strcat(tmp_replacer, html_tokens_end[7]);

				str_replace(buffer, &buffer_size, *buffer + m[0].rm_eo - (bbt_len + 3), bbt_len + 3, "");
				str_replace(buffer, &buffer_size, *buffer + m[0].rm_so, (bbt_len + 2) + ((m[3].rm_eo - 6) - m[2].rm_eo),
							tmp_replacer);
				free(tmp_replacer);
				break;
			}
			// REPLACE FOR URL=
			else if (m[4].rm_eo != -1) {
				int tmp_replacer_size = (16 + (m[4].rm_eo - m[4].rm_so) + (m[5].rm_eo - m[5].rm_so));
				char *tmp_replacer = (char *)malloc(sizeof(char) * tmp_replacer_size);
				memset(tmp_replacer, '\0', tmp_replacer_size);
				strcpy(tmp_replacer, "<a href=\"");
				strncat(tmp_replacer, *buffer + m[4].rm_so, (m[4].rm_eo - m[4].rm_so));
				strcat(tmp_replacer, "\">");
				str_replace(buffer, &buffer_size, *buffer + m[0].rm_eo - 6, 6, "</a>");
				str_replace(buffer, &buffer_size, *buffer + m[0].rm_so, (m[4].rm_eo - m[4].rm_so) + 6, tmp_replacer);
				free(tmp_replacer);
				break;
			}
			// REPLACE FOR LIST
		}
		free(subgroup);
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
	char *prestr = (char *)malloc(sizeof(char) * (strlen(*buf) + (strlen(substr)) + 1));
	char *poststr = (char *)malloc(sizeof(char) * strlen(*buf) + 1);
	memset(prestr, '\0', sizeof((strlen(*buf) + (strlen(substr)))) + 1);
	memset(poststr, '\0', sizeof(strlen(*buf)) + 1);

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