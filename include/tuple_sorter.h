#ifndef __TUPLE_SORTER_H__
#define __TUPLE_SORTER_H__

#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <vector> 
#include <iostream>
#include <queue>
#include <map>
#include <memory>
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
  
struct MergeTuple : public Tuple {
  int block_num;
  
  MergeTuple() : Tuple(), block_num(0) {}
  MergeTuple(int block_num, Tuple tuple) 
    : Tuple(tuple), block_num(block_num) {}
};

class ITupleSorter {
  virtual void FillQueue() = 0;

 public:
  ~ITupleSorter() {}
 
  virtual int get_num_tuples() = 0;
  virtual void Sort(const std::string&) = 0;
  virtual MergeTuple PopTuple() = 0;
  virtual void Clear() = 0;
};
  
class TupleSorter : public ITupleSorter {
  std::shared_ptr<ILexicon> lexicon_;
  TupleBlock* tuple_blocks;

  FILE* temp_file_;
  std::priority_queue< 
    MergeTuple, std::vector<MergeTuple>, std::greater<MergeTuple> 
  > tuple_queue_;

  Tuple *tuples_;

  int num_tuples_;
  int num_blocks_;

  void FillQueue();

 public:
  TupleSorter(std::shared_ptr<ILexicon> lexicon);
  ~TupleSorter() {}
 
  int get_num_tuples() { return num_tuples_; }
  void Sort(const std::string&);
  MergeTuple PopTuple();
  void Clear();
};

} // End of namespace.

#endif
