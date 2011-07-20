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

#ifdef OUTPUT_C
grammar libbash;
#else
grammar java_libbash;
#endif

options
{
	output = AST;
#ifdef OUTPUT_C
	language  = C;
	ASTLabelType  = pANTLR3_BASE_TREE;
#else
	language = Java;
	ASTLabelType = CommonTree;
#endif
}

tokens{
	ANSI_C_QUOTING;
	ARG;
	ARRAY;
	ARRAY_SIZE;
	BRACE_EXP;
	COMMAND_SUB;
	CASE_PATTERN;
	CASE_COMMAND;
	SUBSHELL;
	CURRENT_SHELL;
	COMPOUND_COND;
	CFOR;
	FOR_INIT;
	FOR_COND;
	FOR_MOD;
	IF_STATEMENT;
	OP;
	PRE_INCR;
	PRE_DECR;
	POST_INCR;
	POST_DECR;
	PROCESS_SUBSTITUTION;
	VAR_REF;
	NEGATION;
	LIST;
	STRING;
	COMMAND;
	FILE_DESCRIPTOR;
	FILE_DESCRIPTOR_MOVE;
	REDIR;
	ARITHMETIC_CONDITION;
	ARITHMETIC_EXPRESSION;
	KEYWORD_TEST;
	BUILTIN_TEST;
	MATCH_ANY_EXCEPT;
	EXTENDED_MATCH_EXACTLY_ONE;
	EXTENDED_MATCH_AT_MOST_ONE;
	EXTENDED_MATCH_NONE;
	EXTENDED_MATCH_ANY;
	EXTENDED_MATCH_AT_LEAST_ONE;
	BRANCH;
	MATCH_PATTERN;
	MATCH_REGULAR_EXPRESSION;
	NOT_MATCH_PATTERN;
	MATCH_ANY;
	MATCH_ANY_EXCEPT;
	MATCH_ALL;
	MATCH_ONE;
	CHARACTER_CLASS;
	EQUIVALENCE_CLASS;
	COLLATING_SYMBOL;
	DOUBLE_QUOTED_STRING;
	SINGLE_QUOTED_STRING;
	VARIABLE_DEFINITIONS;

	USE_DEFAULT_WHEN_UNSET;
	USE_ALTERNATE_WHEN_UNSET;
	DISPLAY_ERROR_WHEN_UNSET;
	ASSIGN_DEFAULT_WHEN_UNSET;
	USE_DEFAULT_WHEN_UNSET_OR_NULL;
	USE_ALTERNATE_WHEN_UNSET_OR_NULL;
	DISPLAY_ERROR_WHEN_UNSET_OR_NULL;
	ASSIGN_DEFAULT_WHEN_UNSET_OR_NULL;
	OFFSET;
	LIST_EXPAND;
	REPLACE_FIRST;
	REPLACE_ALL;
	REPLACE_AT_START;
	REPLACE_AT_END;
	LAZY_REMOVE_AT_START;
	LAZY_REMOVE_AT_END;
	EMPTY_EXPANSION_VALUE;

	PLUS_SIGN;
	MINUS_SIGN;

	NOT_EQUALS;
	BUILTIN_LOGIC;
}

@lexer::members
{
#ifdef OUTPUT_C
	bool double_quoted = false;
#else
	boolean double_quoted = false;

	int LA(int index) {
		return input.LA(index);
	}
#endif
	int paren_level = 0;
}
#ifdef OUTPUT_C
@includes {
	C_INCLUDE #include <iostream>
	C_INCLUDE #include <string>

	C_INCLUDE #include <boost/numeric/conversion/cast.hpp>
}
@members
{
	static std::string get_string(pANTLR3_COMMON_TOKEN token)
	{
		if(!token || !token->start)
			return "";
		// Use reinterpret_cast here because we have to cast C code.
		// The real type here is int64_t which is used as a pointer.
		// token->stop - token->start + 1 should be bigger than 0.
		return std::string(reinterpret_cast<const char *>(token->start),
			boost::numeric_cast<unsigned>(token->stop - token->start + 1));
	}

	static bool is_here_end(plibbashParser ctx, const std::string& here_document_word, int number_of_tokens_in_word)
	{
		std::string word;
		for(int i = 1; i <= number_of_tokens_in_word; ++i)
			word += get_string(LT(i));
		return (word == here_document_word);
	}

	static void free_here_document(plibbashParser_here_document_SCOPE scope)
	{
		(&(scope->here_document_word))->std::string::~string();
	}
}
#else
@members
{
	boolean is_here_end(String here_document_word, int number_of_tokens) {
		String word = "";
		for(int i = 1; i <= number_of_tokens; ++i)
			word += input.LT(i).getText();
		return (word.equals(here_document_word));
	}

	String get_string(Token token) {
		return token.getText();
	}

	Token LT(int index) {
		return input.LT(index);
	}

	int LA(int index) {
		return input.LA(index);
	}
}
#endif

start
	:	((POUND) =>first_line_comment)? EOL* BLANK? command_list BLANK? (SEMIC|AMP|EOL)? EOF -> command_list;

