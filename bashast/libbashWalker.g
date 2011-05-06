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
tree grammar libbashWalker;

options
{
	language = C;
	tokenVocab = libbash;
	ASTLabelType = pANTLR3_BASE_TREE;
}

@includes{

	#include <string>
	#include <vector>

	class interpreter;
	void set_interpreter(interpreter* w);

}

@postinclude{

	#include <iostream>
	#include <sstream>

	#include <boost/format.hpp>

	#include "core/interpreter.h"
	#include "cppbash_builtin.h"

}

@members{

	static interpreter* walker = 0;

	void set_interpreter(interpreter* w)
	{
		walker = w;
	}

	inline void set_index(const std::string& name, unsigned& index, int value)
	{
		if(value < 0)
			throw interpreter_exception((boost::format("Array index is less than 0: \%s[\%d]") \% name \% value).str());
		index = value;
	}

	// seek to LT(2) and consume
	static void seek_to_next_tree(plibbashWalker ctx)
	{
		// We start from LA(1)
		int index = 1;
		// Current depth of the tree we are traversing
		int depth = 1;

		for(index = 1; depth != 0; ++index)
		{
			// Go one level done if we encounter DOWN
			if(LA(index) == DOWN)
				++depth;
			// Go one level up if we encounter UP. When depth==0, we finishe one node
			else if(LA(index) == UP)
				--depth;
		}

		// Seek to the correct offset and consume.
		SEEK(INDEX() + index - 3);
		CONSUME();
	}
}

start: list|EOF;

list: ^(LIST (function_def|logic_command_list)+);

variable_definitions: ^(VARIABLE_DEFINITIONS var_def+);

name_base returns[std::string libbash_value]
	:NAME { $libbash_value = walker->get_string($NAME); }
	|LETTER { $libbash_value = walker->get_string($LETTER); }
	|'_' { $libbash_value="_"; };

name returns[std::string libbash_value, unsigned index]
@init {
	$index = 0;
}
	:^(libbash_name=name_base value=arithmetics) {
		set_index(libbash_name, $index, value);
		$libbash_value = libbash_name;
	}
	|libbash_name=name_base {
		$libbash_value = libbash_name;
	};

num returns[std::string libbash_value]
options{ k=1; }
	:DIGIT { $libbash_value = walker->get_string($DIGIT); }
	|NUMBER { $libbash_value = walker->get_string($NUMBER); };

var_def
@declarations {
	std::map<int, std::string> values;
	unsigned index = 0;
}
	:^(EQUALS name string_expr?) {
		walker->set_value($name.libbash_value, $string_expr.libbash_value, $name.index);
	}
	|^(EQUALS libbash_name=name_base ^(ARRAY (
										(expr=string_expr
										|^(EQUALS value=arithmetics {
												set_index(libbash_name, index, value);
											} expr=string_expr))
										{ values[index++] = expr.libbash_value; })*
									 )){
		walker->define(libbash_name, values);
	};

string_expr returns[std::string libbash_value, bool quoted]
@init {
	$quoted = true;
}
	:^(STRING (
		(DOUBLE_QUOTED_STRING) =>
			^(DOUBLE_QUOTED_STRING (libbash_string=double_quoted_string { $libbash_value += libbash_string; })*)
		|(ARITHMETIC_EXPRESSION) =>
			^(ARITHMETIC_EXPRESSION value=arithmetics {
				$libbash_value += boost::lexical_cast<std::string>(value); $quoted = false;
			})
		|(var_ref[false]) => libbash_string=var_ref[false] { $libbash_value += libbash_string; $quoted = false; }
		|libbash_string=command_substitution { $libbash_value += libbash_string; $quoted = false; }
		|(libbash_string=any_string { $libbash_value += libbash_string; $quoted = false; })
	)+);

//double quoted string rule, allows expansions
double_quoted_string returns[std::string libbash_value]
	:(var_ref[true]) => libbash_string=var_ref[true] { $libbash_value = libbash_string; }
	|(ARITHMETIC_EXPRESSION) => ^(ARITHMETIC_EXPRESSION value=arithmetics) {
		$libbash_value = boost::lexical_cast<std::string>(value);
	}
	|libbash_string=any_string { $libbash_value = libbash_string; };

any_string returns[std::string libbash_value]
@declarations {
	pANTLR3_BASE_TREE any_token;
}
	:any_token=. { $libbash_value = walker->get_string(any_token); };

//Allowable variable names in the variable expansion
var_name returns[std::string libbash_value, unsigned index]
@init {
	$var_name.index = 0;
}
	:libbash_string=num { $libbash_value = libbash_string; }
	|name {
		$libbash_value = $name.libbash_value;
		$index = $name.index;
	};

