/**
 * TupleSorter object
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __TUPLE_SORTER_H__
#define __TUPLE_SORTER_H__

#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <vector> 
#include <fstream>
#include <iostream>
#include <queue> 
#include <map>

#include "gumbo.h"

#include "env.h"
#include "tuple.h"
#include "lexicon.h"
#include "util.h"

namespace TP1 {

struct TupleBlock {
  int tuples_in_disk;
  int tuples_in_memory;
  fpos_t pos;
};
  
struct Anchor {
  std::string href;
  std::string text;
};
  
class ITupleSorter {
  virtual void FillQueue() = 0;

 public:
  ~ITupleSorter() {}
 
  virtual int get_num_tuples() = 0;
 
  virtual void ExtractTuplesFromDoc(int doc_id, const std::string& document) = 0;
  // virtual void ExtractTuplesFromAnchorText(const std::string& document) = 0;
  virtual void Sort(const std::string&) = 0;
  virtual MergeTuple PopTuple() = 0;
  virtual void Clear() = 0;
  virtual void SortBlocks(const std::string&) = 0;
};
  
class TupleSorter : public ITupleSorter {
  std::shared_ptr<ILexicon> lexicon_;
  TupleBlock* tuple_blocks;

  FILE* temp_file_;
  std::priority_queue< MergeTuple, std::vector<MergeTuple>, std::greater<MergeTuple> > tuple_queue_;

  Tuple *tuples_;

  int num_tuples_;
  int num_blocks_;

  void FillQueue();

 public:
  TupleSorter(std::shared_ptr<ILexicon> lexicon);
  ~TupleSorter() {}
 
  int get_num_tuples() { return num_tuples_; }
 
  static std::string CleanText(GumboNode* node);
  void ExtractTuplesFromDoc(int doc_id, const std::string& document);
  void Sort(const std::string&);
  MergeTuple PopTuple();
  void Clear();
  void SortBlocks(const std::string&);
};

} // Namespace TP1.

#endif
