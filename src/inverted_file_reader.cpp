//
//  inverted_file_reader.cpp
//  tp2
//
//  Created by João Mateus de Freitas Veneroso on 5/22/15.
//  Copyright (c) 2015 João Mateus de Freitas Veneroso. All rights reserved.
//

#include "inverted_file_reader.h"

using namespace std;
using namespace TP1;

InvertedFileReader::InvertedFileReader(std::shared_ptr<ILexicon> lexicon) 
  : lexicon_(lexicon) {
}

InvertedFileReader::~InvertedFileReader() {
  if (file_ != NULL) fclose(file_);
}

int InvertedFileReader::get_num_documents () {
  return num_documents_;
}

int InvertedFileReader::get_num_lexemes () {
  return num_lexemes_;
}

int InvertedFileReader::get_num_tuples () {
  return num_tuples_;
}

/**
 * Load inverted file from disk, initializing the lexicon,
 * the document catalog and calculating the beginning of the
 * offsets section and the tuples section.
 */
void InvertedFileReader::LoadFromFile(const string& filename) {
  file_ = fopen(filename.c_str(), "rb");

  fread(&num_documents_, sizeof(int), 1, file_);
  fread(&num_lexemes_, sizeof(int), 1, file_);
  fread(&num_tuples_, sizeof(int), 1, file_);
  
  // Load lexicon.
  unsigned short str_size;
  char lexeme_str[MAX_LEXEME_LENGTH];
  for (int i = 0; i < num_lexemes_; i++) {
    fread(&str_size, sizeof(unsigned short), 1, file_);
    fread(&lexeme_str, sizeof(char), str_size, file_);
    
    lexicon_->AddLexeme(lexeme_str);
  }
  
  // Get position to offsets section and tuples section.
  fgetpos(file_, &offsets_section_);
  fseek(file_, sizeof(int) * num_lexemes_, SEEK_CUR);
  fgetpos(file_, &tuples_section_);
}

vector<Tuple> InvertedFileReader::GetTuples(const int& lexeme_id) {  
  // Find inverted list offset.
  fsetpos(file_, &offsets_section_);
  fseek(file_, (lexeme_id - 1) * sizeof(int), SEEK_CUR);
  
  int offset;
  fread(&offset, sizeof(int), 1, file_);
  
  int offset_end;
  
  // Check if this is the last tuple.
  if (lexeme_id == num_lexemes_)
    offset_end = num_tuples_;
  else
    fread(&offset_end, sizeof(int), 1, file_);
  
  // Find number of tuples in the inverted list.
  int num_tuples = offset_end - offset;
  
  vector<Tuple> tuples;
  fsetpos(file_, &tuples_section_);
  fseek(file_, offset * sizeof(int) * 2, SEEK_CUR);
  
  // Get all tuples.
  for (int i = 0; i < num_tuples; i++) {
    Tuple tuple(lexeme_id, 0, 0);
    fread(&tuple.document_id, sizeof(int), 1, file_);
    fread(&tuple.word_position, sizeof(int), 1, file_);
    tuples.push_back(tuple);
  }
  
  return tuples;
}

/**
 * Read all tuples for a given lexeme id.
 */
vector<Tuple> InvertedFileReader::GetTuples (const string& lexeme) {
  if (lexeme.compare("a") == 0 || lexeme.compare("o") == 0 || lexeme.compare("e") == 0 ||
      lexeme.compare("da") == 0 || lexeme.compare("do") == 0 || lexeme.compare("de") == 0 ||
      lexeme.compare("das") == 0 || lexeme.compare("dos") == 0 || lexeme.compare("and") == 0 ||
      lexeme.compare("of") == 0 || lexeme.compare("the") == 0 || lexeme.compare("is") == 0) {
    vector<Tuple> v;
    return v;
  }
  
  int lexeme_id = lexicon_->GetLexemeId(lexeme);
  if (lexeme_id == -1) {
    vector<Tuple> v;
    return v;
  } else {
    return GetTuples(lexeme_id);
  }
}

void InvertedFileReader::Print() {
  std::cout << "Num documents: " << num_documents_ << std::endl;
  std::cout << "Num lexemes: " << num_lexemes_ << std::endl;
  std::cout << "Num tuples: " << num_tuples_ << std::endl;
  std::cout << "=====================" << std::endl;
  
  // Get position to offsets section and tuples section.
  fsetpos(file_, &offsets_section_);

  for (int i = 0; i < num_lexemes_; ++i) {
    fsetpos(file_, &offsets_section_);
    fseek(file_, i * sizeof(int), SEEK_CUR);

    int offset;
    fread(&offset, sizeof(int), 1, file_);
    std::cout << "Lexeme ("  << i + 1 <<  "): " << lexicon_->GetLexemeById(i + 1) << 
                 ", offset: " << offset << ", tuples: ";

    vector<Tuple> tuples = GetTuples(i + 1);
    for (int i = 0; i < tuples.size(); ++i) {
      Tuple& t = tuples[i];
      std::cout << "{" << t.lexeme_id << ", "
              << t.document_id << ", " << t.word_position << "}" << ((i != tuples.size() - 1) ? "," : "");
    }
    std::cout << std::endl;
  }
}