first_line_comment
	:	POUND ~(EOL)* EOL;

command_list
	:	list_level_2 -> ^(LIST list_level_2);
list_level_1
	:	pipeline (BLANK!?(LOGICAND^|LOGICOR^)(BLANK!|EOL!)* pipeline)*;
list_level_2
	:	list_level_1 (BLANK!? command_separator (BLANK!? EOL!)* BLANK!? list_level_1)*;
command_separator
	:	SEMIC!
	|	AMP^
	|	EOL!;
pipeline
	:	time? ((BANG) => (BANG BLANK!))? command^ (BLANK!? PIPE^ BLANK!? command)*;

time
	:	TIME^ BLANK! ((time_posix) => time_posix)?;
time_posix
	:	MINUS! LETTER BLANK!;

redirection
	:	redirection_atom+;
redirection_atom
	:	redirection_operator BLANK? redirection_destination -> ^(REDIR redirection_operator redirection_destination)
	|	BLANK? process_substitution;

process_substitution
	:	(dir=LESS_THAN|dir=GREATER_THAN)LPAREN BLANK* command_list BLANK* RPAREN
			-> ^(PROCESS_SUBSTITUTION $dir command_list);

redirection_destination
	:	(file_descriptor) => file_descriptor
	|	string_expr;
file_descriptor
	:	DIGIT -> ^(FILE_DESCRIPTOR DIGIT)
	|	DIGIT MINUS -> ^(FILE_DESCRIPTOR_MOVE DIGIT);

here_string
	:	BLANK? HERE_STRING_OP^ BLANK!? (string_expr) => string_expr;

here_document
#ifdef OUTPUT_C
scope {
	std::string here_document_word;
	int number_of_tokens;
}
@init {
	// http://antlr.1301665.n2.nabble.com/C-target-initialization-of-return-scope-structures-td5078478.html
	new (&($here_document::here_document_word)) std::string;
	$here_document::number_of_tokens = 0;
	ctx->plibbashParser_here_documentTop->free = &free_here_document;
}
#else
scope {
	String here_document_word;
	int number_of_tokens;
}
@init {
	$here_document::here_document_word = "";
	$here_document::number_of_tokens = 0;
}
#endif
	:	BLANK? (here_document_operator) => here_document_operator BLANK? here_document_begin
		redirection? EOL here_document_content? here_document_end
			-> ^(here_document_operator ^(STRING here_document_content?) redirection?);

here_document_operator
	:	LSHIFT
		(
			(MINUS) => MINUS -> OP["<<-"]
			| -> OP["<<"]
		);

here_document_begin
	:	(
			token=~(EOL|BLANK|LESS_THAN|HERE_STRING_OP|AMP|GREATER_THAN|RSHIFT)
			{
				$here_document::here_document_word += get_string($token);
				$here_document::number_of_tokens++;
			}
		)+;
here_document_end
	:	({ $here_document::number_of_tokens != 0 }? => . { $here_document::number_of_tokens--; })+;
here_document_content
#ifdef OUTPUT_C
	:	({ !is_here_end(ctx, $here_document::here_document_word, $here_document::number_of_tokens)}? => .)+;
#else
	:	({ !is_here_end($here_document::here_document_word, $here_document::number_of_tokens)}? => .)+;
#endif

redirection_operator
	:	BLANK! DIGIT redirection_operator
	|	BLANK?
		(
			AMP LESS_THAN -> OP["&<"]
			|	GREATER_THAN AMP -> OP[">&"]
			|	LESS_THAN AMP -> OP["<&"]
			|	LESS_THAN GREATER_THAN -> OP["<>"]
			|	RSHIFT -> OP[">>"]
			|	AMP GREATER_THAN -> OP["&>"]
			|	AMP RSHIFT -> OP ["&>>"]
			|	LESS_THAN -> LESS_THAN
			|	GREATER_THAN -> GREATER_THAN
		);

command
	:	command_atom
		(
			redirection -> ^(COMMAND command_atom redirection)
			|	here_document -> ^(COMMAND command_atom here_document)
			|	here_string -> ^(COMMAND command_atom here_string)
			|	-> ^(COMMAND command_atom)
		);

