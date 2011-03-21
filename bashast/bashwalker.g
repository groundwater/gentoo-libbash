/*
   Copyright 2011 Mu Qiao

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
tree grammar bashwalker;

options
{
	language = C;
	tokenVocab = bashast;
	ASTLabelType = pANTLR3_BASE_TREE;
}

@includes{
#include <memory>

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

name	returns[std::string libbash_value]:
	NAME {$libbash_value = walker->get_string($NAME);}
	|	LETTER {$libbash_value = walker->get_string($LETTER);}
	|	'_' {$libbash_value="_";};

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
	| NUMBER { $value = walker->parse_int($NUMBER);}
	| DIGIT { $value = walker->parse_int($DIGIT);}
	;
