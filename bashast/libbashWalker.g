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

}

start: list|EOF;

list: ^(LIST var_def+);

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
	^(USE_DEFAULT var_name libbash_word=word) {
		libbash_value = walker->do_default_expansion($var_name.libbash_value, libbash_word, $var_name.index);
	}
	|^(ASSIGN_DEFAULT var_name libbash_word=word) {
		libbash_value = walker->do_assign_expansion($var_name.libbash_value, libbash_word, $var_name.index);
	}
	|^(USE_ALTERNATE var_name libbash_word=word) {
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
	|^(VAR_REF name) {
		$value = walker->resolve<int>($name.libbash_value);
	}
	|^(PRE_INCR ^(VAR_REF name)){ $value = walker->pre_incr($name.libbash_value); }
	|^(PRE_DECR ^(VAR_REF name)){ $value = walker->pre_decr($name.libbash_value); }
	|^(POST_INCR ^(VAR_REF name)){ $value = walker->post_incr($name.libbash_value); }
	|^(POST_DECR ^(VAR_REF name)){ $value = walker->post_decr($name.libbash_value); }
	|^(EQUALS name l=arithmetics) {
		$value = walker->set_value($name.libbash_value, l);
	}
	|^(MUL_ASSIGN name l=arithmetics) {
		$value = walker->assign(&interpreter::multiply, $name.libbash_value, l);
	}
	|^(DIVIDE_ASSIGN name l=arithmetics) {
		$value = walker->assign(&interpreter::divide, $name.libbash_value, l);
	}
	|^(MOD_ASSIGN name l=arithmetics) {
		$value = walker->assign(&interpreter::mod, $name.libbash_value, l);
	}
	|^(PLUS_ASSIGN name l=arithmetics) {
		$value = walker->assign(&interpreter::plus, $name.libbash_value, l);
	}
	|^(MINUS_ASSIGN name l=arithmetics) {
		$value = walker->assign(&interpreter::minus, $name.libbash_value, l);
	}
	|^(LSHIFT_ASSIGN name l=arithmetics) {
		$value = walker->assign(&interpreter::left_shift, $name.libbash_value, l);
	}
	|^(RSHIFT_ASSIGN name l=arithmetics) {
		$value = walker->assign(&interpreter::right_shift, $name.libbash_value, l);
	}
	|^(AND_ASSIGN name l=arithmetics) {
		$value = walker->assign(&interpreter::bitwiseand, $name.libbash_value, l);
	}
	|^(XOR_ASSIGN name l=arithmetics) {
		$value = walker->assign(&interpreter::bitwisexor, $name.libbash_value, l);
	}
	|^(OR_ASSIGN name l=arithmetics) {
		$value = walker->assign(&interpreter::bitwiseor, $name.libbash_value, l);
	}
	| NUMBER { $value = walker->parse_int($NUMBER);}
	| DIGIT { $value = walker->parse_int($DIGIT);}
	;