command_atom
	:	(FOR|SELECT|IF|WHILE|UNTIL|CASE|LPAREN|LBRACE|LLPAREN|LSQUARE|TEST_EXPR) => compound_command
	|	FUNCTION BLANK string_expr_no_reserved_word ((BLANK? parens wspace?)|wspace) compound_command
			-> ^(FUNCTION string_expr_no_reserved_word compound_command)
	|	(name (LSQUARE|EQUALS|PLUS_ASSIGN)|LOCAL|EXPORT) => variable_definitions
			(
				(BLANK bash_command) => BLANK bash_command -> bash_command variable_definitions
				|	-> ^(VARIABLE_DEFINITIONS variable_definitions)
			)
	|	string_expr_no_reserved_word
		(
			(BLANK? parens) => BLANK? parens wspace? compound_command
				-> ^(FUNCTION["function"] string_expr_no_reserved_word compound_command)
			|	(
					{LA(1) == BLANK &&
					(
						LA(2) != AMP
						// Resolve conflicts with bash redirection
						&&LA(2) != LESS_THAN
						&&LA(2) != GREATER_THAN
						&&LA(2) != RSHIFT
						&&(LA(2) != DIGIT || (LA(3) != AMP && LA(3) != LESS_THAN
											  && LA(3) != GREATER_THAN && LA(3) != RSHIFT))
						// Resolve conflicts with end of command
						&&LA(2) != SEMIC
						&&LA(2) != EOL
						// Resolve conflict with sub shell
						&&LA(2) != RPAREN
						// Resolve conflict with case statement
						&&LA(2) != DOUBLE_SEMIC
						// Resolve conflicts with logical operator
						&&LA(2) != LOGICAND
						&&LA(2) != LOGICOR
						// Resolve conflict with pipeline
						&&LA(2) != PIPE
						// Resolve conflicts with here document and here string
						&&LA(2) != HERE_STRING_OP
						&&LA(2) != LSHIFT
					)}? => BLANK bash_command_arguments
				)* -> string_expr_no_reserved_word bash_command_arguments*
		);

variable_definitions
	:	(
			variable_definition_atom ((BLANK name (LSQUARE|EQUALS|PLUS_ASSIGN)) => BLANK! variable_definition_atom)*
			|	(LOCAL) => LOCAL BLANK! local_item ((BLANK name) => BLANK! local_item)*
			|	(EXPORT) => EXPORT! ((BLANK name) => BLANK! export_item)+
		);

variable_definition_atom
	:	name LSQUARE BLANK? explicit_arithmetic BLANK? RSQUARE EQUALS string_expr?
			-> ^(EQUALS ^(name explicit_arithmetic) string_expr?)
	|	name EQUALS value? -> ^(EQUALS name value?)
	|	name PLUS_ASSIGN array_value -> ^(PLUS_ASSIGN name array_value)
	|	name PLUS_ASSIGN string_expr_part?
			-> ^(EQUALS name ^(STRING ^(VAR_REF name) string_expr_part?));
value
	:	string_expr
	|	array_value;

array_value
scope {
#ifdef OUTPUT_C
	bool array_value_end;
#else
	boolean array_value_end;
#endif
}
	:	LPAREN wspace?
		(
			RPAREN -> ^(ARRAY)
			|	{$array_value::array_value_end = false; } array_atom
				({!$array_value::array_value_end}? => wspace array_atom)*
					-> ^(ARRAY array_atom+)
		);
array_atom
	:	(
			(LSQUARE) => LSQUARE! BLANK!? explicit_arithmetic BLANK!? RSQUARE! EQUALS^ string_expr
			|	string_expr
		)
		(
			(wspace RPAREN) => wspace! RPAREN! {$array_value::array_value_end = true; }
			|	(RPAREN) => RPAREN! {$array_value::array_value_end = true; }
			|
		);

local_item
	:	variable_definition_atom
	|	name -> ^(EQUALS name);
export_item
	:	variable_definition_atom
	|	name ->;

bash_command
	:	string_expr_no_reserved_word ((BLANK bash_command_arguments) => BLANK! bash_command_arguments)*;

bash_command_arguments
	:	bash_command_argument_atom+ -> ^(STRING bash_command_argument_atom+);
// TODO support brace expansion and braces
bash_command_argument_atom
	:	string_expr_part;

parens
	:	LPAREN BLANK? RPAREN;

compound_command
	:	for_expr
	|	select_expr
	|	if_expr
	|	while_expr
	|	until_expr
	|	case_expr
	|	subshell
	|	current_shell
	|	arithmetic_expression
	|	condition_comparison;

semiel
	:	BLANK? SEMIC wspace?
	|	BLANK? EOL wspace?;

for_expr
	:	FOR BLANK?
		(
			name wspace
			(
				IN for_each_value* (SEMIC|EOL) wspace?
				|SEMIC wspace?
				|
			) DO wspace command_list semiel DONE -> ^(FOR name for_each_value* command_list)
			|	LLPAREN EOL?
				// initilization
				(BLANK? init=arithmetic BLANK?|BLANK)?
				// condition
				(SEMIC (BLANK? fcond=arithmetic BLANK?|BLANK)? SEMIC|DOUBLE_SEMIC)
				// modification
				(BLANK? mod=arithmetic)? wspace? RPAREN RPAREN semiel DO wspace command_list semiel DONE
					-> ^(CFOR ^(FOR_INIT $init)? ^(FOR_COND $fcond)? command_list ^(FOR_MOD $mod)?)
		);
for_each_value
	:	{LA(1) == BLANK && LA(2) != EOL && LA(2) != SEMIC && LA(2) != DO}?
			=> (BLANK! string_expr);

select_expr
	:	SELECT BLANK name (wspace IN BLANK string_expr)? semiel DO wspace command_list semiel DONE
			-> ^(SELECT name string_expr? command_list) ;
