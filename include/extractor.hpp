#ifndef __EXTRACTOR_HPP__
#define __EXTRACTOR_HPP__

#include "gumbo.h"
#include "logger.hpp"
#include "lexicon.h"
#include "doc_finder.h"
#include <memory>
#include <stdio.h>
#include <string>

namespace TP1 {

class Extractor {
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<ILexicon> lexicon_;
  std::shared_ptr<IDocFinder> doc_finder_;
  FILE* input_file_ = NULL;
  FILE* output_file_ = NULL;
  size_t num_tuples_ = 0;
  size_t num_documents_ = 0;
  size_t num_blocks_ = 0;
  size_t tuples_in_block_ = 0;
  Tuple tuple_block_[MAX_TUPLES];

  static std::string GetCleanText(GumboNode* node);
  void ExtractFromDoc(int, const std::string&);
  void FlushBlock();
  void WriteTuple(Tuple&);

 public:
  Extractor(
    std::shared_ptr<ILogger> logger,
    std::shared_ptr<ILexicon> lexicon,
    std::shared_ptr<IDocFinder> doc_finder
  );
 
  void Extract(const std::string&, const std::string&);
  void Print(const std::string&);
  static std::string GetCleanText(const string&);
};

} // End of namespace.

#endif
