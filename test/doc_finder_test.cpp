#include <iostream>
#include <string>
#include <cstdio>
#include <vector>

#include "gtest/gtest.h"

#include "mocks/mock_inverted_file_reader.h"
#include "doc_finder.h"

using namespace std;
using namespace TP1;


// class DocFinderTest : public ::testing::Test {
//   protected:
//     DocFinderTest() : doc_finder_(&inverted_file_reader_) {}
// 
//     virtual ~DocFinderTest() {}
// 
//     virtual void SetUp() {}
// 
//     virtual void TearDown() {}
// 
//     MockInvertedFileReader inverted_file_reader_;
//     DocFinder doc_finder_;
// };
// 
// TEST_F(DocFinderTest, OperatorAnd) {
//   const int arr1[] = {1, 2, 3, 4, 8};
//   vector<int> v1 (arr1, arr1 + sizeof(arr1) / sizeof(int) );
// 
//   const int arr2[] = {1, 4, 5, 6, 7, 8};
//   vector<int> v2 (arr2, arr2 + sizeof(arr2) / sizeof(int) );
// 
//   const int arr3[] = {1, 4, 8};
//   vector<int> correct_result(arr3, arr3 + sizeof(arr3) / sizeof(int) );
// 
//   vector<int> result = DocFinder::OperatorAnd(v1, v2);
// 
//   EXPECT_EQ(correct_result, result);
// }
// 
// TEST_F(DocFinderTest, OperatorOr) {
//   const int arr1[] = {1, 2, 3, 4, 8};
//   vector<int> v1 (arr1, arr1 + sizeof(arr1) / sizeof(int) );
// 
//   const int arr2[] = {1, 4, 5, 6, 7, 8};
//   vector<int> v2 (arr2, arr2 + sizeof(arr2) / sizeof(int) );
// 
//   const int arr3[] = {1, 2, 3, 4, 5, 6, 7, 8};
//   vector<int> correct_result(arr3, arr3 + sizeof(arr3) / sizeof(int) );
// 
//   vector<int> result = DocFinder::OperatorOr(v1, v2);
// 
//   EXPECT_EQ(correct_result, result);
// }
// 
// TEST_F(DocFinderTest, Query) {
//   // Query 1
//   string query = "cold";
//   vector<int> result = doc_finder_.BooleanQuery(query);
// 
//   EXPECT_EQ(2, result.size());
//   EXPECT_EQ(0, result.at(0));
//   EXPECT_EQ(1, result.at(1));
// 
//   // Query 2
//   query = "porridge and old";
//   result = doc_finder_.BooleanQuery(query);
// 
//   EXPECT_EQ(1, result.size());
//   EXPECT_EQ(0, result.at(0));
// 
//   // Query 3
//   query = "some or porridge";
//   result = doc_finder_.BooleanQuery(query);
// 
//   EXPECT_EQ(2, result.size());
//   EXPECT_EQ(0, result.at(0));
//   EXPECT_EQ(1, result.at(1));
// 
//   // Query 4
//   query = "some AND porridge";
//   result = doc_finder_.BooleanQuery(query);
// 
//   EXPECT_EQ(0, result.size());
// 
//   // Query 5
//   query = "x OR nine";
//   result = doc_finder_.BooleanQuery(query);
// 
//   EXPECT_EQ(2, result.size());
//   EXPECT_EQ(0, result.at(0));
//   EXPECT_EQ(1, result.at(1));
// 
//   // Query 5
//   query = "pease AND porridge AND hot";
//   result = doc_finder_.BooleanQuery(query);
// 
//   EXPECT_EQ(1, result.size());
//   EXPECT_EQ(0, result.at(0));
// 
//   // Query 6
//   query = "xz";
//   result = doc_finder_.BooleanQuery(query);
//   EXPECT_EQ(0, result.size());
// }
