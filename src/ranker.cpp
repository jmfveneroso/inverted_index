#include "ranker.hpp"

namespace InvertedIndex {

using namespace std::chrono;

Ranker::Ranker(
  std::shared_ptr<IInvertedIndex> inverted_index,
  std::shared_ptr<DocMap> doc_map,
  std::shared_ptr<ILexicon> lexicon,
  std::shared_ptr<Extractor> extractor
) : inverted_index_(inverted_index), doc_map_(doc_map), lexicon_(lexicon),
    extractor_(extractor) {
}

std::vector<unsigned int> Ranker::UniqueDocs(std::vector<unsigned int> v) {
  // Get only unique document ids.
  std::vector<unsigned int>::iterator it;
  std::sort(v.begin(), v.end());
  it = std::unique(v.begin(), v.end());
  
  // Resize the vector to fit all elements properly.
  v.resize(std::distance(v.begin(), it));
  
  return v;
}

std::vector<unsigned int> Ranker::OperatorAnd(std::vector<unsigned int> v1, std::vector<unsigned int> v2) {
  // Sort both vectors.
  std::sort(v1.begin(), v1.end());
  std::sort(v2.begin(), v2.end());
  
  // Get intersection between vectors.
  std::vector<unsigned int> v_intersection;
  std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v_intersection));
  
  return v_intersection;
}

// Sorts query terms by increasing document frequency.  
void Ranker::SortQueryTerms() {
  // Not very efficient, but queries are very small, so no huge problem here.
  std::sort(query_lexeme_ids_.begin(), query_lexeme_ids_.end(), 
    [=](const unsigned int& id1, const unsigned int& id2) {
      return lexicon_->GetLexemeById(id1).doc_frequency < 
        lexicon_->GetLexemeById(id2).doc_frequency;
    }
  );
}

void Ranker::ProcessQueryTerms() {
  for (auto id : query_lexeme_ids_) {
    PostingsList postings_list = inverted_index_->GetPostingsList(id);
 
    Lexeme lexeme = lexicon_->GetLexemeById(id);
    for (auto& it : query_results_) it.second.remove = true;

    // We will also calculate the vector rank for all documents 
    // while processing the postings list. Each query_result.rank
    // acts as an accumulator.
    double idf = log2(1 + (doc_map_->GetNumDocs() / lexeme.doc_frequency));

    bool empty_set = query_results_.empty();
    for (auto& it : postings_list.word_offsets) {
      double tf = log2(1 + it.second.size());

      if (empty_set) {
        QueryResult query_result;
        query_result.id = it.first;
        query_result.positions = it.second;
        query_result.vector_rank = tf * idf;
        query_result.remove = false;
        query_results_[it.first] = query_result;
      } else if (query_results_.find(it.first) != query_results_.end()) {
        // Only adds documents that have appeared in previous terms.
        std::vector<unsigned int>& positions = query_results_[it.first].positions;
        positions.insert(positions.end(), it.second.begin(), it.second.end());
        query_results_[it.first].vector_rank += tf * idf;
        query_results_[it.first].remove = false;
      }
    }

    // Only keep documents that contain all terms.
    for (auto it = query_results_.begin(); it != query_results_.end();) {
      if (it->second.remove) it = query_results_.erase(it);
      else it++;
    }
  }

  // Divides ranks by vector norms.
  for (auto& it: query_results_)
    it.second.vector_rank /= doc_map_->GetDocById(it.first).vector_norm;
}

void Ranker::ProcessAnchorRank() {
  for (auto id : query_lexeme_ids_) {
    PostingsList postings_list = inverted_index_->GetPostingsList(id);
 
    std::map<unsigned int, unsigned int> result;
    result = inverted_index_->GetAnchorRank(id);

    for (auto it : result) {
      if (query_results_.find(it.first) == query_results_.end()) continue;
      query_results_[it.first].anchor_rank += it.second;
    }
  }
}

void Ranker::SortResults(double vector_rank_w, double anchor_rank_w, double page_rank_w) {
  for (auto& it : query_results_) {
    it.second.rank = vector_rank_w * it.second.vector_rank + 
                     anchor_rank_w * it.second.anchor_rank + 
                     page_rank_w   * it.second.page_rank;
    sorted_results_.push_back(it.first);
  }

  std::sort(sorted_results_.begin(), sorted_results_.end(), 
    [=](const unsigned int& id1, const unsigned int& id2) {
      const QueryResult& r1 = query_results_[id1];
      const QueryResult& r2 = query_results_[id2];

      return r1.rank > r2.rank;
    }
  );
}

std::vector<QueryResult> Ranker::GetPageOfResults(size_t page_num, size_t results_per_page) {
  std::vector<QueryResult> results;

  size_t first = results_per_page * (page_num - 1);
  size_t last = (first + results_per_page <= sorted_results_.size()) ? 
    first + results_per_page : sorted_results_.size();

  if (first > last) return results; 

  for (size_t i = first; i < last; ++i) {
    unsigned int doc_id = sorted_results_[i];
    QueryResult& result = query_results_[doc_id];
    result.url = doc_map_->GetDocById(doc_id).url;
    result.title = extractor_->GetTitle(doc_id);
    std::sort(result.positions.begin(), result.positions.end());
    result.short_text = extractor_->GetShortTextAt(doc_id, result.positions[0]);
    results.push_back(result);
  }  
  return results;
}

void Ranker::Search(
  const std::string& query, 
  double vector_rank_w, 
  double anchor_rank_w, 
  double page_rank_w
) {
  // Extract valid lexemes from the query string.
  std::vector<std::string> words = Lexicon::ExtractLexemes(query);

  query_results_.clear();
  sorted_results_.clear();
  query_lexeme_ids_.clear();
  for (auto lexeme : words) {
    unsigned int id;
    if ((id = lexicon_->GetLexemeId(lexeme)) != 0) {
      query_lexeme_ids_.push_back(id); 
    }
  }
  
  SortQueryTerms();

  // All documents will already be ranked by the vector model
  // in this step.
  ProcessQueryTerms();
  ProcessAnchorRank();

  // Process Page Rank.
  for (auto& it: query_results_)
    it.second.page_rank = doc_map_->GetDocById(it.first).page_rank;

  SortResults(vector_rank_w, anchor_rank_w, page_rank_w);
}
     
} // End of namespace.
