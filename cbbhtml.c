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
	PCRE2_SPTR pattern = "\\[(b|i|u|s|url|quote|code)\\].*?\\[\\/\\1\\]|"				 // 2,3
						 "\\[(img)\\].*?\\[(\\/img)\\]|"								 // 4,5,6,7
						 "\\[((url|color)=)(#.+?|https?:\\/\\/.+?)\\](.*?)\\[\\/\\5\\]"; // 8,9,10,11,12,13,14,15

	const char bb_tokens[][32] = {"b", "i", "u", "s", "url", "quote", "code", "img", "url=", "color="};
	const char html_tokens_begin[][32] = {"<strong>",	  "<em>",  "<ins>",		 "<del>",	   "<a href\"#\">",
										  "<blockquote>", "<pre>", "<img src='", "<a href=\"", "<div style=\"color:"};
	const char html_tokens_end[][32] = {"</strong>",	 "</em>",  "</ins>", "</del>", "</a>",
										"</blockquote>", "</pre>", "'/>",	 "\">",	   "\">"};

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

	regex = pcre2_compile(pattern, -1, 0, &errorcode, &erroffset, NULL);
	if (regex == NULL) {
		return 2;
	}

	// Search for all instances of regex from left to right
	while (pcre2_match(regex, *buffer, -1, 0, 0, matches, NULL) > 0) {
		m = pcre2_get_ovector_pointer(matches);
		for (int i = 0; i < 16; i++) {
			printf("[%d] - %d\n", i, m[i]);
			printf("%s\n", *buffer + m[i]);
		}

		int symbol;
		char *subgroup;
		for (int i = 2; i < 10; i += 2) {
			if (m[i] != -1) {
				subgroup = (char *)malloc(sizeof(char) * (m[i + 1] - m[i] + 1));
				memset(subgroup, '\0', m[i + 1] - m[i] + 1);
				strncpy(subgroup, *buffer + m[i], m[i + 1] - m[i]);
				break;
			}
		}
		printf("sub: %s\n", subgroup);

		for (symbol = 0; symbol < 7; symbol++) {
			int bbt_len = strlen(bb_tokens[symbol]);
			// REPLACES FOR: "b", "i", "u", "s", "url", "quote", "code"
			// 2,3
			if (!strcmp(subgroup, bb_tokens[symbol])) {
				printf("::1::\n");
				str_replace(buffer, &buffer_size, *buffer + m[1] - (bbt_len + 3), bbt_len + 3, html_tokens_end[symbol]);
				str_replace(buffer, &buffer_size, *buffer + m[0], bbt_len + 2, html_tokens_begin[symbol]);
				break;
			}
		}
		for (symbol = 7; symbol < 8; symbol++) {
			int bbt_len = strlen(bb_tokens[symbol]);
			// REPLACES FOR: "img"
			if (!strcmp(subgroup, bb_tokens[symbol])) {
				printf("::2::\n");
				int tmp_replacer_size = (m[1] - m[0]) + (strlen(html_tokens_begin[7]) + strlen(html_tokens_end[7]));
				char *tmp_replacer = (char *)malloc(sizeof(char) * (tmp_replacer_size));
				memset(tmp_replacer, '\0', tmp_replacer_size);
				strcpy(tmp_replacer, html_tokens_begin[7]);
				strncat(tmp_replacer, *buffer + m[4] + 4, (m[7] - 6) - m[5]);
				strcat(tmp_replacer, html_tokens_end[7]);
				str_replace(buffer, &buffer_size, *buffer + m[1] - (bbt_len + 3), bbt_len + 3, "");
				str_replace(buffer, &buffer_size, *buffer + m[0], (bbt_len + 2) + ((m[7] - 6) - m[5]), tmp_replacer);
				free(tmp_replacer);
				continue;
			}
		}
		// REPLACE FOR "URL=" and "color="
		//[5] 8,9,
		//[6]10,11,
		//[7]12,13
		//[8]14,15
		// REGEX URL= and COLOR=: \[((url|color)=)(#.+?|https?:\/\/.+?)\](.*)?\[\/\5\]
		//						   8|10       11|9|12                13 |14|15
		if (!strcmp(subgroup, bb_tokens[8])) {
			int symbol = 8;
			int tmp_replacer_size = (8 + (m[13] - m[12]) + (m[15] - m[14]));
			char *tmp_replacer = (char *)malloc(sizeof(char) * tmp_replacer_size);
			memset(tmp_replacer, '\0', tmp_replacer_size);

			strcpy(tmp_replacer, html_tokens_begin[symbol]);
			strncat(tmp_replacer, *buffer + m[12], (m[13] - m[12]));
			strcat(tmp_replacer, html_tokens_end[symbol]);

			str_replace(buffer, &buffer_size, *buffer + m[1] - 6, 6, "</a>");
			str_replace(buffer, &buffer_size, *buffer + m[0], (m[13] - m[12]) + 6, tmp_replacer);
			free(tmp_replacer);
		} else if (!strcmp(subgroup, bb_tokens[9])) {
			int symbol = 9;
			int tmp_replacer_size = (8 + (m[13] - m[12]) + (m[15] - m[14]));
			char *tmp_replacer = (char *)malloc(sizeof(char) * tmp_replacer_size);
			memset(tmp_replacer, '\0', tmp_replacer_size);

			strcpy(tmp_replacer, html_tokens_begin[symbol]);
			strncat(tmp_replacer, *buffer + m[12], (m[13] - m[12]));
			strcat(tmp_replacer, html_tokens_end[symbol]);
			str_replace(buffer, &buffer_size, *buffer + m[1] - 8, 8, "</div>");
			str_replace(buffer, &buffer_size, *buffer + m[0], (m[13] - m[12]) + 8, tmp_replacer);
			free(tmp_replacer);
		}
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