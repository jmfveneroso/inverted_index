#include <iostream>
#include <string>
#include <cstdio>
#include <vector>

#include "gtest/gtest.h"

#include "mocks/mock_lexicon.h"
#include "inverted_file_reader.h"

using namespace std;
using namespace TP1;

// class InvertedFileReaderTest : public ::testing::Test {
// protected:
//   InvertedFileReaderTest() : inverted_file_reader_(&lexicon_, "../../data/inverted_file_test.dat") {}
//   
//   virtual ~InvertedFileReaderTest() {}
//   
//   virtual void SetUp() {}
//   
//   virtual void TearDown() {}
//   
//   MockLexicon lexicon_;
//   InvertedFileReader inverted_file_reader_;
// };
// 
// TEST_F(InvertedFileReaderTest, GetTuples) {
//   vector<Tuple> result = inverted_file_reader_.GetTuples("cold");
//   
//   ASSERT_EQ(2, result.size());
//   EXPECT_EQ(Tuple(4, 0, 6), result.at(0));
//   EXPECT_EQ(Tuple(4, 1, 8), result.at(1));
//   
//   result.clear();
//   result = inverted_file_reader_.GetTuples("porridge");
//   
//   ASSERT_EQ(3, result.size());
//   EXPECT_EQ(Tuple(2, 0, 2), result.at(0));
//   EXPECT_EQ(Tuple(2, 0, 5), result.at(1));
//   EXPECT_EQ(Tuple(2, 0, 8), result.at(2));
// 
//   result.clear();
//   result = inverted_file_reader_.GetTuples("old");
//   
//   ASSERT_EQ(2, result.size());
//   EXPECT_EQ(Tuple(10, 0, 14), result.at(0));
//   EXPECT_EQ(Tuple(10, 1, 17), result.at(1));
// 
//   result.clear();
//   result = inverted_file_reader_.GetTuples("some");
// 
//   ASSERT_EQ(3, result.size());
//   EXPECT_EQ(Tuple(11, 1, 1), result.at(0));
//   EXPECT_EQ(Tuple(11, 1, 5), result.at(1));
//   EXPECT_EQ(Tuple(11, 1, 9), result.at(2));
//   
//   result = inverted_file_reader_.GetTuples("x");
//   ASSERT_EQ(0, result.size());
// }
