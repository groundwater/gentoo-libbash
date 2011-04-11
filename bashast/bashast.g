/*
   Please use git log for copyright holder and year information

   This file is part of libbash.

   libbash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.

   libbash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with libbash.  If not, see <http://www.gnu.org/licenses/>.
*/
grammar bashast;
options
{
	backtrack	= true;
	output	= AST;
	language	= Java;
	ASTLabelType	= CommonTree;
}
tokens{
	ARG;
	ARRAY;
	BRACE_EXP;
	COMMAND_SUB;
	CASE_PATTERN;
	SUBSHELL;
	CURRSHELL;
	COMPOUND_ARITH;
	COMPOUND_COND;
	FOR_INIT;
	FOR_COND;
	FOR_MOD;
	FNAME;
	OFFSET;
	LIST_EXPAND;
	OP;
	PRE_INCR;
	PRE_DECR;
	POST_INCR;
	POST_DECR;
	PROCESS_SUBSTITUTION;
	VAR_REF;
	NEGATION;
	LIST;
	REPLACE_FIRST;
	REPLACE_ALL;
	STRING;
	COMMAND;
	REPLACE_FIRST;
	REPLACE_LAST;
	FILE_DESCRIPTOR;
	FILE_DESCRIPTOR_MOVE;
	REDIR;
	ARITHMETIC_CONDITION;
	ARITHMETIC_EXPRESSION;
	KEYWORD_TEST;
	BUILTIN_TEST;
	MATCH_ANY_EXCEPT;
	MATCH_EXACTLY_ONE;
	MATCH_AT_MOST_ONE;
	MATCH_NONE;
	MATCH_ANY;
	MATCH_AT_LEAST_ONE;
	MATCH_PATTERN;
	MATCH_ANY_EXCEPT;
	CHARACTER_CLASS;
	EQUIVALENCE_CLASS;
	COLLATING_SYMBOL;
	SINGLE_QUOTED_STRING;
	DOUBLE_QUOTED_STRING;
	// Avoid ambiguity (being a sign or an operator)
	PLUS_SIGN;
	MINUS_SIGN;
}

start	:	(flcomment!)? EOL!* list^ ;
//Because the comment token doesn't handle the first comment in a file if it's on the first line, have a parser rule for it
flcomment
	:	POUND ~(EOL)* EOL;
list	:	list_level_2 BLANK* (SEMIC|AMP|EOL)? -> ^(LIST list_level_2);
clist
options{greedy=false;}
	:	list_level_2 -> ^(LIST list_level_2);
list_level_1
	:	(function|pipeline) (BLANK!*(LOGICAND^|LOGICOR^)BLANK!* (function|pipeline))*;
// ';' '&' and EOL have lower operator precedence than '&&' and '||' so we need level2 here
list_level_2
	:	list_level_1 ((BLANK!?SEMIC!|BLANK!?AMP^|(BLANK!? EOL!)+)BLANK!? list_level_1)*;
pipeline
	:	BLANK!* time? (BANG BLANK!+)? command^ (BLANK!* PIPE^ BLANK!* command)*;
time	:	TIME^ BLANK!+ time_posix?;
time_posix
	:	TIME_POSIX BLANK!+;
//The structure of a command in bash
command
	:	var_def (BLANK!+ var_def)*
	|	compound_command
	|	simple_command;
//Simple bash commands
simple_command
	:	(var_def BLANK!+)* bash_command^ redirect*;
bash_command
	:	fname_no_res_word (BLANK+ arg)* -> ^(COMMAND fname_no_res_word arg*);
//An argument to a command
//fname can also match var_ref and command_sub but that gives a noisier AST
arg
options{backtrack=false;}
	:	(var_ref) => var_ref
	|	(command_sub) => command_sub
	|	fname;
redirect:	BLANK!* here_string_op^ BLANK!* fname
	|	BLANK!* here_doc_op^ BLANK!* fname EOL! heredoc
	|	BLANK* redir_op BLANK* redir_dest -> ^(REDIR redir_op redir_dest)
	|	BLANK!* process_substitution;
