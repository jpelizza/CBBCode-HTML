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
int bbcodetohtml_simple(const char *bbcode, char **buffer) {
	pcre2_code *regex = NULL;
	pcre2_match_data *matches = pcre2_match_data_create(32, NULL);
	PCRE2_SIZE buffer_size = (*buffer == NULL) ? -1 : (PCRE2_SIZE)strlen(*buffer);
	PCRE2_SIZE erroffset, *m;
	int errorcode;
	// REGEX: \[(b|i|u|s|url|quote|code)\].*?\[\/\1\]
	// REGEX IMAGE: \[img\](.*?)\[\/img\]
	// REGEX URL= and COLOR=: \[((url|color)=)(#.+?|https?:\/\/.+?)\](.*)?\[\/\5\]
	// REGEX COLOR [color=green]text[/color] [color=#\d{6}]text[/color]
	PCRE2_SPTR pattern =
		"\\[(b|i|u|s|center|left|right|quote|spoiler|code)\\](.*?)\\[\\/\\1\\]|" // (2,3)(4,5)
		"\\[(url)(=https?:\\/\\/.+?)?\\](.*?)\\[\\/url\\]|"						 // (6,7)(8,9)(10,11)
		"\\[(img)(\\s+(\\d+)x(\\d+)|\\s+width=(\\d+)\\s+height=(\\d+))?\\](.*)\\[\\/img\\]|" // (12,13)(14,15)(16,17)(18,19)(20,21)(22,23)(24,25)
		"\\[(color)=(red|green|blue|\\#[\\dA-F]{6})\\](.*)\\[\\/color\\]|" // (26,27)(28,29)(30,31)
		"\\[(size)=(\\d*)\\](.*)\\[\\/size\\]";							   // (32,33)(34,35)(36,37)

	const char BB_TAGS[][64] = {"b",	 "i",		"u",   "s",	   "center", "left",  "right",
								"quote", "spoiler", "url", "code", "img",	 "color", "size"};
	const char HTML_OPEN[][64] = {"<strong>",
								  "<em>",
								  "<ins>",
								  "<del>",
								  "<div style=\"text-align:center\">",
								  "<div style=\"text-align:left\">",
								  "<div style=\"text-align:right\">",
								  "<quoteblock>",
								  "<span class=\"spoiler\">",
								  "<a href=\"",
								  "<code>",
								  "<img src=\"",
								  "<div style=\"color:",
								  "<div style=\"font-size:"};
	const char HTML_CLOSE[][64] = {"", "", "", "", "", "", "", "", "", "\">", "", ">", ";\">", "px;\">"};
	const char HTML_END[][64] = {"</strong>",	  "</em>",	 "</ins>", "</del>",  "</div>", "</div>", "</div>",
								 "</quoteblock>", "</span>", "</a>",   "</code>", "",		"</div>", "</div>"};
	// MALLOC BUFFER
	if (buffer_size == -1) {
		*buffer = (char *)malloc(sizeof(char) * (strlen(bbcode) + 1));
		buffer_size = strlen(bbcode);
	}
	// Sanity check

	if (*buffer == NULL)
		return 1;
	// Copies bbcode to buffer for editing;
	strcpy(*buffer, bbcode);

	// REGEX COMPILE
	// printf("*buffer=%s, strlen:%d\n", *buffer, strlen(*buffer));
	regex = pcre2_compile(pattern, -1, 0, &errorcode, &erroffset, NULL);
	// printf("*buffer=%s, strlen:%d\n ", *buffer, strlen(*buffer));
	if (regex == NULL) {
		return 2;
	}

	// While there is at least a match of the regex on buffer
	while (pcre2_match(regex, *buffer, -1, 0, 0, matches, NULL) > 0) {
		m = pcre2_get_ovector_pointer(matches);
		int symbol, sp;
		char *subgroup = NULL;
		// Search for all instances of regex from left to right
		for (sp = 2; sp < 38; sp += 2) {
			if (m[sp] != -1) {
				subgroup = (char *)malloc(sizeof(char) * (m[sp + 1] - m[sp] + 1));
				memset(subgroup, '\0', m[sp + 1] - m[sp] + 1);
				strncpy(subgroup, *buffer + m[sp], m[sp + 1] - m[sp]);
				for (symbol = 0; symbol < 13; symbol++) {
					if (!strcmp(BB_TAGS[symbol], subgroup))
						break;
				}
				break;
			}
		}
		for (sp = 2; sp < 38; sp += 2) {
			if (m[sp] != -1)
				break;
		}
		size_t OPEN_SIZE = strlen(HTML_OPEN[symbol]);
		size_t CLOSE_SIZE = strlen(HTML_CLOSE[symbol]);
		size_t END_SIZE = strlen(HTML_CLOSE[symbol]);
		size_t TAG_SIZE = strlen(BB_TAGS[symbol]);
		// '5' comes from '[]'+'[/]' that are always present
		// 2*TAG since tag both opens and closes
		signed long REPLACE_SIZE = OPEN_SIZE + CLOSE_SIZE + END_SIZE + 5 - (2 * TAG_SIZE);
		// tmp_replacer_size = len(group_1)+REPLACE_SIZE
		int tmp_replacer_size = (m[1] - m[0]) + REPLACE_SIZE + 16;
		char *tmp_replacer = (char *)malloc(sizeof(char) * (tmp_replacer_size));

		// REPLACING FOR SIMPLE
		// TO ADAPT THIS SIMPLY CHANGE [2] and [3]
		// TO [RE_MULT] and [RE_MULT+1]
		// WHERE RE_MULT is on a for loop looking for which m[X] != -1;
		memset(tmp_replacer, '\0', tmp_replacer_size);

		if (sp == 2) {
			strcpy(tmp_replacer, HTML_OPEN[symbol]);
			strncat(tmp_replacer, *buffer + m[sp + 1] + 1, m[1] - (m[sp + 1] + TAG_SIZE + 4));
			strcat(tmp_replacer, HTML_CLOSE[symbol]);
			strcat(tmp_replacer, HTML_END[symbol]);
		} else if (sp == 6) {
			//\[(url)(=https?:\/\/.+?)\](.*?)\[\/\3\]
			// (6,7)       (8,9)        (10,11)
			//"aaa[url=https://link.com]bbb[/url]ccc"
			//"aaa[url]bbb[/url]ccc"
			if (m[8] == -1) { // parsing [url]
				strcpy(tmp_replacer, HTML_OPEN[symbol]);
				strcat(tmp_replacer, "#");
				strcat(tmp_replacer, HTML_CLOSE[symbol]);
				strncat(tmp_replacer, *buffer + m[sp + 1] + 1, m[1] - (m[sp + 1] + TAG_SIZE + 4));
				strcat(tmp_replacer, HTML_END[symbol]);
			} else { // parsing [url=]
				strcpy(tmp_replacer, HTML_OPEN[symbol]);
				strncat(tmp_replacer, *buffer + m[sp + 2] + 1, (m[sp + 3] - m[sp + 2]) - 1);
				strcat(tmp_replacer, HTML_CLOSE[symbol]);
				strncat(tmp_replacer, *buffer + m[sp + 4], (m[sp + 5] - m[sp + 4]));
				strcat(tmp_replacer, HTML_END[symbol]);
			}
		} else if (sp == 12) {
			//"\[(img)(\s+(\d+)x(\d+)|\s+width=(\d+)\s+height=(\d+))?\](.*)\[\/img\]";
			//  (12,13)(14                                        ,15)
			//	         (16,17)(18,19)       (20,21)        (22,23)  (24,25)
			if (m[12] != -1) {					  // parsing [img*]
				if (m[16] == -1 && m[20] == -1) { // parsing [img]
					strcpy(tmp_replacer, HTML_OPEN[symbol]);
					strncat(tmp_replacer, *buffer + m[sp + 1] + 1, m[1] - (m[sp + 1] + TAG_SIZE + 4));
					strcat(tmp_replacer, HTML_CLOSE[symbol]);
					strcat(tmp_replacer, HTML_END[symbol]);
				} else if (m[16] != -1 && m[20] == -1) { // parsing [img \dx\d]
					//<img src="img_girl.jpg" alt="Girl in a jacket" width="500" height="600">
					//"aaa[img 120x320]bbb[/img]ccc",
					strcpy(tmp_replacer, HTML_OPEN[symbol]);
					strncat(tmp_replacer, *buffer + m[sp + 12], (m[sp + 13] - m[sp + 12]));
					strcat(tmp_replacer, " width=");
					strncat(tmp_replacer, *buffer + m[sp + 4], (m[sp + 5] - m[sp + 4]));
					strcat(tmp_replacer, " height=");
					strncat(tmp_replacer, *buffer + m[sp + 6], (m[sp + 7] - m[sp + 6]));
					strcat(tmp_replacer, HTML_CLOSE[symbol]);
					strncat(tmp_replacer, *buffer + m[sp + 8], (m[sp + 9] - m[sp + 8]));
					strcat(tmp_replacer, HTML_END[symbol]);
				} else { // parsing [img width=\d height=\d]
					strcpy(tmp_replacer, HTML_OPEN[symbol]);
					strncat(tmp_replacer, *buffer + m[sp + 12], (m[sp + 13] - m[sp + 12]));
					strcat(tmp_replacer, "\" width=");
					strncat(tmp_replacer, *buffer + m[sp + 8], (m[sp + 9] - m[sp + 8]));
					strcat(tmp_replacer, " height=");
					strncat(tmp_replacer, *buffer + m[sp + 10], (m[sp + 11] - m[sp + 10]));
					strcat(tmp_replacer, HTML_CLOSE[symbol]);
					strcat(tmp_replacer, HTML_END[symbol]);
				}
			}
		} else if (sp == 26) { //[color=]
			strcpy(tmp_replacer, HTML_OPEN[symbol]);
			strncat(tmp_replacer, *buffer + m[sp + 2], (m[sp + 3] - m[sp + 2]));
			strcat(tmp_replacer, HTML_CLOSE[symbol]);
			strncat(tmp_replacer, *buffer + m[sp + 4], (m[sp + 5] - m[sp + 4]));
			strcat(tmp_replacer, HTML_END[symbol]);
		} else if (sp == 32) { //[size=]
			strcpy(tmp_replacer, HTML_OPEN[symbol]);
			strncat(tmp_replacer, *buffer + m[sp + 2], (m[sp + 3] - m[sp + 2]));
			strcat(tmp_replacer, HTML_CLOSE[symbol]);
			strncat(tmp_replacer, *buffer + m[sp + 4], (m[sp + 5] - m[sp + 4]));
			strcat(tmp_replacer, HTML_END[symbol]);
		}

		PCRE2_UCHAR output[3072] = "";
		PCRE2_SIZE outlen = sizeof(output) / sizeof(PCRE2_UCHAR);

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
		strcpy(*buffer, output);
		memset(output, '\0', outlen);
		free(subgroup);
		free(tmp_replacer);
	}
	// pcre2_code_free(regex);
	pcre2_match_data_free(matches);
	return 0;
}