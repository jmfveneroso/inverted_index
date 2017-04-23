/**
 * SearchMachine object
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __DOC_FINDER_H__
#define __DOC_FINDER_H__

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include "inverted_file_reader.h"

namespace TP1 {

class DocFinder {
  std::shared_ptr<IInvertedFileReader> inverted_file_reader_;
  map<int, string> url_map;
  FILE* file_;

 public:
  DocFinder(std::shared_ptr<IInvertedFileReader> inverted_file_reader);
  ~DocFinder();

  void LoadFromFile(const std::string&);
  static std::vector<int> UniqueDocs(std::vector<int> v);
  static std::vector<int> OperatorAnd(std::vector<int> v1, std::vector<int> v2);
  static std::vector<int> OperatorOr(std::vector<int> v1, std::vector<int> v2);

  std::vector<int> BooleanQuery(const std::string& query);
  void PrintDoc(int);
  std::string GetUrl(int);
};
     
} // Namespace TP1.

#endif
