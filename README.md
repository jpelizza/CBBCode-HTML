Hi, currently this barely works(doesn't work at all)
I was just spitballing with myself thinking of a dirty way to make a bb to html conversor.
Check out branches, I might have done actual work on this, who knows, want to help? Here is the goal:

Use regex in C to convert a simple bb code string input into a html string output

if or when this is done, I want to add a simple emoticons structure.
What I want to acomplish goes something like this:

char *str = "[b]Hi, this is bbcode :) [/b]"
structure bb_emoticons;

bb_add_emoticon(":)","https://mywebsite.com/emoticons/smiley.jpg");

char *str_out = bb_to_html(str,bb_emoticons);

print(str_out);

output: <\strong>Hi, this is bbcode <url=https://mywebsite.com/emoticons/smiley.jpg> <\strong>