var_expansion returns[std::string libbash_value]
	:^(USE_DEFAULT_WHEN_UNSET_OR_NULL var_name libbash_word=word) {
		libbash_value = walker->do_default_expansion($var_name.libbash_value, libbash_word, $var_name.index);
	}
	|^(ASSIGN_DEFAULT_WHEN_UNSET_OR_NULL var_name libbash_word=word) {
		libbash_value = walker->do_assign_expansion($var_name.libbash_value, libbash_word, $var_name.index);
	}
	|^(USE_ALTERNATE_WHEN_UNSET_OR_NULL var_name libbash_word=word) {
		libbash_value = walker->do_alternate_expansion($var_name.libbash_value, libbash_word, $var_name.index);
	}
	|(^(OFFSET var_name arithmetics arithmetics)) => ^(OFFSET var_name offset=arithmetics length=arithmetics) {
		libbash_value = walker->do_substring_expansion($var_name.libbash_value, offset, length, $var_name.index);
	}
	|^(OFFSET var_name offset=arithmetics) {
		libbash_value = walker->do_substring_expansion($var_name.libbash_value, offset, $var_name.index);
	}
	|^(POUND(
		var_name {
			libbash_value = boost::lexical_cast<std::string>(walker->get_length($var_name.libbash_value, $var_name.index));
		}
		|^(libbash_name=name_base ARRAY_SIZE) {
			libbash_value = boost::lexical_cast<std::string>(walker->get_array_length(libbash_name));
		}
	))
	|^(REPLACE_ALL var_name pattern=string_expr (replacement=string_expr)?) {
		libbash_value = walker->do_replace_expansion($var_name.libbash_value,
													 std::bind(&interpreter::replace_all,
															   std::placeholders::_1,
															   pattern.libbash_value,
															   replacement.libbash_value),
													 $var_name.index);
	}
	|^(REPLACE_AT_END var_name pattern=string_expr (replacement=string_expr)?) {
		libbash_value = walker->do_replace_expansion($var_name.libbash_value,
													 std::bind(&interpreter::replace_at_end,
															   std::placeholders::_1,
															   pattern.libbash_value,
															   replacement.libbash_value),
													 $var_name.index);
	}
	|^(REPLACE_AT_START var_name pattern=string_expr (replacement=string_expr)?) {
		libbash_value = walker->do_replace_expansion($var_name.libbash_value,
													 std::bind(&interpreter::replace_at_start,
															   std::placeholders::_1,
															   pattern.libbash_value,
															   replacement.libbash_value),
													 $var_name.index);
	}
	|^(REPLACE_FIRST var_name pattern=string_expr (replacement=string_expr)?) {
		libbash_value = walker->do_replace_expansion($var_name.libbash_value,
													 std::bind(&interpreter::replace_first,
															   std::placeholders::_1,
															   pattern.libbash_value,
															   replacement.libbash_value),
													 $var_name.index);
	}
	|^(LAZY_REMOVE_AT_START var_name pattern=string_expr) {
		libbash_value = walker->do_replace_expansion($var_name.libbash_value,
													 std::bind(&interpreter::lazy_remove_at_start,
															   std::placeholders::_1,
															   pattern.libbash_value),
													 $var_name.index);
	}
	|^(LAZY_REMOVE_AT_END var_name pattern=string_expr) {
		libbash_value = walker->do_replace_expansion($var_name.libbash_value,
													 std::bind(&interpreter::lazy_remove_at_end,
															   std::placeholders::_1,
															   pattern.libbash_value),
													 $var_name.index);
	};

word returns[std::string libbash_value]
	:(num) => libbash_string=num { $libbash_value = libbash_string; }
	|string_expr { $libbash_value = $string_expr.libbash_value; }
	|value=arithmetics { $libbash_value = boost::lexical_cast<std::string>(value); };

//variable reference
var_ref [bool double_quoted] returns[std::string libbash_value]
	:^(VAR_REF name) {
		$libbash_value = walker->resolve<std::string>($name.libbash_value, $name.index);
	}
	|^(VAR_REF libbash_string=num) {
		$libbash_value = walker->resolve<std::string>(libbash_string);
	}
	|^(VAR_REF ^(libbash_string=name_base AT)) { walker->get_all_elements(libbash_string, $libbash_value); }
	|^(VAR_REF ^(libbash_string=name_base TIMES)) {
		if(double_quoted)
			walker->get_all_elements_IFS_joined(libbash_string, $libbash_value);
		else
			walker->get_all_elements(libbash_string, $libbash_value);
	}
	|^(VAR_REF TIMES) { std::cerr << "$* has not been implemented yet" << std::endl; }
	|^(VAR_REF AT) { std::cerr << "$@ has not been implemented yet" << std::endl; }
	|^(VAR_REF POUND) { std::cerr << "$# has not been implemented yet" << std::endl; }
	|^(VAR_REF QMARK) { $libbash_value = walker->get_status<std::string>(); }
	|^(VAR_REF MINUS) { std::cerr << "$- has not been implemented yet" << std::endl; }
	|^(VAR_REF BANG) { std::cerr << "$! has not been implemented yet" << std::endl; }
	|^(VAR_REF libbash_string=var_expansion) { $libbash_value = libbash_string; };

