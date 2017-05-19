#ifndef __BOOLEAN_MODEL_H__
#define __BOOLEAN_MODEL_H__

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <chrono>
#include "inverted_index.hpp"

namespace TP1 {

using namespace std;
using namespace std::chrono;

class BooleanModel {
  std::shared_ptr<IInvertedIndex> inverted_index_;
  std::shared_ptr<DocMap> doc_map_;
  std::shared_ptr<ILexicon> lexicon_;

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
  BooleanModel(
    std::shared_ptr<IInvertedIndex> inverted_index,
    std::shared_ptr<DocMap> doc_map,
    std::shared_ptr<ILexicon> lexicon
  ) : inverted_index_(inverted_index), doc_map_(doc_map), lexicon_(lexicon) {
  }

  std::vector<unsigned int> BooleanQuery(const string& query) {
    // Extract valid lexemes from the query string.
    std::vector<std::string> words = Lexicon::ExtractLexemes(query);
    
    std::vector<unsigned int> result;
    std::vector<PostingsList> postings_lists;

    auto then = system_clock::now();
    std::cout << "Results for: ";    
    for (auto word : words) std::cout << word << ", ";
    std::cout << std::endl;

    for (std::vector<std::string>::iterator it = words.begin(); it < words.end(); it++) {    
      PostingsList postings_list = inverted_index_->GetPostingsList(*it);
      postings_lists.push_back(postings_list);
      
      // Get document ids.
      std::vector<unsigned int> docs;
      for (auto& it : postings_list.word_offsets) docs.push_back(it.first);
     
      if (result.size() == 0) result = docs;
      else result = UniqueDocs(OperatorAnd(result, docs));
    }

    if (postings_lists.size() == 0) std::cout << "No results found" << std::endl;

    size_t counter = 0;
    for (auto& postings_list : postings_lists) {
      std::cout << "Word: " << lexicon_->GetLexemeById(postings_list.lexeme_id).lexeme << std::endl;
      for (auto& it : postings_list.word_offsets) {
        if (std::find(result.begin(), result.end(), it.first) == result.end()) continue;
        std::cout << "(" << it.first << ") " << doc_map_->GetDocById(it.first).url << ": ";

        bool comma = false;
        for (auto& pos : it.second) {
          if (comma) std::cout << ", ";
          std::cout << pos;
          comma = true;
          counter++;
        }
        std::cout << std::endl;
      }
    }
    size_t ms = duration_cast<milliseconds>(
      system_clock::now() - then
    ).count();
    std::cout << "Doc frequency: " << result.size() << std::endl;
    std::cout << "Occurrences: " << counter << std::endl;
    std::cout << "The search took: " << ms << " ms" << std::endl;
  
    return UniqueDocs(result);
  }

  std::vector< std::pair<unsigned int, double> > RankByVectorModel(const string& query) {
    std::vector< std::pair<unsigned int, double> > ranked_docs;
    std::map<unsigned int, double> rank;
    std::vector<std::string> words = Lexicon::ExtractLexemes(query);
    for (auto& word : words) {
      PostingsList postings_list = inverted_index_->GetPostingsList(word);

      for (auto& it : postings_list.word_offsets) {
        if (rank.find(it.first) == rank.end()) rank[it.first] = 0;
        rank[it.first] += log2(1 + it.second.size());
      }
    }

    for (auto& it : rank) {
      Document doc = doc_map_->GetDocById(it.first);
      rank[it.first] /= doc_map_->GetDocById(it.first).vector_norm;
      ranked_docs.push_back({ it.first, rank[it.first] });
    }

    auto cmp = [](std::pair<unsigned int, double> const& a, std::pair<unsigned int, double> const& b) { 
      return a.second > b.second;
    };

    std::sort(ranked_docs.begin(), ranked_docs.end(), cmp);
    return ranked_docs;
  }

  std::vector< std::pair<unsigned int, double> > RankByPageRank(const string& query) {
    std::vector< std::pair<unsigned int, double> > ranked_docs;

    std::vector<unsigned int> docs;
    std::vector<std::string> words = Lexicon::ExtractLexemes(query);
    for (auto& word : words) {
      PostingsList postings_list = inverted_index_->GetPostingsList(word);
      for (auto& it : postings_list.word_offsets) docs.push_back(it.first);
    }

    for (auto& doc_id : docs)
      ranked_docs.push_back({ doc_id, doc_map_->GetDocById(doc_id).page_rank });

    auto cmp = [](std::pair<unsigned int, double> const& a, std::pair<unsigned int, double> const& b) { 
      return a.second > b.second;
    };

    std::sort(ranked_docs.begin(), ranked_docs.end(), cmp);
    return ranked_docs;
  }

  // std::vector<unsigned int, double> RankByAnchorText(const string& query) {
  // }
};
     
} // End of namespace.

#endif
