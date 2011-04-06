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
static std::shared_ptr<interpreter> walker;
void set_interpreter(std::shared_ptr<interpreter> w)
{
	walker = w;
}
}

start: list|EOF;

list: ^(LIST var_def+);

name	returns[std::string libbash_value]:
	NAME {$libbash_value = walker->get_string($NAME);}
	|	LETTER {$libbash_value = walker->get_string($LETTER);}
	|	'_' {$libbash_value="_";};

num returns[std::string libbash_value]
options{ k=1; }:
	DIGIT { $libbash_value = walker->get_string($DIGIT); }
	|NUMBER { $libbash_value = walker->get_string($NUMBER); };

var_def:
	^(EQUALS libbash_name=name libbash_value=word){
		walker->define(libbash_name, libbash_value);
	};

string_expr	returns[std::string libbash_value]:
	^(STRING(
		(DOUBLE_QUOTED_STRING) => ^(DOUBLE_QUOTED_STRING (libbash_string=double_quoted_string { $libbash_value += libbash_string; })*)
		|(ARITHMETIC_EXPRESSION) => ^(ARITHMETIC_EXPRESSION value=arithmetics { $libbash_value = boost::lexical_cast<std::string>(value); })
		|libbash_string=any_string { $libbash_value = libbash_string; }
	));

//double quoted string rule, allows expansions
double_quoted_string returns[std::string libbash_value]:
	(var_ref) => libbash_string=var_ref { $libbash_value = libbash_string; }
	|(ARITHMETIC_EXPRESSION) => ^(ARITHMETIC_EXPRESSION value=arithmetics) { $libbash_value = boost::lexical_cast<std::string>(value); }
	|libbash_string=any_string { $libbash_value = libbash_string; };

any_string returns[std::string libbash_value]
@declarations {
	pANTLR3_BASE_TREE any_token;
}:
	any_token=. { $libbash_value = walker->get_string(any_token); };

//Allowable variable names in the variable expansion
var_name returns[std::string libbash_value]
@after {
	$libbash_value = walker->get_string($var_name.start);
}:
	num|name;

var_expansion returns[std::string libbash_value]:
	^(USE_DEFAULT var_name libbash_word=word) {
		libbash_value = walker->do_default_expansion($var_name.libbash_value, libbash_word);
	}
	|^(ASSIGN_DEFAULT var_name libbash_word=word) {
		libbash_value = walker->do_assign_expansion($var_name.libbash_value, libbash_word);
	}
	|^(USE_ALTERNATE var_name libbash_word=word) {
		libbash_value = walker->do_alternate_expansion($var_name.libbash_value, libbash_word);
	}
	|(^(OFFSET var_name arithmetics arithmetics)) => ^(OFFSET var_name offset=arithmetics length=arithmetics) {
		libbash_value = walker->do_substring_expansion($var_name.libbash_value, offset, length);
	}
	|^(OFFSET var_name offset=arithmetics) {
		libbash_value = walker->do_substring_expansion($var_name.libbash_value, offset);
	}
	|^(POUND var_name) {
		libbash_value = boost::lexical_cast<std::string>(walker->get_length($var_name.libbash_value));
	};

word returns[std::string libbash_value]:
	// Avoid conflict with arithmetics (both have VAR_DEF)
	(var_ref) => libbash_string=var_ref { $libbash_value = libbash_string; }
	// Avoid conflict with arithmetics (both have num)
	|(num) => libbash_string=num { $libbash_value = libbash_string; }
	|libbash_string=string_expr { $libbash_value = libbash_string; }
	|value=arithmetics { $libbash_value = boost::lexical_cast<std::string>(value); };

//variable reference
var_ref returns[std::string libbash_value]:
	^(VAR_REF libbash_name=name) { $libbash_value = walker->resolve<std::string>(libbash_name); }
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
	|^(VAR_REF libbash_name=name) {
		$value = walker->resolve<int>(libbash_name);
	}
	|^(PRE_INCR libbash_name=name){ $value = walker->pre_incr(libbash_name); }
	|^(PRE_DECR libbash_name=name){ $value = walker->pre_decr(libbash_name); }
	|^(POST_INCR libbash_name=name){ $value = walker->post_incr(libbash_name); }
	|^(POST_DECR libbash_name=name){ $value = walker->post_decr(libbash_name); }
	|^(EQUALS libbash_name=name l=arithmetics) {
		$value = walker->set_value(libbash_name, l);
	}
	|^(MUL_ASSIGN libbash_name=name l=arithmetics) {
		$value = walker->assign(&interpreter::multiply, libbash_name, l);
	}
	|^(DIVIDE_ASSIGN libbash_name=name l=arithmetics) {
		$value = walker->assign(&interpreter::divide, libbash_name, l);
	}
	|^(MOD_ASSIGN libbash_name=name l=arithmetics) {
		$value = walker->assign(&interpreter::mod, libbash_name, l);
	}
	|^(PLUS_ASSIGN libbash_name=name l=arithmetics) {
		$value = walker->assign(&interpreter::plus, libbash_name, l);
	}
	|^(MINUS_ASSIGN libbash_name=name l=arithmetics) {
		$value = walker->assign(&interpreter::minus, libbash_name, l);
	}
	|^(LSHIFT_ASSIGN libbash_name=name l=arithmetics) {
		$value = walker->assign(&interpreter::left_shift, libbash_name, l);
	}
	|^(RSHIFT_ASSIGN libbash_name=name l=arithmetics) {
		$value = walker->assign(&interpreter::right_shift, libbash_name, l);
	}
	|^(AND_ASSIGN libbash_name=name l=arithmetics) {
		$value = walker->assign(&interpreter::bitwiseand, libbash_name, l);
	}
	|^(XOR_ASSIGN libbash_name=name l=arithmetics) {
		$value = walker->assign(&interpreter::bitwisexor, libbash_name, l);
	}
	|^(OR_ASSIGN libbash_name=name l=arithmetics) {
		$value = walker->assign(&interpreter::bitwiseor, libbash_name, l);
	}
	| NUMBER { $value = walker->parse_int($NUMBER);}
	| DIGIT { $value = walker->parse_int($DIGIT);}
	;
