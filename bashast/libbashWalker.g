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
	^(EQUALS libbash_name=name libbash_value=string_expr){
		walker->define(libbash_name, libbash_value);
	};

string_expr	returns[std::string libbash_value]:
	^(STRING libbash_string=string_expr) { $libbash_value = libbash_string; }
	|^(DOUBLE_QUOTED_STRING (libbash_string=dqstr { $libbash_value += libbash_string; })*);

//A rule for filenames/strings
res_word_str returns[std::string libbash_value]
@after {
	$libbash_value = walker->get_string($res_word_str.start);
}:
	CASE|DO|DONE|ELIF|ELSE|ESAC|FI|FOR|FUNCTION|IF|IN|SELECT|THEN|UNTIL|WHILE|TIME;

//Parts of strings, no slashes, no reserved words
ns_str_part_no_res returns[std::string libbash_value]
options{ backtrack=true; }
@after {
	$libbash_value = walker->get_string($ns_str_part_no_res.start);
}:
	num|name|EQUALS|PCT|PCTPCT|MINUS|DOT|DOTDOT|COLON|BOP|UOP|TEST_EXPR|'_'|TILDE|INC|DEC|MUL_ASSIGN|DIVIDE_ASSIGN|MOD_ASSIGN|PLUS_ASSIGN|MINUS_ASSIGN|LSHIFT_ASSIGN|RSHIFT_ASSIGN|AND_ASSIGN|XOR_ASSIGN|OR_ASSIGN|ESC_CHAR|CARET|OTHER;

//Parts of strings, no slashes
ns_str_part returns[std::string libbash_value]:
	ns_str_part_no_res { $libbash_value = $ns_str_part_no_res.libbash_value; }
	|res_word_str {$libbash_value = $res_word_str.libbash_value; };

//Any allowable part of a string, including slashes, no pounds
str_part returns[std::string libbash_value]:
	libbash_string=ns_str_part { $libbash_value = libbash_string; }
	|SLASH { $libbash_value = "/"; };

//Any allowable part of a string, with pounds
str_part_with_pound returns[std::string libbash_value]
@after {
	$libbash_value = walker->get_string($str_part_with_pound.start);
}:
	str_part|POUND|POUNDPOUND;

//Allowable parts of double quoted strings
dq_str_part returns[std::string libbash_value]
@after {
	$libbash_value = walker->get_string($dq_str_part.start);
}:
	BLANK|EOL|AMP|LOGICAND|LOGICOR|LESS_THAN|GREATER_THAN|PIPE|SQUOTE|SEMIC|COMMA|LPAREN|RPAREN|LLPAREN|RRPAREN|DOUBLE_SEMIC|LBRACE|RBRACE|TICK|LEQ|GEQ|str_part_with_pound;

//double quoted string rule, allows expansions
dqstr returns[std::string libbash_value]:
	dq_str_part { $libbash_value = $dq_str_part.libbash_value; }
	| libbash_string=var_ref { $libbash_value = libbash_string; };

//variable reference
var_ref returns[std::string libbash_value]:
	^(VAR_REF libbash_name=name) { $libbash_value=walker->resolve<std::string>(libbash_name); };

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