if_expr
	:	IF wspace ag=command_list semiel THEN wspace iflist=command_list semiel
		(elif_expr)*
		(ELSE wspace else_list=command_list semiel)? FI
			-> ^(IF_STATEMENT ^(IF $ag $iflist) (elif_expr)* ^(ELSE $else_list)?);
elif_expr
	:	ELIF BLANK ag=command_list semiel THEN wspace iflist=command_list semiel
			-> ^(IF["if"] $ag $iflist);
while_expr
	:	WHILE wspace? istrue=command_list semiel DO wspace dothis=command_list semiel DONE
			-> ^(WHILE $istrue $dothis);
until_expr
	:	UNTIL wspace? istrue=command_list semiel DO wspace dothis=command_list semiel DONE
			-> ^(UNTIL $istrue $dothis);

case_expr
	:	CASE BLANK string_expr wspace IN case_body -> ^(CASE string_expr case_body);
case_body
scope {
#ifdef OUTPUT_C
	bool case_end;
#else
	boolean case_end;
#endif
}
	:	{$case_body::case_end = false;}
		(
			(wspace ESAC) => (wspace ESAC) -> ^(CASE_PATTERN)
			|({!$case_body::case_end}? => case_statement)+ -> case_statement+
		);
case_statement
	:	wspace? (LPAREN BLANK?)? extended_pattern (BLANK? PIPE BLANK? extended_pattern)* BLANK? RPAREN
		wspace
		(
			command_list wspace)? ( (DOUBLE_SEMIC ((wspace ESAC) => wspace ESAC {$case_body::case_end = true;})?)
			|(ESAC) => ESAC {$case_body::case_end = true;}
		)
			-> ^(CASE_PATTERN extended_pattern+ (CASE_COMMAND command_list)?);

subshell
	:	LPAREN wspace? command_list (BLANK? SEMIC)? wspace? RPAREN -> ^(SUBSHELL command_list);

current_shell
	:	LBRACE wspace command_list semiel RBRACE -> ^(CURRENT_SHELL command_list);

arithmetic_expression
	:	LLPAREN wspace? arithmetic wspace? RPAREN RPAREN -> ^(ARITHMETIC_EXPRESSION arithmetic);
condition_comparison
	:	condition_expr -> ^(COMPOUND_COND condition_expr);

condition_expr
	:	LSQUARE LSQUARE wspace keyword_condition wspace RSQUARE RSQUARE -> ^(KEYWORD_TEST keyword_condition)
	|	LSQUARE wspace builtin_condition wspace RSQUARE -> ^(BUILTIN_TEST builtin_condition)
	|	TEST_EXPR wspace builtin_condition-> ^(BUILTIN_TEST builtin_condition);

keyword_condition
	:	((BANG) => keyword_negation_primary|keyword_condition_primary) (BLANK!? (LOGICOR^|LOGICAND^) BLANK!? keyword_condition)?;
keyword_negation_primary
	:	BANG BLANK keyword_condition_primary -> ^(NEGATION keyword_condition_primary);
keyword_condition_primary
	:	LPAREN! BLANK!? keyword_condition BLANK!? RPAREN!
	|	keyword_condition_binary
	|	(unary_operator) => keyword_condition_unary;
keyword_condition_unary
	:	unary_operator^ BLANK! condition_part;
keyword_condition_binary
	:	condition_part
		(
			(BLANK? EQUALS TILDE) => BLANK? EQUALS TILDE BLANK? bash_pattern_part
				-> ^(MATCH_REGULAR_EXPRESSION condition_part ^(STRING bash_pattern_part))
			|	BLANK? keyword_binary_string_operator BLANK? right=condition_part
					-> ^(keyword_binary_string_operator condition_part $right)
			|	BLANK? (BANG EQUALS) BLANK? extended_pattern_match+
					-> ^(NOT_MATCH_PATTERN condition_part extended_pattern_match+)
			|	BLANK? (EQUALS EQUALS) BLANK? extended_pattern_match+
					-> ^(MATCH_PATTERN condition_part extended_pattern_match+)
		)?;
//TODO improve this rule
bash_pattern_part
	:(
		(ESC BLANK) => ESC BLANK
		|	(ESC RSQUARE) => ESC RSQUARE
		|	~(BLANK|RSQUARE|EOL|LOGICAND|LOGICOR|RPAREN)
	 )+;
keyword_binary_string_operator
	:	binary_operator
	|	EQUALS
	|	LESS_THAN
	|	GREATER_THAN;

builtin_condition
	:	((BANG) => builtin_negation_primary|builtin_keyword_condition_primary)
		(BLANK!? builtin_logic_operator^ BLANK!? builtin_condition)?;
builtin_negation_primary
	:	BANG BLANK builtin_keyword_condition_primary -> ^(NEGATION builtin_keyword_condition_primary);
builtin_keyword_condition_primary
	:	LPAREN! BLANK!? builtin_condition BLANK!? RPAREN!
	|	builtin_condition_binary
	|	builtin_condition_unary;
builtin_condition_unary
	:	unary_operator^ BLANK! condition_part;
builtin_condition_binary
	:	condition_part (BLANK!? builtin_binary_string_operator^ BLANK!? condition_part)?;
