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

struct CharBuffer {
  char buffer[10000];
  size_t cursor = 0;
  size_t num_read = 0;
};

class IDocCollection {
 public:
  ~IDocCollection() {}
 
  virtual void Init(const std::string&) = 0;
  virtual bool GetNextDoc(RawDocument*) = 0;
  virtual RawDocument Read(size_t, size_t) = 0;
  virtual void Rewind() = 0;
};

class DocCollection : public IDocCollection {
  std::shared_ptr<ILogger> logger_;
  FILE* file_ = NULL;
  unsigned int file_counter_ = 0;
  unsigned int max_counter_ = 1353;
  unsigned int start_counter_ = 0;
  std::string directory_;
  size_t offset_ = 0;
  CharBuffer char_buffer_;

  bool OpenNextFile();
  bool ReadBit(char*);

 public:
  DocCollection(std::shared_ptr<ILogger>);
 
  void Init(const std::string&);
  bool GetNextDoc(RawDocument*);
  RawDocument Read(size_t, size_t);
  void Rewind();
  static void TrimUrl(std::string&);
};

} // End of namespace.

#endif
