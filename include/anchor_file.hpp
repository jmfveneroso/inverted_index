#ifndef __ANCHOR_FILE_HPP__
#define __ANCHOR_FILE_HPP__

#include "logger.hpp"
#include "lexicon.h"
#include "doc_map.hpp"
#include <memory>
#include <cstdio>
#include <cstring>
#include <vector>

namespace TP1 {

struct Hyperlink {
  std::string href;
  std::string anchor_text;
  Hyperlink() {}
  Hyperlink(std::string href, std::string anchor_text) 
    : href(href), anchor_text(anchor_text) {
  }
};

class AnchorFile {
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<DocMap> doc_map_;
  std::shared_ptr<ILexicon> lexicon_;
  FILE* file_;
  size_t num_hyperlinks_ = 0;

  std::string TruncateUrl(std::string);
  std::string GetRootUrl(std::string);
  std::string NormalizeHyperlink(unsigned int, std::string&);
  void WriteString(const std::string&);
  std::string ReadString();

 public:
  AnchorFile(
    std::shared_ptr<ILogger>,
    std::shared_ptr<DocMap>,
    std::shared_ptr<ILexicon>
  );
 
  void Init();
  void WriteHyperlinks(const std::vector<Hyperlink>&);
  void ReadHyperlinks();
  void Close();
  size_t GetNumHyperlinks() { return num_hyperlinks_; }
};

} // End of namespace.

#endif