redir_dest
	:	DIGIT MINUS?
	|	fname //path to a file
	|	file_desc_as_file; //handles file descriptors0
file_desc_as_file
	:	AMP DIGIT -> FILE_DESCRIPTOR[$DIGIT]
	|	AMP DIGIT MINUS -> FILE_DESCRIPTOR_MOVE[$DIGIT];
heredoc	:	(fname EOL!)*;
here_string_op
	:	HERE_STRING_OP;
here_doc_op
	:	LSHIFT MINUS -> OP["<<-"]
	|	LSHIFT -> OP["<<"];
redir_op:	AMP LESS_THAN -> OP["&<"]
	|	GREATER_THAN AMP -> OP[">&"]
	|	LESS_THAN AMP -> OP["<&"]
	|	LESS_THAN GREATER_THAN -> OP["<>"]
	|	RSHIFT -> OP[">>"]
	|	AMP GREATER_THAN -> OP["&>"]
	|	AMP RSHIFT -> OP ["&>>"]
	|	LESS_THAN
	|	GREATER_THAN
	|	DIGIT redir_op;
brace_expansion
	:	LBRACE BLANK* brace_expansion_inside BLANK* RBRACE -> ^(BRACE_EXP brace_expansion_inside);
brace_expansion_inside
	:	commasep|range;
range	:	DIGIT DOTDOT^ DIGIT
	|	LETTER DOTDOT^ LETTER;
brace_expansion_part
	:	brace_expansion
	|	fname
	|	var_ref
	|	command_sub;
commasep:	brace_expansion_part(COMMA! brace_expansion_part)+;
command_sub
	:	DOLLAR LPAREN BLANK* pipeline BLANK? RPAREN -> ^(COMMAND_SUB pipeline)
	|	TICK BLANK* pipeline BLANK? TICK -> ^(COMMAND_SUB pipeline) ;
//compound commands
compound_command
	:	for_expr
	|	sel_expr
	|	if_expr
	|	while_expr
	|	until_expr
	|	case_expr
	|	subshell
	|	currshell
	|	arith_comparison
	|	cond_comparison;
//Expressions allowed inside a compound command
for_expr:	FOR BLANK+ name (wspace IN BLANK+ word)? semiel DO wspace* clist semiel DONE -> ^(FOR name (word)? clist)
	|	FOR BLANK* LLPAREN EOL? (BLANK* init=arithmetic BLANK*|BLANK+)? (SEMIC (BLANK? fcond=arithmetic BLANK*|BLANK+)? SEMIC|DOUBLE_SEMIC) (BLANK* mod=arithmetic)? wspace* RRPAREN semiel DO wspace clist semiel DONE
		-> ^(FOR ^(FOR_INIT $init)? ^(FOR_COND $fcond)? ^(FOR_MOD $mod)? clist)
	;
sel_expr:	SELECT BLANK+ name (wspace IN BLANK+ word)? semiel DO wspace* clist semiel DONE -> ^(SELECT name (word)? clist)
	;
if_expr	:	IF wspace+ ag=clist semiel THEN wspace+ iflist=clist semiel EOL* (elif_expr)* (ELSE wspace+ else_list=clist semiel EOL*)? FI
		-> ^(IF $ag $iflist (elif_expr)* ^($else_list)?)
	;
elif_expr
	:	ELIF BLANK+ ag=clist semiel THEN wspace+ iflist=clist semiel -> ^(IF["if"] $ag $iflist);
while_expr
	:	WHILE wspace? istrue=clist semiel DO wspace dothis=clist semiel DONE -> ^(WHILE $istrue $dothis)
	;
until_expr
	:	UNTIL wspace? istrue=clist semiel DO wspace dothis=clist semiel DONE -> ^(UNTIL $istrue $dothis)
	;
// double semicolon is optional for the last alternative
case_expr
	:	CASE BLANK+ word wspace IN wspace case_body? ESAC -> ^(CASE word case_body?);
