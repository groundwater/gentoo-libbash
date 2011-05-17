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
/// \file bash_condition_test.cpp
/// \author Mu Qiao
/// \brief series of unit tests for interpreter.
///
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>

#include <boost/lexical_cast.hpp>
#include <gtest/gtest.h>

#include "core/bash_condition.h"
#include "core/interpreter_exception.h"

namespace
{
  void set_time(const std::string& target, int ac, int mod)
  {
    struct utimbuf timebuf;
    timebuf.actime = ac;
    timebuf.modtime = mod;
    EXPECT_EQ(0, utime(target.c_str(), &timebuf));
  }

  class file_test: public testing::Test
  {
    protected:
      const std::string positive;
      const std::string negative;
      const std::string test_link;
      const std::string test_fifo;

      file_test(): positive("scripts/test.positive"),
                   negative("scripts/test.negative"),
                   test_link("scripts/test.link"),
                   test_fifo("scripts/test.fifo") {}

      virtual void SetUp()
      {
        EXPECT_NE(-1, creat(positive.c_str(),
                            S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO)
                 ) << "Can't create " << positive << " for test";
        set_time(positive, 0, 0);
        EXPECT_NE(-1, creat(negative.c_str(), 0)) << "Can't create " << negative << " for test";
        set_time(negative, 1, 0);
        EXPECT_EQ(0, symlink(positive.c_str(), test_link.c_str()));
        EXPECT_EQ(0, mkfifo(test_fifo.c_str(), 0));
      }

      virtual void TearDown()
      {
        EXPECT_EQ(0, unlink(test_fifo.c_str()));
        EXPECT_EQ(0, unlink(positive.c_str()));
        EXPECT_EQ(0, unlink(negative.c_str()));
        EXPECT_EQ(0, unlink(test_link.c_str()));
      }
  };
}

TEST_F(file_test, file_flags_positive)
{
  EXPECT_TRUE(internal::test_unary('a', "/"));
  EXPECT_TRUE(internal::test_unary('b', "/dev/root")) << "You should have /dev/root, or unit test won't pass";
  EXPECT_TRUE(internal::test_unary('c', "/dev/random"));
  EXPECT_TRUE(internal::test_unary('d', "/"));
  EXPECT_TRUE(internal::test_unary('e', "/"));
  EXPECT_TRUE(internal::test_unary('f', positive));
  EXPECT_TRUE(internal::test_unary('g', positive));
  EXPECT_TRUE(internal::test_unary('h', test_link));
  EXPECT_TRUE(internal::test_unary('k', positive));
  EXPECT_TRUE(internal::test_unary('p', test_fifo));
  EXPECT_TRUE(internal::test_unary('r', positive));
  EXPECT_TRUE(internal::test_unary('s', "/etc/fstab"));
  EXPECT_TRUE(internal::test_unary('u', positive));
  EXPECT_TRUE(internal::test_unary('w', positive));
  EXPECT_TRUE(internal::test_unary('x', positive));
  EXPECT_TRUE(internal::test_unary('L', test_link));
  EXPECT_TRUE(internal::test_unary('O', positive));
  EXPECT_TRUE(internal::test_unary('G', positive));
  EXPECT_TRUE(internal::test_unary('S', "/dev/log")) << "You should have /dev/log, or unit test won't pass";
  EXPECT_TRUE(internal::test_unary('N', positive));
}

TEST_F(file_test, file_flags_negative)
{
  EXPECT_FALSE(internal::test_unary('a', "not_exist"));
  EXPECT_FALSE(internal::test_unary('b', negative));
  EXPECT_FALSE(internal::test_unary('c', negative));
  EXPECT_FALSE(internal::test_unary('d', negative));
  EXPECT_FALSE(internal::test_unary('e', "not_exist"));
  EXPECT_FALSE(internal::test_unary('f', "/"));
  EXPECT_FALSE(internal::test_unary('g', negative));
  EXPECT_FALSE(internal::test_unary('h', negative));
  EXPECT_FALSE(internal::test_unary('k', negative));
  EXPECT_FALSE(internal::test_unary('p', negative));
  EXPECT_FALSE(internal::test_unary('r', negative));
  EXPECT_FALSE(internal::test_unary('s', negative));
  EXPECT_FALSE(internal::test_unary('t', "/dev/stdin"));
  EXPECT_FALSE(internal::test_unary('u', negative));
  EXPECT_FALSE(internal::test_unary('w', negative));
  EXPECT_FALSE(internal::test_unary('x', negative));
  EXPECT_FALSE(internal::test_unary('L', negative));
  EXPECT_FALSE(internal::test_unary('O', "/etc/fstab"));
  EXPECT_FALSE(internal::test_unary('G', "/etc/fstab"));
  EXPECT_FALSE(internal::test_unary('S', negative));
  EXPECT_FALSE(internal::test_unary('N', negative));
}

TEST(bash_condition, string_unary_operator)
{
  EXPECT_TRUE(internal::test_unary('z', ""));
  EXPECT_FALSE(internal::test_unary('z', "hello"));

  EXPECT_FALSE(internal::test_unary('n', ""));
  EXPECT_TRUE(internal::test_unary('n', "hello"));

  EXPECT_THROW(internal::test_unary('o', "extglob"), interpreter_exception);
}
