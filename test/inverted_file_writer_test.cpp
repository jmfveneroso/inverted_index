#include <iostream>
#include <cstdio>

#include "gtest/gtest.h"

#include "mocks/mock_lexicon.h"
#include "mocks/mock_tuple_sorter.h"
#include "inverted_file_writer.h"

using namespace std;
using namespace TP1;

// class InvertedFileWriterTest : public ::testing::Test {
//   protected:
//     InvertedFileWriterTest()
//       : tuple_sorter_(&lexicon_),
//         inverted_file_writer_(&lexicon_, &tuple_sorter_, "inverted_file_test.dat") {}
// 
//     virtual ~InvertedFileWriterTest() {}
// 
//     virtual void SetUp() {}
// 
//     virtual void TearDown() {}
// 
//     MockLexicon lexicon_;
//     MockTupleSorter tuple_sorter_;
//   
//     InvertedFileWriter inverted_file_writer_;
// };
// 
// TEST_F(InvertedFileWriterTest, WriteFromCollection) {
//   RICPNS::MockCollectionReader collection_reader("", "");
// 
//   inverted_file_writer_.WriteFromCollection(&collection_reader);
//   
//   FILE *file = fopen("inverted_file_test.dat", "rb");
//   
//   // Read header.
//   InvertedFileHeader inverted_file_header;
//   fread(&inverted_file_header, sizeof(InvertedFileHeader), 1, file);
//   
//   EXPECT_EQ(1, inverted_file_header.num_documents);
//   EXPECT_EQ(18, inverted_file_header.num_lexemes);
//   EXPECT_EQ(36, inverted_file_header.num_tuples);
//   
//   // Read lexemes.
//   string correct_lexemes[] = {"pease", "porridge", "hot", "cold", "in", "the", "pot", "nine",
//     "days", "old", "some", "like", "it", "a", "pontuacao", "nao", "esta", "certa"};
//   
//   for (int i = 0; i < 18; i++) {
//     unsigned short size;
//     char buffer[MAX_LEXEME_LENGTH];
//     
//     fread(&size, sizeof(unsigned short), 1, file);
//     EXPECT_EQ(correct_lexemes[i].size() + 1, size);
//     fread(buffer, sizeof(char), correct_lexemes[i].size() + 1, file);
//     EXPECT_EQ(correct_lexemes[i], buffer);
//   }
//   
//   // Read offsets.
//   int correct_offsets[] = {0, 3, 6, 8, 10, 12, 14, 16, 18, 20, 22, 25, 28, 31, 32, 33, 34, 35};
//   for (int i = 0; i < 18; i++) {
//     int offset;
//     fread(&offset, sizeof(int), 1, file);
//     EXPECT_EQ(correct_offsets[i], offset);
//   }
//   
//   // Read tuples.
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
//   for (int i = 0; i < 36; i++) {
//     int doc_id;
//     int word_position;
//     
//     fread(&doc_id, sizeof(int), 1, file);
//     fread(&word_position, sizeof(int), 1, file);
//     
//     EXPECT_EQ(tuples[i].document_id, doc_id);
//     EXPECT_EQ(tuples[i].word_position, word_position);
//   }
//   
//   fclose(file);
// }
