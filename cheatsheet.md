stolen from geeks4geeks

| Metacharacter          | Description     |
|--------------|-----------|
| . | To match any character once. The dot character matches a literal dot, within POSIX bracket expressions. For example, a.c matches “abc”, etc., but [a.c] matches only “a”, “.”, or “c”.      |
| –     | Used to define a range. For Example, [a-d] will match for chars a to d, both inclusive.  |
| [ ]     | To match anything inside the square brackets. For e.g. [ab] will match a or b.  |
| ^     | 	The ^ (caret) within the square brackets negates the expressions. For example, [^a] will match anything except a.  |
|$      | 	The dollar symbol matches the ending position of the string if it is the last character of the regular expression.  |
|*      | 	To match the preceding character 0 or more times. For example, a*d will match and, annnd, aaaaad, etc.  |
|{n}      | 	To match the preceding chars n times. Example, [0-9]{3} will match 123, 234, 345, etc.  |
|{n,m}      | 	To match preceding char at least n times and not more than m times. Example, [0-9]{3,5} will match 123, 3456, 45668, etc.  |
