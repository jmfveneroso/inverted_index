#ifndef __DOC_COLLECTION_HPP__
#define __DOC_COLLECTION_HPP__

#include <memory>
#include <stdio.h>
#include <string>
#include "logger.hpp"

namespace TP1 {

struct RawDocument {
  std::string url;
  size_t file_number;
  size_t offset;
  std::string content;
};

class IDocCollection {
 public:
  ~IDocCollection() {}
 
  virtual void Init(const std::string&) = 0;
  virtual bool GetNextDoc(RawDocument*) = 0;
};

class DocCollection : public IDocCollection {
  std::shared_ptr<ILogger> logger_;
  FILE* file_ = NULL;
  unsigned int file_counter_ = 0;
  unsigned int max_counter_ = 2;
  std::string directory_;
  size_t offset_ = 0;

  bool OpenNextFile();
  void TrimUrl(std::string&);

 public:
  DocCollection(std::shared_ptr<ILogger>);
 
  void Init(const std::string&);
  bool GetNextDoc(RawDocument*);
};

} // End of namespace.

#endif
