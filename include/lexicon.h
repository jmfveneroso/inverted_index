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

class ILexicon {
 public:
  virtual ~ILexicon() {};

  virtual std::map<std::string, int> get_lexeme_map() = 0;
  virtual std::map<int, std::string> get_id_map() = 0;
 
  virtual int AddLexeme(const std::string& lexeme) = 0;
  virtual int GetLexemeId(const std::string &lexeme) = 0;
  virtual std::string GetLexemeById(const int &id) = 0;
  virtual int GetNumLexemes() = 0;
 
  virtual void LoadFromFile(const string& filename) = 0;
  virtual void LoadFromInvertedFile(const std::string& filename) = 0;
  virtual void WriteToFile(const std::string& filename) = 0;
  virtual void Print() {}
};

class Lexicon : public ILexicon{
  std::shared_ptr<ILogger> logger_;
  
  unsigned int id_counter_;
  std::map<std::string, int> lexeme_map_;
  std::map<int, std::string> id_map_;

 public:
  Lexicon(std::shared_ptr<ILogger>);

  std::map<std::string, int> get_lexeme_map();
  std::map<int, std::string> get_id_map();
  static char GetValidCharacter(int c);
  static std::vector<std::string> ExtractLexemes(std::string text);
  
  int AddLexeme(const std::string& lexeme);
  int GetLexemeId(const std::string &lexeme);
  std::string GetLexemeById(const int &id);
  int GetNumLexemes();
  
  void LoadFromFile(const string& filename);
  void LoadFromInvertedFile(const std::string& filename);
  void WriteToFile(const std::string& filename);
  void Print();
};
  
} // End of namespace.

#endif
