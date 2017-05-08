#ifndef __BOOLEAN_MODEL_H__
#define __BOOLEAN_MODEL_H__

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include "inverted_index.hpp"

namespace TP1 {

using namespace std;

class BooleanModel {
  std::shared_ptr<IInvertedIndex> inverted_index_;
 
  static std::vector<unsigned int> UniqueDocs(vector<unsigned int> v) {
    // Get only unique document ids.
    vector<unsigned int>::iterator it;
    sort(v.begin(), v.end());
    it = unique(v.begin(), v.end());
    
    // Resize the vector to fit all elements properly.
    v.resize(std::distance(v.begin(), it));
    
    return v;
  }
  
  static std::vector<unsigned int> OperatorAnd(vector<unsigned int> v1, vector<unsigned int> v2) {
    // Sort both vectors.
    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
    
    // Get intersection between vectors.
    std::vector<unsigned int> v_intersection;
    std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v_intersection));
    
    return v_intersection;
  }
  
  static std::vector<unsigned int> OperatorOr(vector<unsigned int> v1, vector<unsigned int> v2) {
    std::vector<unsigned int> result;
    
    // Concat both vectors.
    result.reserve(v1.size() + v2.size());
    result.insert(result.end(), v1.begin(), v1.end());
    result.insert(result.end(), v2.begin(), v2.end());
    
    return UniqueDocs(result);
  }
 
 public:
  BooleanModel(std::shared_ptr<IInvertedIndex> inverted_index) : inverted_index_(inverted_index) {}

  std::vector<unsigned int> BooleanQuery(const string& query) {
    // Extract valid lexemes from the query string.
    std::vector<std::string> words = Lexicon::ExtractLexemes(query);
    
    std::vector<unsigned int> result;
    
    bool op_and = false, op_or = false;
    for (std::vector<std::string>::iterator it = words.begin(); it < words.end(); it++) {    
      if (it->compare("and") == 0 || it->compare("AND") == 0) {
        if (op_and || op_or)
          cout << "invalid query" << endl;
        else
          op_and = true;
      } else if (it->compare("or") == 0 || it->compare("OR") == 0) {
        if(op_and || op_or)
          cout << "invalid query" << endl;
        else
          op_or = true;
      } else {
        PostingsList postings_list = inverted_index_->GetPostingsList(*it);
        
        // Get document ids.
        std::vector<unsigned int> docs;
        for (auto& it : postings_list.word_offsets) docs.push_back(it.first);
        
        if (postings_list.word_offsets.size() == 0 && op_and) {
          result.clear();
        } else {
          if (op_and)
            result = OperatorAnd(result, docs);
          else if (op_or)
            result = OperatorOr(result, docs);
          else {
            if(result.size() == 0)
              result = docs;
            else
              result = OperatorOr(result, docs);
          }
        }
        
        op_and = false;
        op_or = false;
      }
    }
  
    return UniqueDocs(result);
  }
};
     
} // End of namespace.

#endif