case_body
	:	case_stmt (wspace* DOUBLE_SEMIC case_stmt)* wspace* DOUBLE_SEMIC? wspace* -> case_stmt*;
case_stmt
	:	wspace* (LPAREN BLANK*)? case_pattern (BLANK* PIPE BLANK? case_pattern)* BLANK* RPAREN (wspace* clist)?
		-> ^(CASE_PATTERN case_pattern+ clist?);
case_pattern
	:	command_sub
	|	fname
	|	TIMES;
//A grouping of commands executed in a subshell
subshell:	LPAREN wspace? clist (BLANK* SEMIC)? (BLANK* EOL)* BLANK* RPAREN -> ^(SUBSHELL clist);
//A grouping of commands executed in the current shell
currshell
	:	LBRACE wspace clist semiel RBRACE -> ^(CURRSHELL clist);
//comparison using arithmetic
arith_comparison
	:	LLPAREN wspace? arithmetic wspace? RRPAREN -> ^(COMPOUND_ARITH arithmetic);
cond_comparison
	:	cond_expr -> ^(COMPOUND_COND cond_expr);
//Variables
//Defining a variable
//It's not legal to do FOO[1]=(a b c)
var_def
	:	name LSQUARE BLANK? explicit_arithmetic BLANK* RSQUARE EQUALS fname -> ^(EQUALS ^(name explicit_arithmetic) fname)
	|	name EQUALS^ value;
//Possible values of a variable
value	:	fname
	|	LPAREN! wspace!? arr_val RPAREN!;
//allow the parser to create array variables
arr_val	:
	|	(ag+=array_atom wspace?)+ -> ^(ARRAY $ag+);
array_atom
	:	LSQUARE! BLANK!* explicit_arithmetic BLANK!? RSQUARE! EQUALS^ fname
	|	fname;
//Referencing a variable (different possible ways/special parameters)
var_ref
	:	DOLLAR LBRACE BLANK* var_exp BLANK* RBRACE -> ^(VAR_REF var_exp)
	|	DOLLAR name -> ^(VAR_REF name)
	|	DOLLAR num -> ^(VAR_REF num)
	|	DOLLAR TIMES -> ^(VAR_REF TIMES)
	|	DOLLAR AT -> ^(VAR_REF AT)
	|	DOLLAR POUND -> ^(VAR_REF POUND)
	|	DOLLAR QMARK -> ^(VAR_REF QMARK)
	|	DOLLAR MINUS -> ^(VAR_REF MINUS)
	|	DOLLAR BANG -> ^(VAR_REF BANG);
//Variable expansions
var_exp	:	var_name (USE_DEFAULT|USE_ALTERNATE|DISPLAY_ERROR|ASSIGN_DEFAULT)^ word
	|	var_name COLON wspace* LPAREN? os=explicit_arithmetic RPAREN? (COLON len=explicit_arithmetic)? -> ^(OFFSET var_name $os ^($len)?)
	|	BANG^ var_name (TIMES|AT)
	|	BANG var_name LSQUARE (op=TIMES|op=AT) RSQUARE -> ^(LIST_EXPAND var_name $op)
	|	BANG var_name -> ^(VAR_REF var_name)
	|	POUND^ var_name
	|	var_name (POUND^|POUNDPOUND^) fname
	|	var_name (PCT^|PCTPCT^) fname
	|	var_name SLASH POUND ns_str SLASH fname -> ^(REPLACE_FIRST var_name ns_str fname)
	| 	var_name SLASH PCT ns_str SLASH fname -> ^(REPLACE_LAST var_name ns_str fname)
	|	var_name SLASH SLASH ns_str SLASH fname -> ^(REPLACE_ALL var_name ns_str fname)
	|	var_name SLASH SLASH ns_str SLASH? -> ^(REPLACE_ALL var_name ns_str)
	|	var_name SLASH ns_str SLASH fname -> ^(REPLACE_FIRST var_name ns_str fname)
	|	var_name SLASH POUND ns_str SLASH? -> ^(REPLACE_FIRST var_name ns_str)
	|	var_name SLASH PCT ns_str SLASH? -> ^(REPLACE_LAST var_name ns_str)
	|	var_name SLASH ns_str SLASH? -> ^(REPLACE_FIRST var_name ns_str)
	|	arr_var_ref
	|	var_name
	|	TIMES
	|	AT;
