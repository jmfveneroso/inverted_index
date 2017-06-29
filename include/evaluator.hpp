#ifndef __EVALUATOR_HPP__
#define __EVALUATOR_HPP__

#include <algorithm>
#include <memory>
#include <chrono>
#include "ranker.hpp"
#include "doc_map.hpp"

namespace InvertedIndex {

struct QRels {
  unsigned int query_id;
  std::map<unsigned int, unsigned int> rel_docs;
  std::vector<unsigned int> results;
  unsigned int num_rel;
  unsigned int num_ret;
  unsigned int num_rel_ret;
  double map;  
  double idcg[5];  
  double ndcg[5];  
  double precision_at[10];  
  double iprec[11];  

  QRels() {}
  QRels(unsigned int query_id) : query_id(query_id) {}
  unsigned int GetDocScore(unsigned int doc_id) {
    if (rel_docs.find(doc_id) == rel_docs.end()) return 0;
    return rel_docs[doc_id];
  }
};

class Evaluator {
  std::shared_ptr<Ranker> ranker_;
  std::shared_ptr<DocMap> doc_map_;

  std::map<unsigned int, QRels> qrels_;
  QRels mean_qrels_;
 
  void LoadQrels(const std::string&);
  void LoadResults(const std::string&);
  void EvalQueryResults(QRels&);
  void PrintQrels(QRels&);

 public:
  Evaluator(std::shared_ptr<Ranker>, std::shared_ptr<DocMap>);

  void Eval(const std::string&, const std::string&);
};
     
} // End of namespace.

#endif
