#ifndef __LEXICON_HPP__
#define __LEXICON_HPP__

#include <memory>
#include <string>
#include <vector>
#include <map>

#include "env.h"
#include "tuple.hpp"
#include "logger.hpp"

namespace InvertedIndex {

struct Lexeme {
  std::string lexeme;
  off_t offset;  
  off_t anchor_offset = 0;  
  size_t doc_frequency;
  size_t anchor_refs;
  std::map<unsigned int, unsigned int> links;
  Lexeme() {}
  Lexeme(std::string lexeme) : lexeme(lexeme) {}
};

class ILexicon {
 public:
  virtual ~ILexicon() {};

  virtual unsigned int AddLexeme(const std::string& lexeme) = 0;
  virtual unsigned int GetLexemeId(const std::string &lexeme) = 0;
  virtual Lexeme GetLexemeById(unsigned int id) = 0;
  virtual size_t GetNumLexemes() = 0;
 
  virtual void Write(FILE*, off_t) = 0;
  virtual void Load(FILE*, off_t, size_t) = 0;
  virtual void Print() {}
  virtual void SetOffset(unsigned int, off_t) = 0;
  virtual void SetAnchorOffset(unsigned int, off_t) = 0;
  virtual void SetDocFrequency(unsigned int, size_t) = 0;
};

class Lexicon : public ILexicon{
  std::shared_ptr<ILogger> logger_;
  
  unsigned int id_counter_;
  std::map<std::string, unsigned int> lexeme_map_;
  std::map<unsigned int, Lexeme> id_map_;

 public:
  Lexicon(std::shared_ptr<ILogger>);

  unsigned int AddLexeme(const std::string& lexeme);
  unsigned int GetLexemeId(const std::string &lexeme);
  Lexeme GetLexemeById(unsigned int id);
  size_t GetNumLexemes();
  
  void Write(FILE*, off_t);
  void Load(FILE*, off_t, size_t);
  void Print();

  void SetOffset(unsigned int id, off_t offset) { id_map_.find(id)->second.offset = offset; }
  void SetAnchorOffset(unsigned int id, off_t offset) { id_map_.find(id)->second.anchor_offset = offset; }
  void SetDocFrequency(unsigned int id, size_t doc_freq) { id_map_.find(id)->second.doc_frequency = doc_freq; }

  static std::vector<size_t> lexeme_offsets;
  static char GetValidCharacter(int);
  static void ClearOffsets();
  static std::vector<std::string> ExtractLexemes(std::string);
};
  
} // End of namespace.

#endif