builtin_binary_string_operator
	:	binary_operator
	|	(EQUALS EQUALS) => EQUALS EQUALS -> EQUALS
	|	EQUALS
	|	BANG EQUALS -> NOT_EQUALS
	|	ESC_LT
	|	ESC_GT;
builtin_logic_operator
	:	unary_operator -> ^(BUILTIN_LOGIC unary_operator);

binary_operator
	:	MINUS! NAME^;
unary_operator
	:	MINUS! LETTER;

// TODO support brace expansion
condition_part
	:	name -> ^(STRING name);

name
	:	NAME |	LETTER | UNDERSCORE;

num
options{k=1;}
	:	DIGIT|NUMBER;

string_expr
	:	(~POUND) => string_expr_part string_expr_part* -> ^(STRING string_expr_part+);

string_expr_part
	:	quoted_string | non_quoted_string | reserved_word;

string_expr_no_reserved_word
	:	(~POUND) =>
			(
				non_quoted_string string_expr_part* -> ^(STRING non_quoted_string string_expr_part*)
				|	quoted_string string_expr_part* -> ^(STRING quoted_string string_expr_part*)
			);

reserved_word
	:	CASE|DO|DONE|ELIF|ELSE|ESAC|FI|FOR|FUNCTION|IF|IN|SELECT|THEN|UNTIL|WHILE|TIME;

non_quoted_string
	:	string_part
	|	variable_reference
	|	command_substitution
	|	arithmetic_expansion
	|	brace_expansion
	|	BANG
	|	DOLLAR SINGLE_QUOTED_STRING_TOKEN -> ^(ANSI_C_QUOTING SINGLE_QUOTED_STRING_TOKEN);

quoted_string
	:	double_quoted_string
	|	SINGLE_QUOTED_STRING_TOKEN -> ^(SINGLE_QUOTED_STRING SINGLE_QUOTED_STRING_TOKEN);

double_quoted_string
	:	DQUOTE double_quoted_string_part* DQUOTE -> ^(DOUBLE_QUOTED_STRING double_quoted_string_part*);
double_quoted_string_part
	:	(DOLLAR (LBRACE|name|num|TIMES|AT|POUND|QMARK|MINUS|DOLLAR|BANG)) => variable_reference
	|	(command_substitution) => command_substitution
	|	(DOLLAR (LLPAREN|LSQUARE)) => arithmetic_expansion
	|	(ESC DQUOTE) => ESC DQUOTE -> DQUOTE
	|	(ESC TICK) => ESC TICK -> TICK
	|	(ESC DOLLAR) => ESC DOLLAR -> DOLLAR
	|	~(TICK|DQUOTE);

// Perform all kinds of expansions
all_expansions
	:	expansion_atom+ -> ^(STRING expansion_atom+);
expansion_atom
	:	(DQUOTE) => double_quoted_string
	|	double_quoted_string_part;

string_part
	:	ns_string_part
	|	SLASH;

ns_string_part
	:	num|name|escaped_character
	|OTHER|EQUALS|PCT|PCTPCT|PLUS|MINUS|DOT|DOTDOT|COLON|TEST_EXPR
	|TILDE|MUL_ASSIGN|DIVIDE_ASSIGN|MOD_ASSIGN|PLUS_ASSIGN|MINUS_ASSIGN
	|LSHIFT_ASSIGN|RSHIFT_ASSIGN|AND_ASSIGN|XOR_ASSIGN|LSQUARE|RSQUARE
	|OR_ASSIGN|CARET|POUND|POUNDPOUND|COMMA|EXPORT|LOCAL;

escaped_character
	:	ESC
		(
			(DIGIT) => DIGIT
			|	(DIGIT DIGIT) => DIGIT DIGIT
			|	(DIGIT DIGIT DIGIT) => DIGIT DIGIT DIGIT
			|	LETTER ALPHANUM ALPHANUM?
			|	.
		);

extended_pattern_match
	:	(QMARK LPAREN) => QMARK LPAREN extended_pattern (PIPE extended_pattern)* RPAREN
			-> ^(EXTENDED_MATCH_AT_MOST_ONE extended_pattern+)
	|	(TIMES LPAREN) => TIMES LPAREN extended_pattern (PIPE extended_pattern)* RPAREN
			-> ^(EXTENDED_MATCH_ANY extended_pattern+)
	|	(PLUS LPAREN) => PLUS LPAREN extended_pattern (PIPE extended_pattern)* RPAREN
			-> ^(EXTENDED_MATCH_AT_LEAST_ONE extended_pattern+)
	|	(AT LPAREN) => AT LPAREN extended_pattern (PIPE extended_pattern)* RPAREN
			-> ^(EXTENDED_MATCH_EXACTLY_ONE extended_pattern+)
	|	(BANG LPAREN) => BANG LPAREN extended_pattern (PIPE extended_pattern)* RPAREN
			-> ^(EXTENDED_MATCH_NONE extended_pattern+)
	|	(bracket_pattern_match) => bracket_pattern_match
	|	(pattern_class_match) => pattern_class_match
	|	string_expr_part;

