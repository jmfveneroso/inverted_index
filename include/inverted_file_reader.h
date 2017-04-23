/**
 * InvertedFileReader object
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __INVERTED_FILE_READER_H__
#define __INVERTED_FILE_READER_H__

#include <cstring>
#include <iostream>

#include "tuple.h"
#include "lexicon.h"

namespace TP1 {

class IInvertedFileReader {
 public:
  ~IInvertedFileReader() {}

  virtual int get_num_documents() = 0;
  virtual int get_num_lexemes() = 0;
  virtual int get_num_tuples() = 0;
 
  virtual void LoadFromFile(const std::string&) = 0;
  virtual std::vector<Tuple> GetTuples(const int& lexeme_id) = 0;
  virtual std::vector<Tuple> GetTuples(const std::string& lexeme) = 0;
  virtual void Print() = 0;
};

class InvertedFileReader : public IInvertedFileReader {
  std::shared_ptr<ILexicon> lexicon_;
 
  FILE* file_;
 
  int num_documents_;
  int num_lexemes_;
  int num_tuples_;
 
  fpos_t offsets_section_;
  fpos_t tuples_section_;

 public:
  InvertedFileReader(std::shared_ptr<ILexicon> lexicon);
  ~InvertedFileReader();

  int get_num_documents();
  int get_num_lexemes();
  int get_num_tuples();
 
  void LoadFromFile(const std::string&);
  std::vector<Tuple> GetTuples(const int& lexeme_id);
  std::vector<Tuple> GetTuples(const std::string& lexeme);
  void Print();
};
  
} // End of namespace.

#endif
