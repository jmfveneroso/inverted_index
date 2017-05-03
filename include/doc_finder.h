#ifndef __DOC_FINDER_H__
#define __DOC_FINDER_H__

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include "inverted_file_reader.h"

namespace TP1 {

struct Document {
  std::string url;
  size_t offset;
  double vector_norm;

  Document() {}
  Document(std::string url, size_t offset, double vector_norm)
    : url(url), offset(offset), vector_norm(vector_norm) {
  }
};

class IDocFinder {
 public:
  ~IDocFinder() {}
 
  virtual void InsertDocument(Document) = 0;
  virtual void LoadFromFile(const std::string&) = 0;
  virtual std::vector<int> BooleanQuery(const std::string& query) = 0;
  virtual std::string GetUrl(int) = 0;
  virtual void PrintDoc(int) = 0;
};

class DocFinder : public IDocFinder {
  std::shared_ptr<IInvertedFileReader> inverted_file_reader_;
  map<int, Document> url_map_;
  FILE* file_;
  size_t num_docs_ = 0;

  static std::vector<int> UniqueDocs(std::vector<int> v);
  static std::vector<int> OperatorAnd(std::vector<int> v1, std::vector<int> v2);
  static std::vector<int> OperatorOr(std::vector<int> v1, std::vector<int> v2);

 public:
  DocFinder(std::shared_ptr<IInvertedFileReader> inverted_file_reader);
  ~DocFinder();

  void InsertDocument(Document);
  void LoadFromFile(const std::string&);
  std::vector<int> BooleanQuery(const std::string& query);
  std::string GetUrl(int);
  void PrintDoc(int);
};
     
} // End of namespace.

#endif