extended_pattern
	:	((~(RPAREN|PIPE)) => extended_pattern_match)+ -> ^(BRANCH extended_pattern_match+);

bracket_pattern_match
	:	LSQUARE! bracket_pattern_match_operator^ bracket_pattern RSQUARE!
	|	TIMES -> MATCH_ALL
	|	QMARK -> MATCH_ONE;
bracket_pattern_match_operator
	:	(BANG) => BANG -> MATCH_ANY_EXCEPT
	|	(CARET) => CARET -> MATCH_ANY_EXCEPT
	|	-> MATCH_ANY;

bracket_pattern_part
	:	(pattern_class_match) => pattern_class_match
	|	string_expr_part;

bracket_pattern
	:	((~RSQUARE) => bracket_pattern_part)+;

pattern_class_match
	:	LSQUARE COLON NAME COLON RSQUARE -> ^(CHARACTER_CLASS NAME)
	|	LSQUARE EQUALS pattern_char EQUALS RSQUARE -> ^(EQUIVALENCE_CLASS pattern_char)
	|	LSQUARE DOT NAME DOT RSQUARE -> ^(COLLATING_SYMBOL NAME);

pattern_char
	:	LETTER|DIGIT|OTHER|QMARK|COLON|AT|SEMIC|POUND|SLASH
		|BANG|TIMES|COMMA|PIPE|AMP|MINUS|PLUS|PCT|LSQUARE|RSQUARE
		|RPAREN|LPAREN|RBRACE|LBRACE|DOLLAR|TICK|DOT|LESS_THAN
		|GREATER_THAN|SQUOTE|DQUOTE;

variable_reference
	:	DOLLAR LBRACE parameter_expansion RBRACE -> ^(VAR_REF parameter_expansion)
	|	DOLLAR name -> ^(VAR_REF name)
	|	DOLLAR num -> ^(VAR_REF num)
	|	DOLLAR TIMES -> ^(VAR_REF TIMES)
	|	DOLLAR AT -> ^(VAR_REF AT)
	|	DOLLAR POUND -> ^(VAR_REF POUND)
	|	DOLLAR QMARK -> ^(VAR_REF QMARK)
	|	DOLLAR MINUS -> ^(VAR_REF MINUS)
	|	DOLLAR DOLLAR -> ^(VAR_REF DOLLAR)
	|	DOLLAR BANG -> ^(VAR_REF BANG);

parameter_expansion
	:	variable_name
		(
			(parameter_value_operator) => parameter_value_operator parameter_expansion_value
				-> ^(parameter_value_operator variable_name parameter_expansion_value)
			|	COLON BLANK? os=explicit_arithmetic (COLON BLANK? len=explicit_arithmetic)?
				-> ^(OFFSET variable_name $os ^($len)?)
			|	parameter_delete_operator parameter_replace_pattern
				-> ^(parameter_delete_operator variable_name parameter_replace_pattern)
			|	parameter_replace_operator parameter_replace_pattern (SLASH parameter_expansion_value)?
				-> ^(parameter_replace_operator variable_name parameter_replace_pattern parameter_expansion_value?)
			|	BLANK? -> variable_name
		)
		|	BANG variable_name_for_bang
			(
				TIMES -> ^(BANG variable_name_for_bang TIMES)
				|	AT -> ^(BANG variable_name_for_bang AT)
				|	LSQUARE (op=TIMES|op=AT) RSQUARE -> ^(LIST_EXPAND variable_name_for_bang $op)
				|	-> ^(VAR_REF variable_name_for_bang)
			)
		|	variable_size_ref;
parameter_delete_operator
	:	POUND -> LAZY_REMOVE_AT_START
	|	POUNDPOUND -> REPLACE_AT_START
	|	PCT -> LAZY_REMOVE_AT_END
	|	PCTPCT -> REPLACE_AT_END;
parameter_value_operator
	:	COLON MINUS -> USE_DEFAULT_WHEN_UNSET_OR_NULL
	|	COLON EQUALS -> ASSIGN_DEFAULT_WHEN_UNSET_OR_NULL
	|	COLON QMARK -> DISPLAY_ERROR_WHEN_UNSET_OR_NULL
	|	COLON PLUS -> USE_ALTERNATE_WHEN_UNSET_OR_NULL
	|	MINUS -> USE_DEFAULT_WHEN_UNSET
	|	EQUALS -> ASSIGN_DEFAULT_WHEN_UNSET
	|	QMARK -> DISPLAY_ERROR_WHEN_UNSET
	|	PLUS -> USE_ALTERNATE_WHEN_UNSET;
parameter_replace_pattern
	:	((~SLASH) => parameter_pattern_part)+ -> ^(STRING parameter_pattern_part+);
parameter_pattern_part
	:	extended_pattern_match|BLANK|SEMIC;

// TODO fix this rule
parameter_expansion_value
scope {
	int num_of_braces;
}
	:	parameter_expansion_value_atom -> ^(STRING parameter_expansion_value_atom);