//Allowable variable names in the variable expansion
var_name:	num|name|POUND;
//Referencing an array variable
arr_var_ref
	:	name^ LSQUARE! DIGIT+ RSQUARE!;
//Conditional Expressions
cond_expr
	:	LSQUARE LSQUARE wspace keyword_cond wspace RSQUARE RSQUARE -> ^(KEYWORD_TEST keyword_cond)
	|	LSQUARE wspace builtin_cond wspace RSQUARE -> ^(BUILTIN_TEST builtin_cond)
	|	TEST_EXPR wspace builtin_cond-> ^(BUILTIN_TEST builtin_cond);
cond_primary
	:	LPAREN! BLANK!* keyword_cond BLANK!* RPAREN!
	|	keyword_cond_binary
	|	keyword_cond_unary
	|	fname;
keyword_cond_binary
	:	cond_part BLANK!* binary_str_op_keyword^ BLANK!? cond_part;
keyword_cond_unary
	:	uop^ BLANK!+ cond_part;
builtin_cond_primary
	:	LPAREN! BLANK!* builtin_cond BLANK!* RPAREN!
	|	builtin_cond_binary
	|	builtin_cond_unary
	|	fname;
builtin_cond_binary
	:	cond_part BLANK!* binary_string_op_builtin^ BLANK!? cond_part;
builtin_cond_unary
	:	uop^ BLANK!+ cond_part;
keyword_cond
	:	(negate_primary|cond_primary) (BLANK!* (LOGICOR^|LOGICAND^) BLANK!* keyword_cond)?;
builtin_cond
	:	(negate_builtin_primary|builtin_cond_primary) (BLANK!* (LOGICOR^|LOGICAND^) BLANK!* builtin_cond)?;
negate_primary
	:	BANG BLANK+ cond_primary -> ^(NEGATION cond_primary);
negate_builtin_primary
	:	BANG BLANK+ builtin_cond_primary -> ^(NEGATION builtin_cond_primary);
binary_str_op_keyword
	:	bop
	|	EQUALS EQUALS -> OP["=="]
	|	EQUALS
	|	BANG EQUALS -> OP["!="]
	|	LESS_THAN
	|	GREATER_THAN;
binary_string_op_builtin
	:	bop
	|	EQUALS
	|	BANG EQUALS -> OP["!="]
	|	ESC_LT
	|	ESC_GT;
bop	:	MINUS! NAME^;
unary_cond
	:	uop^ BLANK! cond_part;
uop	:	MINUS! LETTER;
//Allowable parts of conditions
cond_part:	brace_expansion
	|	var_ref
	|	res_word_str -> ^(STRING res_word_str)
	|	num
	|	fname
	|	arithmetic;
//Rules for whitespace/line endings
wspace	:	BLANK+|EOL+;
semiel	:	BLANK* (SEMIC|EOL) BLANK*;

//definition of word.  this is just going to grow...
word	:	(brace_expansion) => brace_expansion
	|	(command_sub) => command_sub
	|	(var_ref) => var_ref
	|	(num) => num
	|	(arithmetic_expansion) => arithmetic_expansion
	|	fname;

num
options{k=1;backtrack=false;}
	:	DIGIT|NUMBER;
//A rule for filenames/strings
res_word_str
	:	CASE|DO|DONE|ELIF|ELSE|ESAC|FI|FOR|FUNCTION|IF|IN|SELECT|THEN|UNTIL|WHILE|TIME;
//Any allowable part of a string, including slashes, no pounds
str_part
	:	ns_str_part
	|	SLASH;
//Any allowable part of a string, with pounds
str_part_with_pound
	:	str_part
	|	POUND
	|	POUNDPOUND;
