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

#include <memory>
#include <string>

class interpreter;
void set_interpreter(std::shared_ptr<interpreter> w);

}

@postinclude{

#include "core/interpreter.h"
#include <boost/format.hpp>

}

@members{

static std::shared_ptr<interpreter> walker;

void set_interpreter(std::shared_ptr<interpreter> w)
{
	walker = w;
}

inline void set_index(const std::string& name, unsigned& index, int value)
{
	if(value < 0)
		throw interpreter_exception((boost::format("Array index is less than 0: \%s[\%d]") \% name \% value).str());
	index = value;
}

// Recursively count number of nodes of curr
static int count_nodes(pANTLR3_BASE_TREE_ADAPTOR adaptor, pANTLR3_BASE_TREE curr)
{
	int child_count = adaptor->getChildCount(adaptor, curr);
	if(child_count == 0)
	{
		// Leaf node
		return 1;
	}
	else
	{
		int result = 0;
		// Count every child
		for(int i = 0; i != child_count; ++i)
			result += count_nodes(adaptor, (pANTLR3_BASE_TREE)(adaptor->getChild(adaptor, curr, i)));
		// Add itself, DOWN and UP
		return result + 3;
	}
}

}

start: list|EOF;

list:
	^(LIST (function_def|logic_command_list)+);

variable_definitions: ^(VARIABLE_DEFINITIONS var_def+);

name_base	returns[std::string libbash_value]:
	NAME {$libbash_value = walker->get_string($NAME);}
	|	LETTER {$libbash_value = walker->get_string($LETTER);}
	|	'_' {$libbash_value="_";};

name	returns[std::string libbash_value, unsigned index]
@init {
	$index = 0;
}:
	^(libbash_name=name_base value=arithmetics){
		set_index(libbash_name, $index, value);
		$libbash_value = libbash_name;
	}
	|libbash_name=name_base{
		$libbash_value = libbash_name;
	};

num returns[std::string libbash_value]
options{ k=1; }:
	DIGIT { $libbash_value = walker->get_string($DIGIT); }
	|NUMBER { $libbash_value = walker->get_string($NUMBER); };

var_def
@declarations {
	std::map<int, std::string> values;
	unsigned index = 0;
	bool is_null = true;
}:
	^(EQUALS name (libbash_string=string_expr { is_null = false; })?){
		walker->set_value($name.libbash_value, libbash_string, $name.index, is_null);
	}
	|^(EQUALS libbash_name=name_base ^(ARRAY (
										(libbash_string=string_expr
										|^(EQUALS value=arithmetics {
												set_index(libbash_name, index, value);
											} libbash_string=string_expr))
										{ values[index++] = libbash_string; })*
									 )){
		walker->define(libbash_name, values);
	};

string_expr	returns[std::string libbash_value]:
	^(STRING(
		(DOUBLE_QUOTED_STRING) => ^(DOUBLE_QUOTED_STRING (libbash_string=double_quoted_string { $libbash_value += libbash_string; })*)
		|(ARITHMETIC_EXPRESSION) => ^(ARITHMETIC_EXPRESSION value=arithmetics { $libbash_value = boost::lexical_cast<std::string>(value); })
		|(var_ref[false]) => libbash_string=var_ref[false] { $libbash_value = libbash_string; }
		|(libbash_string=any_string { $libbash_value += libbash_string; })+
	));

//double quoted string rule, allows expansions
double_quoted_string returns[std::string libbash_value]:
	(var_ref[true]) => libbash_string=var_ref[true] { $libbash_value = libbash_string; }
	|(ARITHMETIC_EXPRESSION) => ^(ARITHMETIC_EXPRESSION value=arithmetics) { $libbash_value = boost::lexical_cast<std::string>(value); }
	|libbash_string=any_string { $libbash_value = libbash_string; };

any_string returns[std::string libbash_value]
@declarations {
	pANTLR3_BASE_TREE any_token;
}:
	any_token=. { $libbash_value = walker->get_string(any_token); };

//Allowable variable names in the variable expansion
var_name returns[std::string libbash_value, unsigned index]
@init {
	$var_name.index = 0;
}:
	libbash_string=num { $libbash_value = libbash_string; }
	|name {
		$libbash_value = $name.libbash_value;
		$index = $name.index;
	};

var_expansion returns[std::string libbash_value]:
	^(USE_DEFAULT_WHEN_UNSET_OR_NULL var_name libbash_word=word) {
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
	));

word returns[std::string libbash_value]:
	|(num) => libbash_string=num { $libbash_value = libbash_string; }
	|libbash_string=string_expr { $libbash_value = libbash_string; }
	|value=arithmetics { $libbash_value = boost::lexical_cast<std::string>(value); };

