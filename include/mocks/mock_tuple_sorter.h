//
//  MOCK_TUPLE_SORTER_H
//  TP1
//
//  Created by João Mateus de Freitas Veneroso on 5/11/15.
//  Copyright (c) 2015 João Mateus de Freitas Veneroso. All rights reserved.
//

#ifndef MOCK_TUPLE_SORTER_H
#define MOCK_TUPLE_SORTER_H

#include "mock_lexicon.h"
#include "tuple_sorter.h"

using namespace std;

namespace TP1 {

class MockTupleSorter : public ITupleSorter {
  public:
    explicit MockTupleSorter(ILexicon *lexicon) {
      counter_ = 0;
      
      tuples_[0] = Tuple(1, 0, 1);
      tuples_[1] = Tuple(1, 0, 4);
      tuples_[2] = Tuple(1, 0, 7);
      tuples_[3] = Tuple(2, 0, 2);
      tuples_[4] = Tuple(2, 0, 5);
      tuples_[5] = Tuple(2, 0, 8);
      tuples_[6] = Tuple(3, 0, 3);
      tuples_[7] = Tuple(3, 1, 4);
      tuples_[8] = Tuple(4, 0, 6);
      tuples_[9] = Tuple(4, 1, 8);
      tuples_[10] = Tuple(5, 0, 9);
      tuples_[11] = Tuple(5, 1, 12);
      tuples_[12] = Tuple(6, 0, 10);
      tuples_[13] = Tuple(6, 1, 13);
      tuples_[14] = Tuple(7, 0, 11);
      tuples_[15] = Tuple(7, 1, 14);
      tuples_[16] = Tuple(8, 0, 12);
      tuples_[17] = Tuple(8, 1, 15);
      tuples_[18] = Tuple(9, 0, 13);
      tuples_[19] = Tuple(9, 1, 16);
      tuples_[20] = Tuple(10, 0, 14);
      tuples_[21] = Tuple(10, 1, 17);
      tuples_[22] = Tuple(11, 1, 1);
      tuples_[23] = Tuple(11, 1, 5);
      tuples_[24] = Tuple(11, 1, 9);
      tuples_[25] = Tuple(12, 1, 2);
      tuples_[26] = Tuple(12, 1, 6);
      tuples_[27] = Tuple(12, 1, 10);
      tuples_[28] = Tuple(13, 1, 3);
      tuples_[29] = Tuple(13, 1, 7);
      tuples_[30] = Tuple(13, 1, 11);
      tuples_[31] = Tuple(14, 2, 1);
      tuples_[32] = Tuple(15, 2, 2);
      tuples_[33] = Tuple(16, 2, 3);
      tuples_[34] = Tuple(17, 2, 4);
      tuples_[35] = Tuple(18, 2, 5);
    }
  
    void Initialize() {}
  
    ILexicon *get_lexicon() {
      return &mock_lexicon_;
    };
  
    int get_num_tuples() {
      return 36;
    }
  
    void set_doc_map (map<std::string, int> doc_map) {}
  
    void Sort() {}
  
    void ExtractTuplesFromDoc (int doc_id, const string& document) {}
    void ExtractTuplesFromAnchorText(const std::string& document) {}
    void ExtractTuplesFromLinks(GumboNode* node) {};
  
    MergeTuple PopTuple () {
      return MergeTuple(0, tuples_[counter_++]);
    }
  
    void Clear () {}
  
  private:
    MockLexicon mock_lexicon_;
  
    Tuple tuples_[36];
    int counter_;
  
    static std::string CleanText(GumboNode* node) { return ""; };
    void FillQueue() {};
    void SortBlocks(const std::string&) {};
};
  
} // Namespace TP1.

#endif