//Parts of strings, no slashes, no reserved words
ns_str_part
	:	num
	|	name
	|	esc_char
	|OTHER|EQUALS|PCT|PCTPCT|MINUS|DOT|DOTDOT|COLON|TEST_EXPR
	|TILDE|MUL_ASSIGN|DIVIDE_ASSIGN|MOD_ASSIGN|PLUS_ASSIGN|MINUS_ASSIGN
	|TIME_POSIX|LSHIFT_ASSIGN|RSHIFT_ASSIGN|AND_ASSIGN|XOR_ASSIGN
	|OR_ASSIGN|CARET;
//strings with no slashes, used in certain variable expansions
ns_str	:	ns_str_part* -> ^(STRING ns_str_part*);
//Generic strings/filenames.
fname	:	fname_part+ -> ^(STRING fname_part+);
//A string that is NOT a bash reserved word
fname_no_res_word
	:	nqstr fname_part* -> ^(STRING nqstr fname_part*);
fname_part
	:	nqstr
	|	res_word_str;
//non-quoted string rule, allows expansions
nqstr	:	(bracket_pattern_match|extended_pattern_match|var_ref|command_sub|arithmetic_expansion|brace_expansion|dqstr|sqstr|(str_part str_part_with_pound*)|pattern_match_trigger|BANG)+;
//double quoted string rule, allows expansions
dqstr	:	QUOTE dqstr_part* QUOTE -> ^(DOUBLE_QUOTED_STRING dqstr_part*);
dqstr_part
	: var_ref
	| command_sub
	| arithmetic_expansion
	| ~(DOLLAR|TICK|QUOTE);
//single quoted string rule, no expansions
sqstr_part
	: ~SQUOTE*;
sqstr	:	SQUOTE sqstr_part SQUOTE -> ^(SINGLE_QUOTED_STRING sqstr_part);
//certain tokens that trigger pattern matching
pattern_match_trigger
	:	LSQUARE
	|	RSQUARE
	|	QMARK
	|	PLUS
	|	TIMES
	|	AT;
//Pattern matching using brackets
bracket_pattern_match
	:	LSQUARE RSQUARE (BANG|CARET) pattern_match* RSQUARE -> ^(MATCH_ANY_EXCEPT RSQUARE pattern_match*)
	|	LSQUARE RSQUARE pattern_match* RSQUARE -> ^(MATCH_PATTERN RSQUARE pattern_match*)
	|	LSQUARE (BANG|CARET) pattern_match+ RSQUARE -> ^(MATCH_ANY_EXCEPT pattern_match+)
	|	LSQUARE pattern_match+ RSQUARE -> ^(MATCH_PATTERN pattern_match+);
//allowable patterns with bracket pattern matching
pattern_match
	:	pattern_class_match
	|	str_part str_part_with_pound*;
//special class patterns to match: [:alpha:] etc
pattern_class_match
	:	LSQUARE COLON NAME COLON RSQUARE -> ^(CHARACTER_CLASS NAME)
	|	LSQUARE EQUALS pattern_char EQUALS RSQUARE -> ^(EQUIVALENCE_CLASS pattern_char)
	|	LSQUARE DOT NAME DOT RSQUARE -> ^(COLLATING_SYMBOL NAME);
//Characters allowed in matching equivalence classes
pattern_char
	:	LETTER|DIGIT|OTHER|QMARK|COLON|AT|SEMIC|POUND|SLASH|BANG|TIMES|COMMA|PIPE|AMP|MINUS|PLUS|PCT|EQUALS|LSQUARE|RSQUARE|RPAREN|LPAREN|RBRACE|LBRACE|DOLLAR|TICK|DOT|LESS_THAN|GREATER_THAN|SQUOTE|QUOTE;
//extended pattern matching
extended_pattern_match
	:	QMARK LPAREN fname (PIPE fname)* RPAREN -> ^(MATCH_AT_MOST_ONE fname+)
	|	TIMES LPAREN fname (PIPE fname)* RPAREN -> ^(MATCH_ANY fname+)
	|	PLUS LPAREN fname (PIPE fname)* RPAREN -> ^(MATCH_AT_LEAST_ONE fname+)
	|	AT LPAREN fname (PIPE fname)* RPAREN -> ^(MATCH_EXACTLY_ONE fname+)
	|	BANG LPAREN fname (PIPE fname)* RPAREN -> ^(MATCH_NONE fname+);
