/**
 * Extractor
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __EXTRACTOR_HPP__
#define __EXTRACTOR_HPP__

#include "gumbo.h"
#include "lexicon.h"
#include <stdio.h>
#include <string>

namespace TP1 {

struct Document {
  std::string url;
  std::string body;
};

struct HtmlElement {
  std::string name;
};

class Extractor {
  std::shared_ptr<ILexicon> lexicon_;
  FILE* input_file_ = NULL;
  FILE* output_file_ = NULL;
  size_t num_tuples_ = 0;
  size_t num_documents_ = 0;

  static std::string GetCleanText(GumboNode* node);
  void ExtractFromDoc(int, const std::string&);

 public:
  Extractor(std::shared_ptr<ILexicon>);
 
  void Extract(const std::string&, const std::string&);
  void Print(const std::string&);
  static std::string GetCleanText(const string&);
};

} // End of namespace.

#endif