command
	:variable_definitions
	|simple_command
	|compound_command;

simple_command
@declarations {
	std::vector<std::string> libbash_args;
}
	:^(COMMAND string_expr (argument[libbash_args])* var_def*) {
		if(walker->has_function($string_expr.libbash_value))
		{
			walker->set_status(walker->call($string_expr.libbash_value,
											libbash_args,
											ctx,
											compound_command));
		}
		else if(cppbash_builtin::is_builtin($string_expr.libbash_value))
		{
			walker->set_status(walker->execute_builtin($string_expr.libbash_value, libbash_args));
		}
		else
		{
			std::cerr << $string_expr.libbash_value << " is not supported yet" << std::endl;
			walker->set_status(1);
		}
	};

argument[std::vector<std::string>& args]
	: string_expr {
		if($string_expr.quoted)
			args.push_back($string_expr.libbash_value);
		else
			walker->split_word($string_expr.libbash_value, args);
	};

logic_command_list
@declarations {
	bool logic_and;
}
	:command
	|^((LOGICAND { logic_and = true; } | LOGICOR { logic_and = false; }) command {
		if(logic_and ? !walker->get_status() : walker->get_status())
			command(ctx);
		else
			seek_to_next_tree(ctx);
	});

command_list: ^(LIST logic_command_list+);

compound_command
	: ^(CURRENT_SHELL command_list)
	| ^(COMPOUND_COND cond_expr)
	| for_expr
	| while_expr
	| if_expr
	| case_expr;

cond_expr
	:^(BUILTIN_TEST status=builtin_condition) { walker->set_status(!status); };

builtin_condition returns[bool status]
	:^(NEGATION l=builtin_condition) { $status = !l; }
	|s=builtin_condition_primary { $status = s; };

builtin_condition_primary returns[bool status]
	:^(NAME string_expr string_expr) { throw interpreter_exception(walker->get_string($NAME) + "(NAME) is not supported for now");}
	|^(EQUALS l=string_expr r=string_expr) { $status = (l.libbash_value == r.libbash_value); }
	|^(NOT_EQUALS l=string_expr r=string_expr) { $status = (l.libbash_value != r.libbash_value); }
	|^(ESC_LT l=string_expr r=string_expr) { $status = (l.libbash_value < r.libbash_value); }
	|^(ESC_GT l=string_expr r=string_expr) { $status = (l.libbash_value > r.libbash_value); }
	|^(LETTER l=string_expr) { throw interpreter_exception(walker->get_string($LETTER) + "(LETTER) is not supported for now");}
	|string_expr { $status = ($string_expr.libbash_value.size() != 0); };

for_expr
@declarations {
	ANTLR3_MARKER commands_index;
	std::vector<std::string> splitted_values;

	ANTLR3_MARKER condition_index;
}
	:^(FOR libbash_string=name_base
		// Empty value as $@ is not supported currently
		(string_expr
		{
			// Word splitting happens here
			if($string_expr.quoted)
				splitted_values.push_back($string_expr.libbash_value);
			else
				walker->split_word($string_expr.libbash_value, splitted_values);
		}
		)+
		{
			commands_index = INDEX();
			for(auto iter = splitted_values.begin(); iter != splitted_values.end(); ++iter)
			{
				SEEK(commands_index);
				walker->set_value(libbash_string, *iter);
				command_list(ctx);
			}
		})
	|^(CFOR {
		// omit the first DOWN token for for_INIT
		SEEK(INDEX() + 1);

		if(LA(1) == FOR_INIT)
			for_initilization(ctx);

		condition_index = INDEX();
		bool has_condition = (LA(1) != FOR_COND);
		while(has_condition || for_condition(ctx))
		{
			command_list(ctx);
			if(LA(1) == FOR_MOD)
				for_modification(ctx);
			SEEK(condition_index);
		}

		// Get out of the loop
		// We are standing right after for_condition, we need to skip the command_list and optional for_modification
		seek_to_next_tree(ctx);
		if(LA(1) == FOR_MOD)
			seek_to_next_tree(ctx);

		// omit the last UP token
		SEEK(INDEX() + 1);
	});