//Arithmetic expansion
arithmetic_expansion
	:	DOLLAR LLPAREN BLANK* arithmetics BLANK* RRPAREN -> ^(ARITHMETIC_EXPRESSION arithmetics);
//explicit arithmetic in places like array indexes
explicit_arithmetic
	:	(DOLLAR LLPAREN BLANK*)? arithmetics (BLANK* RRPAREN)? -> arithmetics;
//The comma operator for arithmetic expansions
arithmetics
	:	arithmetic (BLANK!* COMMA! BLANK!* arithmetic)*;
arithmetic
	:	arithmetic_condition
	|	arithmetic_assignment;
//The base of the arithmetic operator.  Used for order of operations
primary	:	num
	|	var_ref
	|	command_sub
	|	name -> ^(VAR_REF name)
	|	LPAREN! (arithmetics) RPAREN!;
post_inc_dec
	:	primary BLANK? PLUS PLUS -> ^(POST_INCR primary)
	|	primary BLANK? MINUS MINUS -> ^(POST_DECR primary);
pre_inc_dec
	:	PLUS PLUS BLANK? primary -> ^(PRE_INCR primary)
	|	MINUS MINUS BLANK? primary -> ^(PRE_DECR primary);
unary	:	post_inc_dec
	|	pre_inc_dec
	|	primary
	|	PLUS unary -> ^(PLUS_SIGN unary)
	|	MINUS unary -> ^(MINUS_SIGN unary)
	|	(TILDE|BANG)^ unary;
exponential
	:	unary (BLANK!* EXP^ BLANK!* unary)* ;
times_division_modulus
	:	exponential (BLANK!* (TIMES^|SLASH^|PCT^) BLANK!* exponential)*;
addsub	:	times_division_modulus (BLANK!* (PLUS^|MINUS^)BLANK!* times_division_modulus)*;
shifts	:	addsub (BLANK!* (LSHIFT^|RSHIFT^) BLANK!* addsub)*;
compare	:	shifts (BLANK!* (LEQ^|GEQ^|LESS_THAN^|GREATER_THAN^)BLANK!* shifts)?;
bitwiseand
	:	compare (BLANK!* AMP^ BLANK!* compare)*;
bitwisexor
	:	bitwiseand (BLANK!* CARET^ BLANK!* bitwiseand)*;
bitwiseor
	:	bitwisexor (BLANK!* PIPE^ BLANK!* bitwisexor)*;
logicand:	bitwiseor (BLANK!* LOGICAND^ BLANK!* bitwiseor)*;
logicor	:	logicand (BLANK!* LOGICOR^ BLANK!* logicand)*;

arithmetic_condition
	:	cnd=logicor QMARK t=logicor COLON f=logicor -> ^(ARITHMETIC_CONDITION $cnd $t $f);
arithmetic_assignment
	:	(name BLANK!* (EQUALS|MUL_ASSIGN|DIVIDE_ASSIGN|MOD_ASSIGN|PLUS_ASSIGN|MINUS_ASSIGN|LSHIFT_ASSIGN|RSHIFT_ASSIGN|AND_ASSIGN|XOR_ASSIGN|OR_ASSIGN)^ BLANK!*)? logicor;
process_substitution
	:	(dir=LESS_THAN|dir=GREATER_THAN)LPAREN clist BLANK* RPAREN -> ^(PROCESS_SUBSTITUTION $dir clist);
//the biggie: functions
function:	FUNCTION BLANK+ name (BLANK* parens)? wspace compound_command redirect* -> ^(FUNCTION name compound_command redirect*)
	|	name BLANK* parens wspace compound_command redirect* -> ^(FUNCTION["function"] name compound_command redirect*);
parens	:	LPAREN BLANK* RPAREN;
name	:	NAME
	|	LETTER
	|	'_';
