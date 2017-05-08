#include "inverted_file_writer.h"

using namespace std;
using namespace TP1;

InvertedFileWriter::InvertedFileWriter (
  std::shared_ptr<ILexicon> lexicon,
  std::shared_ptr<ITupleSorter> tuple_sorter
) : lexicon_(lexicon),
    tuple_sorter_(tuple_sorter) {
}

/**
 * Write header in the format:
 * --------------------
 * Header
 * --------------------
 * num_lexemes
 * num_tuples
 */
void InvertedFileWriter::WriteHeader () {
  // InvertedFileHeader header(
  //   num_documents_, 
  //   lexicon_->GetNumLexemes(), 
  //   tuple_sorter_->get_num_tuples()
  // );
  // fwrite(&header, sizeof(InvertedFileHeader), 1, output_file_);
}

/**
 * Write all lexemes to file.
 */
void InvertedFileWriter::WriteLexicon() {
  // char buffer[MAX_LEXEME_LENGTH];
  // std::map<unsigned int, string> id_map = lexicon_->get_id_map();
  // std::map<unsigned int, string>::iterator it = id_map.begin();
  //   
  // std::cout << "Writing lexicon..." << std::endl;
  // startbar();
  // for (int counter = 0; it != id_map.end(); ++it) {
  //   unsigned short size = it->second.size() + 1;
  //   strncpy(buffer, it->second.c_str(), size);
  //   fwrite(&size, sizeof(unsigned short), 1, output_file_);
  //   fwrite(buffer, sizeof(char), size, output_file_);
  //   loadbar(++counter, lexicon_->GetNumLexemes());
  // }
  // std::cout << std::endl << "Finished writing lexicon!" << std::endl;
}

/**
 * Write sorted tuples to file.
 */
void InvertedFileWriter::WriteTuples () {
  // int num_lexemes = lexicon_->GetNumLexemes();
  // int num_tuples = tuple_sorter_->get_num_tuples();
  //   
  // // Save the position to the beginning of the offsets section.
  // fpos_t offsets_pos;
  // fgetpos(output_file_, &offsets_pos);
  // 
  // // Allocate space for the inverted list offsets.
  // int blank = 0;
  // for (int i = 0; i < num_lexemes; i++)
  //   fwrite(&blank, sizeof(int), 1, output_file_);
  //   
  // // Save the position to the beginning of the tuples section.
  // fpos_t tuples_pos;
  // fgetpos(output_file_, &tuples_pos);
  // 
  // unsigned int current_lexeme_id = 0;
  // int inverted_list_num = 0;
  // 
  // int *offsets = new int[num_lexemes];
  // 
  // cout << "Sorting tuples..." << endl;
  // startbar();
  // 
  // int offset = 0;
  // for (int i = 0; i < num_tuples; i++) {
  //   // Write tuple to file.
  //   MergeTuple tuple(tuple_sorter_->PopTuple());
  //   
  //   // cout << "Tuple {" << tuple.lexeme_id << ", " << tuple.document_id << ", " << tuple.word_position << endl;
  //   
  //   fwrite(&tuple.document_id, sizeof(int), 1, output_file_);
  //   fwrite(&tuple.word_position, sizeof(int), 1, output_file_);
  //   
  //   // Save offset.
  //   if ((tuple.lexeme_id != current_lexeme_id && current_lexeme_id != 0)) {
  //     offsets[current_lexeme_id - 1] = offset;
  //     
  //     offset += inverted_list_num;
  //     inverted_list_num = 0;
  //   }
  //   
  //   current_lexeme_id = tuple.lexeme_id;
  //   
  //   if (i == num_tuples - 1)
  //     offsets[current_lexeme_id - 1] = offset;
  //   
  //   inverted_list_num++;
  //   loadbar(i, num_tuples -1);
  // }
  // 
  // cout << endl << "Finished sorting tuples!" << endl;
  // 
  // // Write the offset to the last tuple.
  // fsetpos(output_file_, &offsets_pos);
  // fwrite(offsets, sizeof(int), num_lexemes, output_file_);

  // // TODO: entender isso.
  // rewind(output_file_);
  // 
  // tuple_sorter_->Clear();
  // delete offsets;
}

/**
 * Create an inverted file from a collection of compressed documents.
 *
 */
void InvertedFileWriter::Write(
  const std::string& tuple_file, 
  const std::string& inverted_file
) {
  output_file_ = fopen(inverted_file.c_str(), "wb");

  // Sort tuples.
  // tuple_sorter_->Sort(tuple_file);

  // Write Inverted File.
  WriteHeader();
  WriteLexicon();
  WriteTuples();

  fclose(output_file_);
}
