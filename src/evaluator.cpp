#include "evaluator.hpp"
#include <fstream>
#include <sstream>

namespace InvertedIndex {

Evaluator::Evaluator(
  std::shared_ptr<Ranker> ranker,
  std::shared_ptr<DocMap> doc_map
) : ranker_(ranker), doc_map_(doc_map) {
}

// void Evaluator::LoadQrels(const std::string& qrels_file) {
//   std::ifstream infile(qrels_file);
// 
//   std::string line;
//   while (std::getline(infile, line)) {
//     size_t pos = line.find_first_of(",");
//     unsigned int query_id = std::atoi(line.substr(0, pos).c_str());
//     line = line.substr(pos + 1);
// 
//     std::string url;
//     if (line[0] == '"') {
//       pos = line.find_first_of("\"", 1);
//       url = line.substr(1, pos - 1);
//       line = line.substr(pos + 2);
//     } else {
//       pos = line.find_first_of(",");
//       url = line.substr(0, pos);
//       line = line.substr(pos + 1);
//     }
// 
//     unsigned int doc_id = doc_map_->GetDocId(url);
//     // std::cout << url << std::endl;
//     if (doc_id == 0) continue;
// 
//     if (qrels_.find(query_id) == qrels_.end())
//       qrels_.insert({ query_id, QRels(query_id) });
// 
//     unsigned int score = std::atoi(line.c_str());
//     qrels_[query_id].rel_docs[doc_id] = score;
// 
//     std::cout << query_id << " " << doc_id << " " << score << std::endl;
//   }
// }

void Evaluator::LoadQrels(const std::string& qrels_file) {
  std::ifstream infile(qrels_file);

  unsigned int query_id;
  unsigned int doc_id;
  unsigned int score;
  while (infile >> query_id) {
    infile >> doc_id;
    infile >> score;

    if (qrels_.find(query_id) == qrels_.end())
      qrels_.insert({ query_id, QRels(query_id) });
    qrels_[query_id].rel_docs[doc_id] = score;
  }

  for (auto& qrel : qrels_) {
    std::vector< std::pair<unsigned int, unsigned int> > rel_docs;
    for (auto& it : qrel.second.rel_docs) 
      rel_docs.push_back({ it.first, it.second });

    std::sort(rel_docs.begin(), rel_docs.end(), [](
      std::pair<unsigned int, unsigned int>& left,
      std::pair<unsigned int, unsigned int>& right
    ) {
      return left.second > right.second;
    });

    // Calculate ideal discounted cumulated gain.
    double cumulated_gain = 0;
    for (size_t i = 0; i < 5; ++i) {
      if (i < rel_docs.size()) {
        double discount = (i > 2) ? log2(i) : 1;
        cumulated_gain += rel_docs[i].second / discount;
      }
      qrel.second.idcg[i] = cumulated_gain;
    }

    // Find the number of relevant documents.
    qrel.second.num_rel = 0;
    for (auto& it : rel_docs) {
      if (it.second >= 3)
        ++qrel.second.num_rel;
    }
  }
}

void Evaluator::LoadResults(const std::string& results_file) {
  std::ifstream infile(results_file);

  std::string line;
  while (std::getline(infile, line)) {
    std::stringstream ss(line);
    unsigned int query_id;
    unsigned int doc_id;
    ss >> query_id;
    ss >> doc_id;
    ss >> doc_id;

    qrels_[query_id].results.push_back(doc_id);
  }
}

void Evaluator::EvalQueryResults(QRels& qrel) {
  // Calculate ndcg.
  double cumulated_gain = 0;
  for (size_t i = 0; i < 5; ++i) {
    if (i < qrel.results.size()) {
      double discount = (i > 2) ? log2(i) : 1;
      cumulated_gain += qrel.GetDocScore(qrel.results[i]) / discount;
    }
    qrel.ndcg[i] = cumulated_gain / qrel.idcg[i];
  }

  // Calculate precision at position.
  for (size_t i = 0; i < 10; ++i) qrel.precision_at[i] = 0;
  qrel.map = 0;

  unsigned int num_ret = 0;
  unsigned int num_rel_ret = 0;
  for (auto& result : qrel.results) {
    ++num_ret;
    unsigned int score = qrel.GetDocScore(result);
    if (score >= 3) {
      ++num_rel_ret;
      qrel.map += num_rel_ret / (double) num_ret;
    }
 
    double precision = num_rel_ret / (double) num_ret;
    switch (num_ret) {
      case 5: qrel.precision_at[0] = precision; break;
      case 10: qrel.precision_at[1] = precision; break;
      case 15: qrel.precision_at[2] = precision; break;
      case 20: qrel.precision_at[3] = precision; break;
      case 30: qrel.precision_at[4] = precision; break;
      case 50: qrel.precision_at[5] = precision; break;
      case 100: qrel.precision_at[6] = precision; break;
      case 200: qrel.precision_at[7] = precision; break;
      case 500: qrel.precision_at[8] = precision; break;
      case 1000: qrel.precision_at[9] = precision; break;
    }
  }

  // Mean average precision. 
  if (qrel.num_rel)
    qrel.map /= qrel.num_rel;

  qrel.num_ret = num_ret;
  qrel.num_rel_ret = num_rel_ret;

  // Calculate precision at recall.
  for (size_t i = 0; i < 11; ++i) qrel.iprec[i] = 0;

  double max_precision = 0;
  std::vector<unsigned int>::iterator it = qrel.results.end();
  --it;
  for (;; --it) {
    if (num_ret == 0) break;

    double precision = num_rel_ret / (double) num_ret;
    if (precision > max_precision) 
      max_precision = precision;
   
    if (qrel.num_rel != 0) {
      unsigned int recall_partition = 10 * (num_rel_ret / (double) qrel.num_rel);
      qrel.iprec[recall_partition] = max_precision;
    }

    unsigned int score = qrel.GetDocScore(*it);
    if (score >= 3) --num_rel_ret;
    --num_ret;
  }

  max_precision = qrel.iprec[10];
  for (size_t i = 9; ; --i) {
    if (qrel.iprec[i] > max_precision) max_precision = qrel.iprec[i];
    qrel.iprec[i] = max_precision;
    if (i == 0) break;
  }
}

void Evaluator::PrintQrels(QRels& q) {
  std::cout << "num_ret " << q.num_ret << std::endl;
  std::cout << "num_rel " << q.num_rel << std::endl;
  std::cout << "num_rel_ret " << q.num_rel_ret << std::endl;
  std::cout << "map " << q.map << std::endl;
  std::cout << "NDCG_5 " << q.ndcg[4] << std::endl;
  std::cout << "P_5 " << q.precision_at[0] << std::endl;
  std::cout << "P_10 " << q.precision_at[1] << std::endl;
  std::cout << "P_15 " << q.precision_at[2] << std::endl;
  std::cout << "P_20 " << q.precision_at[3] << std::endl;
  std::cout << "P_30 " << q.precision_at[4] << std::endl;
  std::cout << "P_50 " << q.precision_at[5] << std::endl;
  std::cout << "P_100 " << q.precision_at[6] << std::endl;
  std::cout << "P_200 " << q.precision_at[7] << std::endl;
  std::cout << "P_500 " << q.precision_at[8] << std::endl;
  std::cout << "P_1000 " << q.precision_at[9] << std::endl;
  std::cout << "iprec_at_recall_0.00 " << q.iprec[0] << std::endl;
  std::cout << "iprec_at_recall_0.10 " << q.iprec[1] << std::endl;
  std::cout << "iprec_at_recall_0.20 " << q.iprec[2] << std::endl;
  std::cout << "iprec_at_recall_0.30 " << q.iprec[3] << std::endl;
  std::cout << "iprec_at_recall_0.40 " << q.iprec[4] << std::endl;
  std::cout << "iprec_at_recall_0.50 " << q.iprec[5] << std::endl;
  std::cout << "iprec_at_recall_0.60 " << q.iprec[6] << std::endl;
  std::cout << "iprec_at_recall_0.70 " << q.iprec[7] << std::endl;
  std::cout << "iprec_at_recall_0.80 " << q.iprec[8] << std::endl;
  std::cout << "iprec_at_recall_0.90 " << q.iprec[9] << std::endl;
  std::cout << "iprec_at_recall_1.00 " << q.iprec[10] << std::endl;
}

void Evaluator::Eval(
  const std::string& qrels_file, const std::string& results_file
) {
  LoadQrels(qrels_file); 
  LoadResults(results_file);

  mean_qrels_.num_ret = 0;
  mean_qrels_.num_rel = 0;
  mean_qrels_.num_rel_ret = 0;
  mean_qrels_.map = 0;
  for (size_t i = 0; i < 5; ++i) mean_qrels_.ndcg[i] = 0;
  for (size_t i = 0; i < 10; ++i) mean_qrels_.precision_at[i] = 0;
  for (size_t i = 0; i < 11; ++i) mean_qrels_.iprec[i] = 0;

  for (auto& qrel : qrels_) {
    EvalQueryResults(qrel.second);
    std::cout << "Query " << qrel.first << std::endl;
    PrintQrels(qrel.second);

    mean_qrels_.num_ret += qrel.second.num_ret;
    mean_qrels_.num_rel += qrel.second.num_rel;
    mean_qrels_.num_rel_ret += qrel.second.num_rel_ret;
    mean_qrels_.map += qrel.second.map;
    for (size_t i = 0; i < 5; ++i) mean_qrels_.ndcg[i] += qrel.second.ndcg[i];
    for (size_t i = 0; i < 10; ++i) mean_qrels_.precision_at[i] += qrel.second.precision_at[i];
    for (size_t i = 0; i < 11; ++i) mean_qrels_.iprec[i] += qrel.second.iprec[i];
  }

  unsigned int num_queries = qrels_.size();
  mean_qrels_.map /= num_queries;
  for (size_t i = 0; i < 5; ++i) mean_qrels_.ndcg[i] /= num_queries;
  for (size_t i = 0; i < 10; ++i) mean_qrels_.precision_at[i] /= num_queries;
  for (size_t i = 0; i < 11; ++i) mean_qrels_.iprec[i] /= num_queries;

  std::cout << "Query all" << std::endl;
  PrintQrels(mean_qrels_);
}
     
} // End of namespace.