esc_char:	ESC (DIGIT DIGIT? DIGIT?|LETTER ALPHANUM ALPHANUM?|.);

//****************
// TOKENS/LEXER RULES
//****************

COMMENT
	:	(BLANK|EOL) '#' ~('\n'|'\r')* {$channel=HIDDEN;}
	;
//Bash "reserved words"
BANG	:	'!';
CASE	:	'case';
DO	:	'do';
DONE	:	'done';
ELIF	:	'elif';
ELSE	:	'else';
ESAC	:	'esac';
FI	:	'fi';
FOR	:	'for';
FUNCTION:	'function';
IF	:	'if';
IN	:	'in';
SELECT	:	'select';
THEN	:	'then';
UNTIL	:	'until';
WHILE	:	'while';
LBRACE	:	'{';
RBRACE	:	'}';
TIME	:	'time';

//Other special useful symbols
RPAREN	:	')';
LPAREN	:	'(';
LLPAREN	:	'((';
RRPAREN	:	'))';
LSQUARE	:	'[';
RSQUARE	:	']';
TICK	:	'`';
DOLLAR	:	'$';
AT	:	'@';
DOT	:	'.';
DOTDOT	:	'..';
//Arith ops
TIMES	:	'*';
EQUALS	:	'=';
MINUS	:	'-';
PLUS	:	'+';
EXP	:	'**';
AMP	:	'&';
LEQ	:	'<=';
GEQ	:	'>=';
CARET	:	'^';
LESS_THAN	:	'<';
GREATER_THAN	:	'>';
LSHIFT	:	'<<';
RSHIFT	:	'>>';
MUL_ASSIGN	:	'*=';
DIVIDE_ASSIGN	:	'/=';
MOD_ASSIGN	:	'%=';
PLUS_ASSIGN	:	'+=';
MINUS_ASSIGN	:	'-=';
LSHIFT_ASSIGN	:	'<<=';
RSHIFT_ASSIGN	:	'>>=';
AND_ASSIGN	:	'&=';
XOR_ASSIGN	:	'^=';
OR_ASSIGN	:	'|=';
//some separators
SEMIC	:	';';
DOUBLE_SEMIC
	:	';;';
PIPE	:	'|';
QUOTE	:	'"';
SQUOTE	:	'\'';
COMMA	:	',';
//Because bash isn't exactly whitespace dependent... need to explicitly handle blanks
BLANK	:	(' '|'\t')+;
EOL	:	('\r'?'\n')+ ;
//some fragments for creating words...
DIGIT	:	'0'..'9';
NUMBER	:	DIGIT DIGIT+;
LETTER	:	('a'..'z'|'A'..'Z');
fragment
ALPHANUM:	(DIGIT|LETTER);
//Some special redirect operators
TILDE	:	'~';
HERE_STRING_OP
	:	'<<<';
//Tokens for parameter expansion
POUND	:	'#';
POUNDPOUND
	:	'##';
PCT	:	'%';
PCTPCT	:	'%%';
SLASH	:	'/';
USE_DEFAULT	:	':-';
ASSIGN_DEFAULT:	':=';
DISPLAY_ERROR:	':?';
USE_ALTERNATE:	':+';
COLON	:	':';
QMARK	:	'?';
//Operators for conditional statements
TEST_EXPR	:	'test';
LOGICAND
	:	'&&';
LOGICOR	:	'||';
//Tokens for strings
CONTINUE_LINE
	:	(ESC EOL)+{$channel=HIDDEN;};
ESC_RPAREN
	:	ESC RPAREN;
ESC_LPAREN
	:	ESC LPAREN;
ESC_LT	:	ESC'<';
ESC_GT	:	ESC'>';
//For pipeline
TIME_POSIX
	:	'-p';
//Handle ANSI C escaped characters: escaped octal, escaped hex, escaped ctrl+ chars, then all others
ESC	:	'\\';
NAME	:	(LETTER|'_')(ALPHANUM|'_')+;
OTHER	:	.;
