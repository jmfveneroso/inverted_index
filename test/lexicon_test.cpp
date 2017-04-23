#include <iostream>
#include <cstring>
#include <cstdio>
#include <vector>
#include "gtest/gtest.h"

#include "lexicon.h"

using namespace std;
using namespace TP1;

// The fixture for testing class Lexicon. From google test primer.
class LexiconTest : public ::testing::Test {
  protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    LexiconTest() {}

    virtual ~LexiconTest() {
      // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:
    virtual void SetUp() {
    }

    virtual void TearDown() {
      // Code here will be called immediately after each test (right
      // before the destructor).
    }

    // Objects declared here can be used by all tests in the test case for Lexicon.
    Lexicon lexicon_;
};

// int addLexeme(const std::string& term);
TEST_F(LexiconTest, AddLexeme) {
  EXPECT_EQ(-1, lexicon_.GetLexemeId("pease"));
  EXPECT_EQ(-1, lexicon_.GetLexemeId("porridge"));
  EXPECT_EQ(-1, lexicon_.GetLexemeId("hot"));
  
  lexicon_.AddLexeme("pease");
  lexicon_.AddLexeme("porridge");
  lexicon_.AddLexeme("hot");
  lexicon_.AddLexeme("cold");
  lexicon_.AddLexeme("in");
  lexicon_.AddLexeme("the");
  lexicon_.AddLexeme("pot");
  lexicon_.AddLexeme("nine");
  lexicon_.AddLexeme("days");
  lexicon_.AddLexeme("old");
  
  EXPECT_EQ(-1, lexicon_.GetLexemeId("sadwadwa"));
  EXPECT_EQ(-1, lexicon_.GetLexemeId("dwdwfafaw"));
  EXPECT_EQ(1, lexicon_.GetLexemeId("pease"));
  EXPECT_EQ(2, lexicon_.GetLexemeId("porridge"));
  EXPECT_EQ(3, lexicon_.GetLexemeId("hot"));
  EXPECT_EQ(4, lexicon_.GetLexemeId("cold"));
  EXPECT_EQ(5, lexicon_.GetLexemeId("in"));
  EXPECT_EQ(6, lexicon_.GetLexemeId("the"));
  EXPECT_EQ(7, lexicon_.GetLexemeId("pot"));
  EXPECT_EQ(8, lexicon_.GetLexemeId("nine"));
  EXPECT_EQ(9, lexicon_.GetLexemeId("days"));
  EXPECT_EQ(10, lexicon_.GetLexemeId("old"));
}

TEST_F(LexiconTest, ExtractLexemes) {
  vector<string> words = Lexicon::ExtractLexemes("pease porridge hot, pease porridge cold");
  
  EXPECT_EQ("pease", words.at(0));
  EXPECT_EQ("porridge", words.at(1));
  EXPECT_EQ("hot", words.at(2));
  EXPECT_EQ("pease", words.at(3));
  EXPECT_EQ("porridge", words.at(4));
  EXPECT_EQ("cold", words.at(5));
}

TEST_F(LexiconTest, GetValidCharacter) {
  EXPECT_EQ('a', Lexicon::GetValidCharacter('a'));
  EXPECT_EQ('a', Lexicon::GetValidCharacter('A'));
  EXPECT_EQ('v', Lexicon::GetValidCharacter('V'));
  EXPECT_EQ('y', Lexicon::GetValidCharacter('Y'));
  EXPECT_EQ(NULL, Lexicon::GetValidCharacter('&'));
}