for_initilization
	:^(FOR_INIT arithmetics);

for_condition returns[int libbash_value]
	:^(FOR_COND condition=arithmetics) { libbash_value = condition; };

for_modification
	:^(FOR_MOD arithmetics);

while_expr
@declarations {
	ANTLR3_MARKER command_index;
	bool negate;
}
	:^((WHILE { negate = false; } | UNTIL { negate = true; }) {
		// omit the first DOWN token
		SEEK(INDEX() + 1);

		command_index = INDEX();
		while(true)
		{
			command_list(ctx);
			if(walker->get_status() == (negate? 0 : 1))
				break;
			command_list(ctx);
			SEEK(command_index);
		}
		// Skip the body and get out
		seek_to_next_tree(ctx);

		// omit the last UP token
		SEEK(INDEX() + 1);
	});

if_expr
@declarations {
	bool matched = false;
}
	:^(IF_STATEMENT {
		// omit the first DOWN token
		SEEK(INDEX() + 1);

		while(LA(1) == IF)
		{
			if(matched)
				seek_to_next_tree(ctx);
			else
				matched = elif_expr(ctx);
		}

		if(LA(1) == ELSE)
		{
			if(matched == false)
			{
				// omit the ELSE DOWN tokens
				SEEK(INDEX() + 2);

				command_list(ctx);

				// omit the last UP token
				SEEK(INDEX() + 1);
			}
			else
			{
				seek_to_next_tree(ctx);
			}
		}

		// omit the last UP token
		SEEK(INDEX() + 1);
	});

elif_expr returns[bool matched]
	:^(IF {
		// omit the first DOWN token
		SEEK(INDEX() + 1);

		command_list(ctx);
		if(walker->get_status() == 0)
		{
			$matched=true;
			command_list(ctx);
		}
		else
		{
			$matched=false;
			seek_to_next_tree(ctx);
		}

		// omit the last UP token
		SEEK(INDEX() + 1);
	});

case_expr
	:^(CASE libbash_string=word (matched=case_clause[libbash_string]{
		if(matched)
		{
			while(LA(1) == CASE_PATTERN)
				seek_to_next_tree(ctx);
		}
	})*);

case_clause[const std::string& target] returns[bool matched]
@declarations {
	std::vector<std::string> patterns;
}
	:^(CASE_PATTERN (libbash_string=case_pattern { patterns.push_back(libbash_string); })+ {
		if(LA(1) == CASE_COMMAND)
		{
			// omit CASE_COMMAND
			SEEK(INDEX() + 1);
			matched = false;
			for(auto iter = patterns.begin(); iter != patterns.end(); ++iter)
			{
				// pattern matching should happen here in future
				if(*iter == "*" || *iter == target)
				{
					command_list(ctx);
					$matched = true;
				}
				else
				{
					seek_to_next_tree(ctx);
				}
			}
		}
	});

case_pattern returns[std::string libbash_value]
	:libbash_string=command_substitution { $libbash_value = libbash_string; }
	|string_expr { $libbash_value = $string_expr.libbash_value; }
	|TIMES { $libbash_value = "*"; };

command_substitution returns[std::string libbash_value]
@declarations {
	std::stringstream out;
}
	:^(COMMAND_SUB{ walker->set_output_stream(&out); } command_list) {
		walker->restore_output_stream();
		$libbash_value = out.str();
		walker->trim_trailing_eols($libbash_value);
	};

function_def returns[int placeholder]
	:^(FUNCTION ^(STRING name) {
		// Define the function with current index
		walker->define_function($name.libbash_value, INDEX());
		// Skip the AST for function body
		seek_to_next_tree(ctx);
	});

// Only used in arithmetic expansion
primary returns[std::string libbash_value, unsigned index]
	:(^(VAR_REF name)) => ^(VAR_REF name) {
		$libbash_value = $name.libbash_value;
		$index = $name.index;
	}
	|name {
		$libbash_value = $name.libbash_value;
		$index = $name.index;
	}
	// array[@] and array[*] is meaningless to arithmetic expansion so true/false are both ok.
	|^(VAR_REF libbash_string=var_ref[false]) {
		$libbash_value = libbash_string;
		$index = 0;
	};