//variable reference
var_ref [bool double_quoted] returns[std::string libbash_value]:
	^(VAR_REF name) { $libbash_value = walker->resolve<std::string>($name.libbash_value, $name.index); }
	|^(VAR_REF ^(libbash_string=name_base AT)) { walker->get_all_elements(libbash_string, $libbash_value); }
	|^(VAR_REF ^(libbash_string=name_base TIMES)) {
		if(double_quoted)
			walker->get_all_elements_IFS_joined(libbash_string, $libbash_value);
		else
			walker->get_all_elements(libbash_string, $libbash_value);
	}
	|^(VAR_REF libbash_string=var_expansion) { $libbash_value = libbash_string; };

command:
	variable_definitions
	|simple_command;

simple_command:
	^(COMMAND libbash_string=string_expr argument* var_def*) {
		ANTLR3_MARKER func_index;
		if((func_index = walker->get_function_index(libbash_string)) != -1)
		{
			// Saving current index
			ANTLR3_MARKER curr = INDEX();
			// Push function index into INPUT
			INPUT->push(INPUT, func_index);
			// Execute function body
			compound_command(ctx);
			// Reset to the previous index
			SEEK(curr);
		}
		else
		{
			throw interpreter_exception("Unimplemented command: " + libbash_string);
		}
	};

argument:
	var_ref[false]
	|string_expr;

logic_command_list:
	command
	| ^(LOGICAND command command)
	| ^(LOGICOR command command);

command_list:
	^(LIST logic_command_list+);

compound_command:
	^(CURRENT_SHELL command_list);

function_def returns[int placeholder]:
	^(FUNCTION ^(STRING name) {
		// Define the function with current index
		walker->define_function($name.libbash_value, INDEX());
		// Skip the AST for function body, minus one is needed to make the offset right.
		// LT(1) is the function body. It should match the compound_command rule.
		SEEK(INDEX() + count_nodes(ADAPTOR, LT(1)) - 1);
		// After seeking ahead, we need to call CONSUME to eat all the nodes we've skipped.
		CONSUME();
	});

// Only used in arithmetic expansion
primary returns[std::string libbash_value, unsigned index]:
	(^(VAR_REF name)) => ^(VAR_REF name) {
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
:
	^(LOGICOR l=arithmetics r=arithmetics){ $value = walker->logicor(l, r); }
	|^(LOGICAND l=arithmetics r=arithmetics){ $value = walker->logicand(l, r); }
	|^(PIPE l=arithmetics r=arithmetics){ $value = walker->bitwiseor(l, r); }
	|^(CARET l=arithmetics r=arithmetics){ $value = walker->bitwisexor(l, r); }
	|^(AMP l=arithmetics r=arithmetics){ $value = walker->bitwiseand(l, r); }
	|^(LEQ l=arithmetics r=arithmetics){ $value = walker->less_equal_than(l, r); }
	|^(GEQ l=arithmetics r=arithmetics){ $value = walker->greater_equal_than(l, r); }
	|^(LESS_THAN l=arithmetics r=arithmetics){ $value = walker->less_than(l, r); }
	|^(GREATER_THAN l=arithmetics r=arithmetics){ $value = walker->greater_than(l, r); }
	|^(LSHIFT l=arithmetics r=arithmetics){ $value = walker->left_shift(l, r); }
	|^(RSHIFT l=arithmetics r=arithmetics){ $value = walker->right_shift(l, r); }
	|^(PLUS l=arithmetics r=arithmetics){ $value = walker->plus(l, r); }
	|^(PLUS_SIGN l=arithmetics){ $value = l; }
	|^(MINUS l=arithmetics r=arithmetics){ $value = walker->minus(l, r); }
	|^(MINUS_SIGN l=arithmetics){ $value = -l; }
	|^(TIMES l=arithmetics r=arithmetics){ $value = walker->multiply(l, r); }
	|^(SLASH l=arithmetics r=arithmetics){ $value = walker->divide(l, r); }
	|^(PCT l=arithmetics r=arithmetics){ $value = walker->mod(l, r); }
	|^(EXP l=arithmetics r=arithmetics){ $value = walker->exp(l, r); }
	|^(BANG l=arithmetics){ $value = walker->negation(l); }
	|^(TILDE l=arithmetics){ $value = walker->bitwise_negation(l); }
	|^(ARITHMETIC_CONDITION cnd=arithmetics l=arithmetics r=arithmetics){
		$value = walker->arithmetic_condition(cnd, l, r);
	}
	|primary {
		$value = walker->resolve<int>($primary.libbash_value, $primary.index);
	}
	|^(PRE_INCR primary){ $value = walker->pre_incr($primary.libbash_value, $primary.index); }
	|^(PRE_DECR primary){ $value = walker->pre_decr($primary.libbash_value, $primary.index); }
	|^(POST_INCR primary){ $value = walker->post_incr($primary.libbash_value, $primary.index); }
	|^(POST_DECR primary){ $value = walker->post_decr($primary.libbash_value, $primary.index); }
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