parameter_expansion_value_atom
	:	(~RBRACE) =>
			{$parameter_expansion_value::num_of_braces = 1;}
			(
				{$parameter_expansion_value::num_of_braces != 0}? => .
				{
					if(LA(1) == LBRACE && LA(-1) != ESC)
						++$parameter_expansion_value::num_of_braces;
					else if(LA(1) == RBRACE && LA(-1) != ESC)
						--$parameter_expansion_value::num_of_braces;
				}
			)+
	|	-> EMPTY_EXPANSION_VALUE;

parameter_replace_operator
	:	(SLASH SLASH) => SLASH SLASH -> REPLACE_ALL
	|	(SLASH PCT) => SLASH PCT -> REPLACE_AT_END
	|	(SLASH POUND) => SLASH POUND -> REPLACE_AT_START
	|	SLASH -> REPLACE_FIRST;

variable_name
	:	num
	|	name LSQUARE AT RSQUARE -> ^(ARRAY name AT)
	|	name LSQUARE TIMES RSQUARE -> ^(ARRAY name TIMES)
	|	variable_name_no_digit
	|	DOLLAR
	|	TIMES
	|	AT
	|	POUND;

variable_name_no_digit
	:	name LSQUARE explicit_arithmetic RSQUARE -> ^(name explicit_arithmetic)
	|	name;

variable_name_for_bang
	:	num|name|POUND;
variable_size_ref
	:	POUND name LSQUARE array_size_index RSQUARE -> ^(POUND ^(name array_size_index))
	|	POUND^ name;
array_size_index
	:	DIGIT+
	|	(AT|TIMES) -> ARRAY_SIZE;

wspace
	:	(BLANK|EOL)+;

command_substitution
	:	COMMAND_SUBSTITUTION_PAREN -> ^(COMMAND_SUB COMMAND_SUBSTITUTION_PAREN)
	|	COMMAND_SUBSTITUTION_TICK -> ^(COMMAND_SUB COMMAND_SUBSTITUTION_TICK);

brace_expansion
	:	LBRACE BLANK* brace_expansion_inside BLANK* RBRACE -> ^(BRACE_EXP brace_expansion_inside);
brace_expansion_inside
	:	commasep|range;
range
	:	DIGIT DOTDOT^ DIGIT
	|	LETTER DOTDOT^ LETTER;
brace_expansion_part
	:	((~COMMA) => string_expr_part)* -> ^(STRING string_expr_part*);
commasep
	:	brace_expansion_part (COMMA! brace_expansion_part)+;

explicit_arithmetic
	:	arithmetic_part
	|	arithmetics;

arithmetic_expansion
	:	arithmetic_part -> ^(ARITHMETIC_EXPRESSION arithmetic_part);

arithmetic_part
	:	DOLLAR LLPAREN BLANK? arithmetics BLANK? RPAREN RPAREN -> arithmetics
	|	DOLLAR LSQUARE BLANK? arithmetics BLANK? RSQUARE -> arithmetics;

arithmetics
	:	arithmetic (COMMA! BLANK!? arithmetic)*;

arithmetics_test
	:	arithmetics EOF!;

arithmetic
	:(variable_name_no_digit BLANK? arithmetic_assignment_operator)
		=> variable_name_no_digit BLANK!? arithmetic_assignment_operator^ BLANK!? logicor
	|	(arithmetic_variable_reference BLANK? arithmetic_assignment_operator)
			=> arithmetic_variable_reference BLANK!? arithmetic_assignment_operator^ BLANK!? logicor
	|	cnd=logicor
		(
			QMARK t=logicor COLON f=logicor -> ^(ARITHMETIC_CONDITION $cnd $t $f)
			|	-> $cnd
		);

arithmetic_assignment_operator
	:	EQUALS|MUL_ASSIGN|DIVIDE_ASSIGN|MOD_ASSIGN|PLUS_ASSIGN|MINUS_ASSIGN|LSHIFT_ASSIGN|RSHIFT_ASSIGN|AND_ASSIGN|XOR_ASSIGN|OR_ASSIGN;

arithmetic_variable_reference
	:	variable_reference -> ^(VAR_REF variable_reference);
primary
	:	num
	|	command_substitution
	|	variable_name_no_digit -> ^(VAR_REF variable_name_no_digit)
	|	variable_reference
	|	LPAREN! (arithmetics) RPAREN!;
pre_post_primary
	:	primary;
post_inc_dec
	:	pre_post_primary ((BLANK) => BLANK)?
		(
			(PLUS PLUS) => BLANK? PLUS PLUS -> ^(POST_INCR pre_post_primary)
			|	(MINUS MINUS) => BLANK? MINUS MINUS -> ^(POST_DECR pre_post_primary)
			|	-> pre_post_primary
		);
pre_inc_dec
	:	PLUS PLUS BLANK? pre_post_primary -> ^(PRE_INCR pre_post_primary)
	|	MINUS MINUS BLANK? pre_post_primary -> ^(PRE_DECR pre_post_primary);