// shell arithmetic
arithmetics returns[int value]
	:^(LOGICOR l=arithmetics r=arithmetics) { $value = walker->logicor(l, r); }
	|^(LOGICAND l=arithmetics r=arithmetics) { $value = walker->logicand(l, r); }
	|^(PIPE l=arithmetics r=arithmetics) { $value = walker->bitwiseor(l, r); }
	|^(CARET l=arithmetics r=arithmetics) { $value = walker->bitwisexor(l, r); }
	|^(AMP l=arithmetics r=arithmetics) { $value = walker->bitwiseand(l, r); }
	|^(LEQ l=arithmetics r=arithmetics) { $value = walker->less_equal_than(l, r); }
	|^(GEQ l=arithmetics r=arithmetics) { $value = walker->greater_equal_than(l, r); }
	|^(LESS_THAN l=arithmetics r=arithmetics) { $value = walker->less_than(l, r); }
	|^(GREATER_THAN l=arithmetics r=arithmetics) { $value = walker->greater_than(l, r); }
	|^(LSHIFT l=arithmetics r=arithmetics) { $value = walker->left_shift(l, r); }
	|^(RSHIFT l=arithmetics r=arithmetics) { $value = walker->right_shift(l, r); }
	|^(PLUS l=arithmetics r=arithmetics) { $value = walker->plus(l, r); }
	|^(PLUS_SIGN l=arithmetics) { $value = l; }
	|^(MINUS l=arithmetics r=arithmetics) { $value = walker->minus(l, r); }
	|^(MINUS_SIGN l=arithmetics) { $value = -l; }
	|^(TIMES l=arithmetics r=arithmetics) { $value = walker->multiply(l, r); }
	|^(SLASH l=arithmetics r=arithmetics) { $value = walker->divide(l, r); }
	|^(PCT l=arithmetics r=arithmetics) { $value = walker->mod(l, r); }
	|^(EXP l=arithmetics r=arithmetics) { $value = walker->exp(l, r); }
	|^(BANG l=arithmetics) { $value = walker->negation(l); }
	|^(TILDE l=arithmetics) { $value = walker->bitwise_negation(l); }
	|^(ARITHMETIC_CONDITION cnd=arithmetics l=arithmetics r=arithmetics) {
		$value = walker->arithmetic_condition(cnd, l, r);
	}
	|primary {
		$value = walker->resolve<int>($primary.libbash_value, $primary.index);
	}
	|^(PRE_INCR primary) { $value = walker->pre_incr($primary.libbash_value, $primary.index); }
	|^(PRE_DECR primary) { $value = walker->pre_decr($primary.libbash_value, $primary.index); }
	|^(POST_INCR primary) { $value = walker->post_incr($primary.libbash_value, $primary.index); }
	|^(POST_DECR primary) { $value = walker->post_decr($primary.libbash_value, $primary.index); }
	|^(EQUALS primary l=arithmetics) {
		$value = walker->set_value($primary.libbash_value, l, $primary.index);
	}
	|^(MUL_ASSIGN primary l=arithmetics) {
		$value = walker->assign(&interpreter::multiply, $primary.libbash_value, l, $primary.index);
	}
	|^(DIVIDE_ASSIGN primary l=arithmetics) {
		$value = walker->assign(&interpreter::divide, $primary.libbash_value, l, $primary.index);
	}
	|^(MOD_ASSIGN primary l=arithmetics) {
		$value = walker->assign(&interpreter::mod, $primary.libbash_value, l, $primary.index);
	}
	|^(PLUS_ASSIGN primary l=arithmetics) {
		$value = walker->assign(&interpreter::plus, $primary.libbash_value, l, $primary.index);
	}
	|^(MINUS_ASSIGN primary l=arithmetics) {
		$value = walker->assign(&interpreter::minus, $primary.libbash_value, l, $primary.index);
	}
	|^(LSHIFT_ASSIGN primary l=arithmetics) {
		$value = walker->assign(&interpreter::left_shift, $primary.libbash_value, l, $primary.index);
	}
	|^(RSHIFT_ASSIGN primary l=arithmetics) {
		$value = walker->assign(&interpreter::right_shift, $primary.libbash_value, l, $primary.index);
	}
	|^(AND_ASSIGN primary l=arithmetics) {
		$value = walker->assign(&interpreter::bitwiseand, $primary.libbash_value, l, $primary.index);
	}
	|^(XOR_ASSIGN primary l=arithmetics) {
		$value = walker->assign(&interpreter::bitwisexor, $primary.libbash_value, l, $primary.index);
	}
	|^(OR_ASSIGN primary l=arithmetics) {
		$value = walker->assign(&interpreter::bitwiseor, $primary.libbash_value, l, $primary.index);
	}
	| NUMBER { $value = walker->parse_int($NUMBER);}
	| DIGIT { $value = walker->parse_int($DIGIT);}
	;
