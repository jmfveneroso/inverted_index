#include "lexicon.h"
#include "utf8cpp/utf8.h"

using namespace std;
using namespace TP1;
using namespace utf8;

Lexicon::Lexicon(std::shared_ptr<ILogger> logger) 
  : logger_(logger), id_counter_(0) {
}

std::map<std::string, int> Lexicon::get_lexeme_map () {
  return lexeme_map_;
}

std::map<int, std::string> Lexicon::get_id_map () {
  return id_map_;
}

/**
 * Transform accented characters and uppercase characters into
 * its unnacented lowercase form.
 *
 */
char Lexicon::GetValidCharacter (int c) {
  if (c >= 0x41 && c <= 0x5A) { // A-Z to a-z.
      return c + 0x20;
  } else if (c >= 0x61 && c <= 0x7A) { // a-z remains a-z.
      return c;
  } else if (c >= 0xC0 && c <= 0xFF) { // Accented characters to unaccented.
      static const char*
      //   "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ"
      tr = "aaaaaaeceeeeiiiidnoooooxouuuuypsaaaaaaeceeeeiiiionooooooouuuuypy";
      return tr[c - 0xC0];
  } else { // Non alphanumeric characters to null.
      return '\0';
  }
}

/**
 * Extract valid lexemes from text.
 *
 */
vector<string> Lexicon::ExtractLexemes (string text) {
  vector<string> result;
  
  string lexeme;
  string::iterator current_it = text.begin();
  
  int utf8char;
  while (current_it != text.end()) {
    utf8char = next(current_it, text.end());
    char c = Lexicon::GetValidCharacter(utf8char);
    if (c != '\0') {
        lexeme += c;
    } else if (lexeme.length() > 0 && lexeme.length() < MAX_LEXEME_LENGTH) {
        result.push_back(lexeme);
        lexeme.clear();
    } else {
        lexeme.clear();
    }
  }
  
  if (lexeme.length() > 0 && lexeme.length() < MAX_LEXEME_LENGTH)
    result.push_back(lexeme);
  
  return result;
}

/**
 * Inserts a new lexeme in the lexicon or returns its id if it has already been inserted.
 *
 */
int Lexicon::AddLexeme(const string& lexeme) {
  int id;
  if ((id = GetLexemeId(lexeme)) == -1) {
    id_counter_++;
    lexeme_map_.insert(pair<string, int>(lexeme, id_counter_));
    id_map_.insert(pair<int, string>(id_counter_, lexeme));
    return id_counter_;
  } else
    return id;
}

/**
 * Gets a lexeme id or -1 if it does not exist.
 *
 */
int Lexicon::GetLexemeId(const string &lexeme) {
  map<string, int>::iterator it;
  if ((it = lexeme_map_.find(lexeme)) == lexeme_map_.end())
    return -1;
  else
    return it->second;
}

string Lexicon::GetLexemeById(const int &id) {
  map<int, string>::iterator it;
  if ((it = id_map_.find(id)) == id_map_.end())
    return "";
  else
    return it->second;
}


int Lexicon::GetNumLexemes() {
  return lexeme_map_.size();
}

void Lexicon::WriteToFile(const string& filename) {
  FILE *file = fopen(filename.c_str(), "wb");
  
  char buffer[MAX_LEXEME_LENGTH];
  std::map<int, string>::iterator it = id_map_.begin();

  logger_->Log("Started writing lexicon.");
  int counter = 0;
  for (; it != id_map_.end(); ++it) {
    unsigned short size = it->second.size() + 1;
    strncpy(buffer, it->second.c_str(), size);
    fwrite(&size, sizeof(unsigned short), 1, file);
    fwrite(buffer, sizeof(char), size, file);
    if (++counter % 100000 == 0)
      logger_->Log(std::to_string(counter) + " lexemes were written.");
  }
  logger_->Log(
    "Finished writing lexicon. " + std::to_string(counter) + 
    " lexemes were written to file " + filename + "."
  );
  
  fclose(file);
}

void Lexicon::LoadFromFile(const string& filename) {
  std::cout << "Loading lexicon..." << std::endl;
  FILE *file = fopen(filename.c_str(), "rb");
  char buffer[255];
  short int size;
  while (fread(&size, sizeof(short int), 1, file)) {
    fread(buffer, sizeof(char), size, file);
    AddLexeme(buffer);
  }
  std::cout << "Finished loading lexicon!" << std::endl;
}

void Lexicon::LoadFromInvertedFile (const string& filename) {
  FILE *file = fopen(filename.c_str(), "rb");
  
  // Skip num_documents.
  fseek(file, sizeof(int), SEEK_CUR);
  
  int num_lexemes;
  fread(&num_lexemes, sizeof(int), 1, file);
  
  // Skip num_tuples.
  fseek(file, sizeof(int), SEEK_CUR);
  
  // Load lexicon.
  unsigned short str_size;
  char lexeme_str[MAX_LEXEME_LENGTH];
  for (int i = 0; i < num_lexemes; i++) {
    fread(&str_size, sizeof(unsigned short), 1, file);
    fread(&lexeme_str, sizeof(char), str_size, file);
    
    AddLexeme(lexeme_str);
  }
}

void Lexicon::Print() {
  std::map<int, string>::iterator it = id_map_.begin();
  for (; it != id_map_.end(); ++it)
    std::cout << "Lexeme " << it->first << ": " << it->second << std::endl;
}
