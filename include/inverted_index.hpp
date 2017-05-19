#ifndef __INVERTED_INDEX_HPP__
#define __INVERTED_INDEX_HPP__

#include <memory>
#include <stdio.h>
#include <string>
#include "logger.hpp"
#include "extractor.hpp"
#include "doc_collection.hpp"
#include "tuple_sorter.h"

namespace TP1 {

struct InvertedIndexHeader {
  size_t num_blocks; 
  size_t num_lexemes; 
  size_t num_docs; 
  size_t anchor_index_offset; 
  size_t doc_map_offset;
  size_t lexicon_offset;
};

class IInvertedIndex {
 public:
  ~IInvertedIndex() {}
 
  virtual PostingsList GetPostingsList(const std::string&) = 0;
  virtual void CreateIndexForCollection(
    const std::string&, const std::string&
  ) = 0;
  virtual void Load(const std::string&) = 0;
  virtual void Sort(const std::string&) = 0;
  virtual void CreateAnchorIndex(const std::string&, const std::string&) = 0;
  virtual void Extract(const std::string&, const std::string&) = 0;
};

class InvertedIndex : public IInvertedIndex {
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<IDocCollection> doc_collection_;
  std::shared_ptr<DocMap> doc_map_;
  std::shared_ptr<ILexicon> lexicon_;
  std::shared_ptr<Extractor> extractor_;
  std::shared_ptr<ITupleSorter> tuple_sorter_;
  InvertedIndexHeader header_;
  FILE* output_file_;
  off_t write_offset_ = 0;

  void ReadHeader();
  void WriteHeader();
  void RunExtractor();
  void CalculateVectorNorms();
  void WriteAnchorFile();

 public:
  InvertedIndex(
    std::shared_ptr<ILogger>, 
    std::shared_ptr<IDocCollection>,
    std::shared_ptr<DocMap>,
    std::shared_ptr<ILexicon>,
    std::shared_ptr<Extractor>,
    std::shared_ptr<ITupleSorter>
  );
 
  PostingsList GetPostingsList(unsigned int);
  PostingsList GetPostingsList(const std::string&);
  void CreateIndexForCollection(
    const std::string&, const std::string&
  );
  void Sort(const std::string&);
  void Load(const std::string&);
  void CreateAnchorIndex(const std::string&, const std::string&);
  void Extract(const std::string&, const std::string&);
};

} // End of namespace.

#endif
