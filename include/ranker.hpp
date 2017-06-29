#ifndef __RANKER_HPP__
#define __RANKER_HPP__

#include <algorithm>
#include <memory>
#include <chrono>
#include "inverted_index.hpp"

namespace InvertedIndex {

using namespace std::chrono;

struct QueryResult {
 public:
  unsigned int id;
  double rank;
  double vector_rank;
  double anchor_rank;
  double page_rank;
  bool remove = false;
  std::string url;
  std::string title;
  std::string short_text;
  std::vector<unsigned int> positions;

  QueryResult() : rank(0), anchor_rank(0), page_rank(0) {}
  QueryResult(
    unsigned int id, double rank, std::string url, 
    std::string title, std::string short_text
  ) : id(id), rank(rank), url(url), title(title), short_text(short_text) {}
};

class Ranker {
  std::shared_ptr<IInvertedIndex> inverted_index_;
  std::shared_ptr<DocMap> doc_map_;
  std::shared_ptr<ILexicon> lexicon_;
  std::shared_ptr<Extractor> extractor_;

  std::vector<unsigned int> query_lexeme_ids_;
  std::map<unsigned int, QueryResult> query_results_;
  std::vector<unsigned int> sorted_results_;
  bool retrieve_short_text_ = true;

  void SortQueryTerms();
  void ProcessQueryTerms();
  void ProcessAnchorRank();
  void SortResults(double, double, double);
  static std::vector<unsigned int> UniqueDocs(std::vector<unsigned int>);
  static std::vector<unsigned int> OperatorAnd(std::vector<unsigned int>, std::vector<unsigned int>);
 
 public:
  Ranker(
    std::shared_ptr<IInvertedIndex> inverted_index,
    std::shared_ptr<DocMap> doc_map,
    std::shared_ptr<ILexicon> lexicon,
    std::shared_ptr<Extractor> extractor
  );

  void set_retrieve_short_text(bool flag) { retrieve_short_text_ = flag; }
  void Search(const std::string&, double, double, double);
  std::vector<QueryResult> GetPageOfResults(size_t page_num = 1, size_t results_per_page = 10);
  size_t GetNumResults() { return query_results_.size(); }
};
     
} // End of namespace.

#endif
