#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>

#include "gtest/gtest.h"

#include "mocks/mock_lexicon.h"
#include "tuple_sorter.h"

using namespace std;
using namespace TP1;

// class TupleSorterTest : public ::testing::Test {
//   protected:
//     TupleSorterTest() : tuple_sorter_(&lexicon_) {}
// 
//     virtual ~TupleSorterTest() {}
// 
//     virtual void SetUp() {}
// 
//     virtual void TearDown() {
//                                 cout << "dont get" << endl;
//     }
// 
//     MockLexicon lexicon_;
//     TupleSorter tuple_sorter_;
// };
// 
// TEST_F(TupleSorterTest, ExtractTuplesFromDoc) {
//   const string html1 =
//     "<html><body>"
//     "<p>Pease porridge hot, pease porridge cold,</p>"
//     "<p>Pease porridge in the pot, nine days old;</p>"
//     "</body></html>";
//   
//   const string html2 =
//     "<html><body>"
//     "<p>Some like it hot, some like it cold,</p>"
//     "<p>Some like it in the pot, nine days old.</p>"
//     "</body></html>";
//   
//   const string html3 =
//     "<html><body>"
//     "<p>A pontuação #$%2 não? está! certa.</p>"
//     "</body></html>";
//   
//   tuple_sorter_.ExtractTuplesFromDoc(0, html1);
//   tuple_sorter_.ExtractTuplesFromDoc(1, html2);
//   tuple_sorter_.ExtractTuplesFromDoc(2, html3);
// 
//   tuple_sorter_.Sort();
//   
//   Tuple tuples[36];
//   tuples[0] = Tuple(1, 0, 1);
//   tuples[1] = Tuple(1, 0, 4);
//   tuples[2] = Tuple(1, 0, 7);
//   tuples[3] = Tuple(2, 0, 2);
//   tuples[4] = Tuple(2, 0, 5);
//   tuples[5] = Tuple(2, 0, 8);
//   tuples[6] = Tuple(3, 0, 3);
//   tuples[7] = Tuple(3, 1, 4);
//   tuples[8] = Tuple(4, 0, 6);
//   tuples[9] = Tuple(4, 1, 8);
//   tuples[10] = Tuple(5, 0, 9);
//   tuples[11] = Tuple(5, 1, 12);
//   tuples[12] = Tuple(6, 0, 10);
//   tuples[13] = Tuple(6, 1, 13);
//   tuples[14] = Tuple(7, 0, 11);
//   tuples[15] = Tuple(7, 1, 14);
//   tuples[16] = Tuple(8, 0, 12);
//   tuples[17] = Tuple(8, 1, 15);
//   tuples[18] = Tuple(9, 0, 13);
//   tuples[19] = Tuple(9, 1, 16);
//   tuples[20] = Tuple(10, 0, 14);
//   tuples[21] = Tuple(10, 1, 17);
//   tuples[22] = Tuple(11, 1, 1);
//   tuples[23] = Tuple(11, 1, 5);
//   tuples[24] = Tuple(11, 1, 9);
//   tuples[25] = Tuple(12, 1, 2);
//   tuples[26] = Tuple(12, 1, 6);
//   tuples[27] = Tuple(12, 1, 10);
//   tuples[28] = Tuple(13, 1, 3);
//   tuples[29] = Tuple(13, 1, 7);
//   tuples[30] = Tuple(13, 1, 11);
//   tuples[31] = Tuple(14, 2, 1);
//   tuples[32] = Tuple(15, 2, 2);
//   tuples[33] = Tuple(16, 2, 3);
//   tuples[34] = Tuple(17, 2, 4);
//   tuples[35] = Tuple(18, 2, 5);
// 
//   int num_tuples = tuple_sorter_.get_num_tuples();
//   EXPECT_EQ(36, num_tuples);
//   
//   for (int i = 0; i < 36; i++) {
//     Tuple tuple = tuple_sorter_.PopTuple();
//     
//     EXPECT_EQ(tuples[i].lexeme_id, tuple.lexeme_id);
//     EXPECT_EQ(tuples[i].document_id, tuple.document_id);
//     EXPECT_EQ(tuples[i].word_position, tuple.word_position);
//   }
// }
// 
// // TEST_F(TupleSorterTest, ExtractTuplesFromAnchorText) {
// //   const string html1 =
// //     "<html><body>"
// //       "<div>Esse texto não deve ser lido</div>"
// //       "<a href=\"www.link1.com\">Pease porridge hot, pease porridge cold,</a>"
// //       "<a href=\"www.link2.com.br\">Pease porridge in the pot, nine days old;</a>"
// //     "</body></html>";
// //   
// //   const string html2 =
// //     "<html><body>"
// //       "<a href=\"www.link3.com\">Some like it hot, some like it cold,</a>"
// //       "<a href=\"www.link4.com.br\">Some like it in the pot, nine days old.</a>"
// //       "<div>Nem esse</div>"
// //     "</body></html>";
// //   
// //   const string html3 =
// //     "<html><body>"
// //       "<a href=\"www.link5.com\">A pontuação #$%2 não? está! certa.</a>"
// //     "</body></html>";
// //   
// //   map<string, int> doc_map;
// //   
// //   doc_map.insert(pair<string, int>("www.link1.com", 0));
// //   doc_map.insert(pair<string, int>("www.link2.com.br", 1));
// //   doc_map.insert(pair<string, int>("www.link3.com", 2));
// //   doc_map.insert(pair<string, int>("www.link4.com.br", 3));
// //   doc_map.insert(pair<string, int>("www.link5.com", 4));
// //   
// //   tuple_sorter_.set_doc_map(doc_map);
// //   
// //   tuple_sorter_.ExtractTuplesFromAnchorText(html1);
// //   tuple_sorter_.ExtractTuplesFromAnchorText(html2);
// //   tuple_sorter_.ExtractTuplesFromAnchorText(html3);
// //   
// //   tuple_sorter_.Sort();
// //   
// //   Tuple tuples[36];
// //   
// //   tuples[0] = Tuple(1, 0, 0);
// //   tuples[1] = Tuple(1, 0, 0);
// //   tuples[2] = Tuple(1, 1, 0);
// //   tuples[3] = Tuple(2, 0, 0);
// //   tuples[4] = Tuple(2, 0, 0);
// //   tuples[5] = Tuple(2, 1, 0);
// //   tuples[6] = Tuple(3, 0, 0);
// //   tuples[7] = Tuple(3, 2, 0);
// //   tuples[8] = Tuple(4, 0, 0);
// //   tuples[9] = Tuple(4, 2, 0);
// //   tuples[10] = Tuple(5, 1, 0);
// //   tuples[11] = Tuple(5, 3, 0);
// //   tuples[12] = Tuple(6, 1, 0);
// //   tuples[13] = Tuple(6, 3, 0);
// //   tuples[14] = Tuple(7, 1, 0);
// //   tuples[15] = Tuple(7, 3, 0);
// //   tuples[16] = Tuple(8, 1, 0);
// //   tuples[17] = Tuple(8, 3, 0);
// //   tuples[18] = Tuple(9, 1, 0);
// //   tuples[19] = Tuple(9, 3, 0);
// //   tuples[20] = Tuple(10, 1, 0);
// //   tuples[21] = Tuple(10, 3, 0);
// //   tuples[22] = Tuple(11, 2, 0);
// //   tuples[23] = Tuple(11, 2, 0);
// //   tuples[24] = Tuple(11, 3, 0);
// //   tuples[25] = Tuple(12, 2, 0);
// //   tuples[26] = Tuple(12, 2, 0);
// //   tuples[27] = Tuple(12, 3, 0);
// //   tuples[28] = Tuple(13, 2, 0);
// //   tuples[29] = Tuple(13, 2, 0);
// //   tuples[30] = Tuple(13, 3, 0);
// //   tuples[31] = Tuple(14, 4, 0);
// //   tuples[32] = Tuple(15, 4, 0);
// //   tuples[33] = Tuple(16, 4, 0);
// //   tuples[34] = Tuple(17, 4, 0);
// //   tuples[35] = Tuple(18, 4, 0);
// //   
// //   int num_tuples = tuple_sorter_.get_num_tuples();
// //   ASSERT_EQ(36, num_tuples);
// //   
// //   for (int i = 0; i < 36; i++) {
// //     Tuple tuple = tuple_sorter_.PopTuple();
// //     
// //     EXPECT_EQ(tuples[i].lexeme_id, tuple.lexeme_id);
// //     EXPECT_EQ(tuples[i].document_id, tuple.document_id);
// //     EXPECT_EQ(tuples[i].word_position, tuple.word_position);
// //   }
// // }
