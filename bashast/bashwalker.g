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
	| NUMBER { $value = walker->parse_int($NUMBER);}
	| DIGIT { $value = walker->parse_int($DIGIT);}
	;
