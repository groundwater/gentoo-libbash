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
///
/// \file lib_test.cpp
/// \author Mu Qiao
/// \brief series of unit tests for generated walker.
///

#include <antlr3defs.h>

#include <gtest/gtest.h>

#include "bashastLexer.h"
#include "bashastParser.h"
#include "bashwalker.h"
#include "core/interpreter.h"

using namespace std;

class walker_test: public ::testing::Test
{
  pANTLR3_INPUT_STREAM input;
  pbashastLexer lxr;
  pANTLR3_COMMON_TOKEN_STREAM tstream;
  pbashastParser psr;
  bashastParser_arithmetics_return langAST;
  pANTLR3_COMMON_TREE_NODE_STREAM nodes;
protected:
  virtual void SetUp()
  {
    walker = shared_ptr<interpreter>(new interpreter);
  }
  virtual void TearDown()
  {
    nodes->free(nodes);
    treePsr->free(treePsr);
    psr->free(psr);
    tstream->free(tstream);
    lxr->free(lxr);
    input->close(input);
  }
  void init_walker(const char* script);
public:
  pbashwalker treePsr;
  shared_ptr<interpreter> walker;

  int run_arithmetic(const char* script)
  {
    init_walker(script);
    return treePsr->arithmetics(treePsr);
  }
};


void walker_test::init_walker(const char *script){

  auto start = reinterpret_cast<pANTLR3_UINT8>(const_cast<char *>(script));
  input  = antlr3NewAsciiStringInPlaceStream(start,
                                             strlen(script),
                                             NULL);
  if ( input == NULL )
  {
    ANTLR3_FPRINTF(stderr,
                   "Unable to create input stream for script: %s\n",
                   script);
    FAIL();
  }

  lxr = bashastLexerNew(input);
  if ( lxr == NULL )
  {
    ANTLR3_FPRINTF(stderr,
                   "Unable to create the lexer due to malloc() failure\n");
    FAIL();
  }

  tstream = antlr3CommonTokenStreamSourceNew(
      ANTLR3_SIZE_HINT, lxr->pLexer->rec->state->tokSource);
  if (tstream == NULL)
  {
    ANTLR3_FPRINTF(stderr,
                   "Out of memory trying to allocate token stream\n");
    FAIL();
  }

  psr = bashastParserNew(tstream);
  if (psr == NULL)
  {
    ANTLR3_FPRINTF(stderr, "Out of memory trying to allocate parser\n");
    FAIL();
  }

  langAST = psr->arithmetics(psr);
  nodes   = antlr3CommonTreeNodeStreamNewTree(langAST.tree,
                                              ANTLR3_SIZE_HINT);
  treePsr = bashwalkerNew(nodes);
  walker->define("value", 100);
  set_interpreter(walker);
}

#define TEST_BINARY_ARITHMETIC(name, script, exp_value)\
  TEST_F(walker_test, name)\
  {EXPECT_EQ(exp_value, run_arithmetic(script));}

TEST_BINARY_ARITHMETIC(logicor_true,        "0 || -2",      1)
TEST_BINARY_ARITHMETIC(logicor_false,       "0 || 0",       0)
TEST_BINARY_ARITHMETIC(logicand_true,       "-1 && 10",     1)
TEST_BINARY_ARITHMETIC(logicand_false,      "-1 && 0",      0)
TEST_BINARY_ARITHMETIC(bitwiseor,           "1 | 2",        3)
TEST_BINARY_ARITHMETIC(bitwiseand,          "4 & 2",        0)
TEST_BINARY_ARITHMETIC(bitwisexor,          "5 ^ 10",       15)
TEST_BINARY_ARITHMETIC(less_equal_true,     "5 <= 5",       1)
TEST_BINARY_ARITHMETIC(less_equal_false,    "5 <= -10",     0)
TEST_BINARY_ARITHMETIC(greater_equal_true,  "5 >= 5",       1)
TEST_BINARY_ARITHMETIC(greater_equal_false, "-5 >= 5",      0)
TEST_BINARY_ARITHMETIC(less_true,           "5 < 6",        1)
TEST_BINARY_ARITHMETIC(less_false,          "-5 < -5",      0)
TEST_BINARY_ARITHMETIC(greater_true,        "5 > 4",        1)
TEST_BINARY_ARITHMETIC(greater_false,       "-5 > -5",      0)
TEST_BINARY_ARITHMETIC(left_shift,          "-5 << 2",      -20)
TEST_BINARY_ARITHMETIC(right_shift,         "-5 >> 2",      -2)
TEST_BINARY_ARITHMETIC(add,                 "1 + 1",        2)
TEST_BINARY_ARITHMETIC(minus,               "10 - 5",       5)
TEST_BINARY_ARITHMETIC(mul,                 "10 * 5",       50)
TEST_BINARY_ARITHMETIC(div,                 "10 / 4",       2)
TEST_BINARY_ARITHMETIC(mod,                 "10 % 4",       2)
TEST_BINARY_ARITHMETIC(exp,                 "10 ** 4",      10000)
TEST_BINARY_ARITHMETIC(negation,            "!10",          0)
TEST_BINARY_ARITHMETIC(complement,          "~   10",       -11)
TEST_BINARY_ARITHMETIC(arithmetic_cnd,      "1?10:5",       10)
TEST_BINARY_ARITHMETIC(arithmetic_cnd2,     "0?10:5",       5)
TEST_BINARY_ARITHMETIC(var_ref,             "$value",       100)
TEST_BINARY_ARITHMETIC(pre_incr,            "++value",       101)
TEST_BINARY_ARITHMETIC(pre_decr,            "--value",       99)
TEST_BINARY_ARITHMETIC(post_incr,           "value++",       100)
TEST_BINARY_ARITHMETIC(post_decr,           "value--",       100)
TEST_BINARY_ARITHMETIC(complex_incr_decr,   "value+++value++", 201)
TEST_BINARY_ARITHMETIC(complex_incr_decr2,  "++value+value++", 202)
TEST_BINARY_ARITHMETIC(complex_cal,         "10*(2+5)<<3%2**5", 560)
TEST_BINARY_ARITHMETIC(complex_cal2,        "10*${value}<<3%2**5", 8000)
TEST_BINARY_ARITHMETIC(complex_cal3,        "(20&5|3||1*100-20&5*10)+~(2*5)", -10)
