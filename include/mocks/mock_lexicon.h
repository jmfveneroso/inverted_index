//
//  MOCK_LEXICON_H
//  TP2
//
//  Created by João Mateus de Freitas Veneroso on 5/11/15.
//  Copyright (c) 2015 João Mateus de Freitas Veneroso. All rights reserved.
//

#ifndef MOCK_LEXICON_H
#define MOCK_LEXICON_H

#include "lexicon.h"
#include <cstdio>

namespace TP1 {
  
class MockLexicon : public ILexicon {
  public:
    MockLexicon () {
      lexeme_map.insert(std::pair<std::string, int>("pease", 1));
      lexeme_map.insert(std::pair<std::string, int>("porridge", 2));
      lexeme_map.insert(std::pair<std::string, int>("hot", 3));
      lexeme_map.insert(std::pair<std::string, int>("cold", 4));
      lexeme_map.insert(std::pair<std::string, int>("in", 5));
      lexeme_map.insert(std::pair<std::string, int>("the", 6));
      lexeme_map.insert(std::pair<std::string, int>("pot", 7));
      lexeme_map.insert(std::pair<std::string, int>("nine", 8));
      lexeme_map.insert(std::pair<std::string, int>("days", 9));
      lexeme_map.insert(std::pair<std::string, int>("old", 10));
      lexeme_map.insert(std::pair<std::string, int>("some", 11));
      lexeme_map.insert(std::pair<std::string, int>("like", 12));
      lexeme_map.insert(std::pair<std::string, int>("it", 13));
      lexeme_map.insert(std::pair<std::string, int>("a", 14));
      lexeme_map.insert(std::pair<std::string, int>("pontuacao", 15));
      lexeme_map.insert(std::pair<std::string, int>("nao", 16));
      lexeme_map.insert(std::pair<std::string, int>("esta", 17));
      lexeme_map.insert(std::pair<std::string, int>("certa", 18));
                        
      id_map.insert(std::pair<int, std::string>(1, "pease"));
      id_map.insert(std::pair<int, std::string>(2, "porridge"));
      id_map.insert(std::pair<int, std::string>(3, "hot"));
      id_map.insert(std::pair<int, std::string>(4, "cold"));
      id_map.insert(std::pair<int, std::string>(5, "in"));
      id_map.insert(std::pair<int, std::string>(6, "the"));
      id_map.insert(std::pair<int, std::string>(7, "pot"));
      id_map.insert(std::pair<int, std::string>(8, "nine"));
      id_map.insert(std::pair<int, std::string>(9, "days"));
      id_map.insert(std::pair<int, std::string>(10, "old"));
      id_map.insert(std::pair<int, std::string>(11, "some"));
      id_map.insert(std::pair<int, std::string>(12, "like"));
      id_map.insert(std::pair<int, std::string>(13, "it"));
      id_map.insert(std::pair<int, std::string>(14, "a"));
      id_map.insert(std::pair<int, std::string>(15, "pontuacao"));
      id_map.insert(std::pair<int, std::string>(16, "nao"));
      id_map.insert(std::pair<int, std::string>(17, "esta"));
      id_map.insert(std::pair<int, std::string>(18, "certa"));
    }
  
    std::vector<std::string> get_inserted_lexemes () {
      return inserted_lexemes_;
    }
  
    std::map<std::string, int> get_lexeme_map () {
      return lexeme_map;
    }
  
    std::map<int, std::string> get_id_map () {
      return id_map;
    }
  
    static char GetValidCharacter(int c) { return '\0'; }
  
    static std::vector<std::string> ExtractLexemes(int doc_id, std::string text) {
      std::vector<std::string> lexemes;
      
      switch (doc_id) {
        case 0:
          lexemes.push_back("pease");
          lexemes.push_back("porridge");
          lexemes.push_back("hot");
          lexemes.push_back("pease");
          lexemes.push_back("porridge");
          lexemes.push_back("cold");
          lexemes.push_back("pease");
          lexemes.push_back("porridge");
          lexemes.push_back("in");
          lexemes.push_back("the");
          lexemes.push_back("pot");
          lexemes.push_back("nine");
          lexemes.push_back("days");
          lexemes.push_back("old");
          break;
        case 1:
          lexemes.push_back("some");
          lexemes.push_back("like");
          lexemes.push_back("it");
          lexemes.push_back("hot");
          lexemes.push_back("some");
          lexemes.push_back("like");
          lexemes.push_back("it");
          lexemes.push_back("cold");
          lexemes.push_back("some");
          lexemes.push_back("like");
          lexemes.push_back("it");
          lexemes.push_back("in");
          lexemes.push_back("the");
          lexemes.push_back("pot");
          lexemes.push_back("nine");
          lexemes.push_back("days");
          lexemes.push_back("old");
        case 2:
          lexemes.push_back("a");
          lexemes.push_back("pontuacao");
          lexemes.push_back("nao");
          lexemes.push_back("esta");
          lexemes.push_back("certa");
        default:
          break;
      }
      
      return lexemes;
    }
  
    int AddLexeme(const std::string& lexeme) {
      inserted_lexemes_.push_back(lexeme);
      std::map<std::string, int>::iterator it = lexeme_map.find(lexeme);
      if (it == lexeme_map.end())
        return -1;
      return it->second;
    }
  
    int GetLexemeId(const std::string &lexeme) {
      std::map<std::string, int>::iterator it = lexeme_map.find(lexeme);
      if (it == lexeme_map.end())
        return -1;
      return it->second;
    }
  
    std::string GetLexemeById(const int& id) {
      return "";
    }
  
    int GetNumLexemes() { return 18; }
  
    void LoadFromFile(const string& filename) {};
    void LoadFromInvertedFile (const std::string& filename) {}
    void WriteToFile(const std::string& filename) {};
  
  private:
    std::map<std::string, int> lexeme_map;
    std::map<int, std::string> id_map;
    std::vector<std::string> inserted_lexemes_;
};
  
} // Namespace TP1.

#endif
