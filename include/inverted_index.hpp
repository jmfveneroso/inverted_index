#ifndef __INVERTED_INDEX_HPP__
#define __INVERTED_INDEX_HPP__

#include <memory>
#include <stdio.h>
#include <string>
#include "logger.hpp"

namespace TP1 {

struct InvertedIndexHeader {
  size_t fingerprint; 
  size_t num_lexemes; 
  size_t num_docs; 
  size_t anchor_index_offset; 
  size_t doc_map_offset;
  size_t lexicon_offset;
};

class IInvertedIndex {
 public:
  ~IInvertedIndex() {}
 
  virtual void CreateIndexForCollection(
    const std::string&, const std::string&
  ) = 0;
};

class InvertedIndex : public IInvertedIndex {
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<IExtractor> doc_collection_;
  std::shared_ptr<IDocCollection> doc_collection_;
  InvertedIndexHeader header_;

 public:
  InvertedIndex(
    std::shared_ptr<ILogger>, 
    std::shared_ptr<IDocCollection>,
    std::shared_ptr<IExtractor>
  );
 
  void CreateIndexForCollection(
    const std::string&, const std::string&
  );
};

} // End of namespace.

#endif
