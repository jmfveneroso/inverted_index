#ifndef __EXTRACTOR_HPP__
#define __EXTRACTOR_HPP__

#include "gumbo.h"
#include "logger.hpp"
#include "lexicon.h"
#include "doc_map.hpp"
#include "doc_collection.hpp"
#include "tuple_sorter.h"
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

namespace TP1 {

class Extractor {
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<ILexicon> lexicon_;
  std::shared_ptr<DocMap> doc_map_;
  std::shared_ptr<ITupleSorter> tuple_sorter_;
  std::shared_ptr<IDocCollection> doc_collection_;
  std::vector<std::string> lexemes_;

  FILE* input_file_ = NULL;
  FILE* output_file_ = NULL;
  size_t num_tuples_ = 0;
  size_t num_documents_ = 0;
  size_t num_blocks_ = 0;
  size_t block_size_ = 0;
  size_t tuples_in_block_ = 0;
  unsigned int current_doc_id_ = 0;
  Tuple tuple_block_[MAX_TUPLES];

  std::string GetCleanText(GumboNode* node);
  void Parse(GumboNode* node, bool get_links = false);
  void ExtractFromDoc(int, const std::string&);
  void ExtractLexemes(std::string);
  char GetValidCharacter(int);
  std::string TruncateUrl(std::string);
  std::string GetRootUrl(std::string);
  std::string NormalizeHyperlink(unsigned int, std::string&);

 public:
  Extractor(
    std::shared_ptr<ILogger> logger,
    std::shared_ptr<ILexicon> lexicon,
    std::shared_ptr<DocMap> doc_map,
    std::shared_ptr<ITupleSorter> tuple_sorter,
    std::shared_ptr<IDocCollection> doc_collection
  );
 
  void ExtractLinks(RawDocument&);
  void ExtractFromDoc(RawDocument&);
  void Extract(const std::string&, const std::string&);
  void PrintLexemes(RawDocument&);
  void Print(const std::string&);
  void Parse(const std::string&, bool get_links = false);
  void ReadDoc(unsigned int);
};

} // End of namespace.

#endif