unary_with_operator
	:	PLUS BLANK? unary -> ^(PLUS_SIGN unary)
	|	MINUS BLANK? unary -> ^(MINUS_SIGN unary)
	|	TILDE BLANK? unary -> ^(TILDE unary)
	|	BANG BLANK? unary -> ^(BANG unary);
unary
	:	post_inc_dec
	|	(PLUS PLUS|MINUS MINUS) => pre_inc_dec
	|	(PLUS|MINUS|TILDE|BANG) => unary_with_operator;
exponential
	:	unary (EXP^ BLANK!? unary)* ;
times_division_modulus
	:	exponential ((TIMES^|SLASH^|PCT^) BLANK!? exponential)*;
addsub
	:	times_division_modulus ((PLUS^|MINUS^) BLANK!? times_division_modulus)*;
shifts
	:	addsub ((LSHIFT^|RSHIFT^) BLANK!? addsub)*;
compare
	:	shifts (compare_operator^ BLANK!? shifts)?;
compare_operator
	:	LEQ
	|	GEQ
	|	LESS_THAN
	|	GREATER_THAN
	|	BANG EQUALS -> NOT_EQUALS;
bitwiseand
	:	compare (AMP^ BLANK!? compare)*;
bitwisexor
	:	bitwiseand (CARET^ BLANK!? bitwiseand)*;
bitwiseor
	:	bitwisexor (PIPE^ BLANK!? bitwisexor)*;
logicand
	:	bitwiseor (LOGICAND^ BLANK!? bitwiseor)*;
logicor
	:	logicand (LOGICOR^ BLANK!? logicand)*;

COMMENT
	:	{ !double_quoted }?=> (BLANK|EOL) '#' ~('\n'|'\r')* {$channel=HIDDEN;}
	;

BANG	:	'!';
CASE	:	'case';
DO		:	'do';
DONE	:	'done';
ELIF	:	'elif';
ELSE	:	'else';
ESAC	:	'esac';
FI		:	'fi';
FOR		:	'for';
FUNCTION:	'function';
IF		:	'if';
IN		:	'in';
SELECT	:	'select';
THEN	:	'then';
UNTIL	:	'until';
WHILE	:	'while';
LBRACE	:	'{';
RBRACE	:	'}';
TIME	:	'time';
RPAREN	:	')';
LPAREN	:	'(';
LLPAREN	:	'((';
LSQUARE	:	'[';
RSQUARE	:	']';
TICK	:	'`';
DOLLAR	:	'$';
AT	:	'@';
DOT	:	'.';
DOTDOT	:	'..';

TIMES	:	'*';
EQUALS	:	'=';
MINUS	:	'-';
PLUS	:	'+';
EXP		:	'**';
AMP		:	'&';
LEQ		:	'<=';
GEQ		:	'>=';
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

SEMIC	:	';';
DOUBLE_SEMIC	:	';;';
PIPE	:	'|';
DQUOTE	:	'"' { if(LA(-1) != '\\') double_quoted = !double_quoted; };
SQUOTE	:	{ double_quoted }? => '\'';
SINGLE_QUOTED_STRING_TOKEN	:	{ !double_quoted }? => '\'' .* '\'';
COMMA	:	',';
BLANK	:	(' '|'\t')+;
EOL		:	('\r'?'\n')+ ;

DIGIT	:	'0'..'9';
NUMBER	:	DIGIT DIGIT+;
LETTER	:	('a'..'z'|'A'..'Z');
fragment
ALPHANUM	:	(DIGIT|LETTER);

TILDE	:	'~';
HERE_STRING_OP	:	'<<<';
POUND	:	'#';
POUNDPOUND	:	'##';
PCT		:	'%';
PCTPCT	:	'%%';
SLASH	:	'/';
COLON	:	':';
QMARK	:	'?';

TEST_EXPR	:	'test';
LOCAL	:	'local';
EXPORT	:	'export';
LOGICAND	:	'&&';
LOGICOR	:	'||';

CONTINUE_LINE	:	(ESC EOL)+{$channel=HIDDEN;};
ESC_RPAREN	:	ESC RPAREN;
ESC_LPAREN	:	ESC LPAREN;
ESC_DOLLAR	:	ESC DOLLAR;
ESC_TICK	:	ESC TICK;
COMMAND_SUBSTITUTION_PAREN
	:	{LA(1) == '$' && LA(2) == '(' && LA(3) != '('}? =>
			(DOLLAR LPAREN ({ paren_level = 1; }
				(
					ESC_LPAREN
					|	ESC_RPAREN
					|	LPAREN { ++paren_level; }
					|	RPAREN
						{
							if(--paren_level == 0)
							{
#ifdef OUTPUT_C
								LEXSTATE->type = _type;
#else
								state.type = _type;
								state.channel = _channel;
#endif
								return;
							}
						}
					|	.
				)+
			));
COMMAND_SUBSTITUTION_TICK	:	TICK (~(TICK))+ TICK;
ESC_LT	:	ESC'<';
ESC_GT	:	ESC'>';

ESC	:	'\\';
UNDERSCORE	:	'_';
NAME	:	(LETTER|UNDERSCORE)(ALPHANUM|UNDERSCORE)+;
OTHER	:	.;
