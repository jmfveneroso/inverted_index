#ifndef __DOC_MAP_HPP__
#define __DOC_MAP_HPP__

#include "logger.hpp"
#include "lexicon.h"
#include "doc_collection.hpp"
#include <memory>
#include <stdio.h>
#include <string>

namespace TP1 {

struct Document {
  std::string url;
  size_t file_num;
  off_t offset;
  double page_rank;
  double next_rank;
  double vector_norm;
  std::vector<unsigned int> outbound_links;

  Document() {}
  Document(std::string url, size_t file_num, off_t offset) 
    : url(url), file_num(file_num), offset(offset), page_rank(1), 
      next_rank(0) {}
};

class DocMap {
  std::shared_ptr<ILogger> logger_;
  std::map<std::string, unsigned int> id_map_;
  std::map<unsigned int, Document> doc_map_;
  unsigned int id_counter_ = 0;

  std::string TruncateUrl(std::string);

 public:
  DocMap(std::shared_ptr<ILogger> logger);
 
  unsigned int GetDocId(const std::string&);
  unsigned int AddDoc(Document&);
  Document GetDocById(unsigned int);
  size_t GetNumDocs() { return doc_map_.size(); }
  void AddToVectorNorm(unsigned int, double);
  void SqrtVectorNorms();
  void AddOutboundLink(unsigned int, unsigned int);
  void CalculatePageRank();
  void Write(FILE*, off_t);
  void Load(FILE*, off_t, size_t);
};

} // End of namespace.

#endif
