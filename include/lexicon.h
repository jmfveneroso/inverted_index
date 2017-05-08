#ifndef __LEXICON_H__
#define __LEXICON_H__

#include <memory>
#include <string>
#include <vector>
#include <map>

#include "util.h"
#include "env.h"
#include "tuple.h"
#include "logger.hpp"

namespace TP1 {

struct Lexeme {
  std::string lexeme;
  off_t offset;  
  off_t anchor_offset;  
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
  virtual void AddLink(unsigned int, size_t) = 0;
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

  void SetOffset(unsigned int id, off_t offset) { id_map_[id].offset = offset; }
  void SetAnchorOffset(unsigned int id, off_t offset) { id_map_[id].anchor_offset = offset; }
  void SetDocFrequency(unsigned int id, size_t doc_freq) { id_map_[id].doc_frequency = doc_freq; }
  void AddLink(unsigned int id, size_t link_id) { 
    if (id_map_[id].links.find(link_id) == id_map_[id].links.end()) 
      id_map_[id].links[link_id] = 0;
    id_map_[id].links[link_id]++;
  }

  static char GetValidCharacter(int);
  static std::vector<std::string> ExtractLexemes(std::string);
};
  
} // End of namespace.

#endif
