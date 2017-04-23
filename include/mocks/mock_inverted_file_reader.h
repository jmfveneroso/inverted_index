//
//  MOCK_INVERTED_FILE_READER_H
//  TP1
//
//  Created by João Mateus de Freitas Veneroso on 5/11/15.
//  Copyright (c) 2015 João Mateus de Freitas Veneroso. All rights reserved.
//

#ifndef MOCK_INVERTED_FILE_READER_H
#define MOCK_INVERTED_FILE_READER_H

#include "mock_lexicon.h"
#include "inverted_file_reader.h"

using namespace std;

namespace TP1 {
  
  class MockInvertedFileReader : public IInvertedFileReader {
    public:
      string get_filename() { return "mock_inverted_file.dat"; }
    
      ILexicon *get_lexicon() { return &mock_lexicon; }
      int get_num_documents() { return 3; }
      int get_num_lexemes() { return 18; }
      int get_num_tuples() { return 36; }
    
      std::vector<Tuple> GetTuples (const int& lexeme_id) {
        vector<Tuple> tuples;
        
        switch (lexeme_id) {
          case 1:
            tuples.push_back(Tuple(1, 0, 1));
            tuples.push_back(Tuple(1, 0, 4));
            tuples.push_back(Tuple(1, 0, 7));
            break;
          case 2:
            tuples.push_back(Tuple(2, 0, 2));
            tuples.push_back(Tuple(2, 0, 5));
            tuples.push_back(Tuple(2, 0, 8));
            break;
          case 3 :
            tuples.push_back(Tuple(3, 0, 3));
            tuples.push_back(Tuple(3, 1, 4));
            break;
          case 4:
            tuples.push_back(Tuple(4, 0, 6));
            tuples.push_back(Tuple(4, 1, 8));
            break;
          case 5:
            tuples.push_back(Tuple(5, 0, 9));
            tuples.push_back(Tuple(5, 1, 12));
            break;
          case 6:
            tuples.push_back(Tuple(6, 0, 10));
            tuples.push_back(Tuple(6, 1, 13));
            break;
          case 7:
            tuples.push_back(Tuple(7, 0, 11));
            tuples.push_back(Tuple(7, 1, 14));
            break;
          case 8:
            tuples.push_back(Tuple(8, 0, 12));
            tuples.push_back(Tuple(8, 1, 15));
            break;
          case 9:
            tuples.push_back(Tuple(9, 0, 13));
            tuples.push_back(Tuple(9, 1, 16));
            break;
          case 10:
            tuples.push_back(Tuple(10, 0, 14));
            tuples.push_back(Tuple(10, 1, 17));
            break;
          case 11:
            tuples.push_back(Tuple(11, 1, 1));
            tuples.push_back(Tuple(11, 1, 5));
            tuples.push_back(Tuple(11, 1, 9));
            break;
          case 12:
            tuples.push_back(Tuple(12, 1, 2));
            tuples.push_back(Tuple(12, 1, 6));
            tuples.push_back(Tuple(12, 1, 10));
            break;
          case 13:
            tuples.push_back(Tuple(13, 1, 3));
            tuples.push_back(Tuple(13, 1, 7));
            tuples.push_back(Tuple(13, 1, 11));
            break;
          case 14:
            tuples.push_back(Tuple(14, 2, 1));
            break;
          case 15:
            tuples.push_back(Tuple(15, 2, 2));
            break;
          case 16:
            tuples.push_back(Tuple(16, 2, 3));
            break;
          case 17:
            tuples.push_back(Tuple(17, 2, 4));
            break;
          case 18:
            tuples.push_back(Tuple(18, 2, 5));
            break;
        }
        return tuples;
      }
    
      std::vector<Tuple> GetTuples (const std::string& lexeme) {
        if (lexeme == "pease") {
          return GetTuples(1);
        } else if (lexeme == "porridge") {
          return GetTuples(2);
        } else if (lexeme == "hot") {
          return GetTuples(3);
        } else if (lexeme == "cold") {
          return GetTuples(4);
        } else if (lexeme == "in") {
          return GetTuples(5);
        } else if (lexeme == "the") {
          return GetTuples(6);
        } else if (lexeme == "pot") {
          return GetTuples(7);
        } else if (lexeme == "nine") {
          return GetTuples(8);
        } else if (lexeme == "days") {
          return GetTuples(9);
        } else if (lexeme == "old") {
          return GetTuples(10);
        } else if (lexeme == "some") {
          return GetTuples(11);
        } else if (lexeme == "like") {
          return GetTuples(12);
        } else if (lexeme == "it") {
          return GetTuples(13);
        } else if (lexeme == "a") {
          return GetTuples(14);
        } else if (lexeme == "pontuacao") {
          return GetTuples(15);
        } else if (lexeme == "nao") {
          return GetTuples(16);
        } else if (lexeme == "esta") {
          return GetTuples(17);
        } else if (lexeme == "certa") {
          return GetTuples(18);
        }
        
        vector<Tuple> tuples;
        return tuples;
      }
    private:
      MockLexicon mock_lexicon;
  };
  
} // Namespace TP1.

#endif