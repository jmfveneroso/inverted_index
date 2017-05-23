#ifndef __TUPLE_SORTER_HPP__
#define __TUPLE_SORTER_HPP__

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
#include "tuple.hpp"
#include "lexicon.hpp"

namespace InvertedIndex {

struct TupleBlock {
  size_t pos;
  off_t offset;
  size_t tuples_fetched_from_disk;
  size_t tuples_in_memory;

  bool is_output_block;
  size_t output_pos;

  TupleBlock(off_t offset) 
    : offset(offset), tuples_fetched_from_disk(0), 
      tuples_in_memory(0), is_output_block(false) {
  }
};

struct HoldingBlock {
  Tuple* tuples;
  size_t num_tuples;
};

struct MergeTuple : public Tuple {
  int block_num;
  
  MergeTuple() : Tuple(), block_num(0) {}
  MergeTuple(int block_num, Tuple tuple) 
    : Tuple(tuple), block_num(block_num) {}
};

struct PostingsList {
  unsigned int lexeme_id;
  std::map< unsigned int, std::vector<unsigned int> > word_offsets;
};

class ITupleSorter {
 public:
  ~ITupleSorter() {}
 
  virtual void Init(FILE*) = 0;
  virtual void WriteTuple(Tuple&) = 0;
  virtual void FlushHoldingBlock() = 0;
  virtual void Sort() = 0;
  virtual void LoadBlocks(size_t, off_t) = 0;
  virtual size_t GetNumBlocks() = 0;
};
  
class TupleSorter : public ITupleSorter {
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<ILexicon> lexicon_;
  FILE* output_file_;
  std::vector<TupleBlock> tuple_blocks_;
  HoldingBlock holding_block_;
  std::vector<size_t> block_map_;
  std::vector<TupleBlock> extra_blocks_;
  PostingsList output_postings_list_;
  off_t output_offset_;
  std::priority_queue< 
    MergeTuple, std::vector<MergeTuple>, std::greater<MergeTuple> 
  > tuple_heap_;

  void FillHeap();
  void WriteOutputTuple(MergeTuple&);
  TupleBlock* GetBlock(size_t);
  void ReadBlock(size_t);
  void CopyBlock(size_t, size_t);
  void FlushPostingsList();
  void FlushOutputBlock();
  void ReorderTupleBlocks();

 public:
  TupleSorter(std::shared_ptr<ILogger>, std::shared_ptr<ILexicon>);
 
  void Init(FILE*);
  void WriteTuple(Tuple&);
  void FlushHoldingBlock();
  void Sort();
  void LoadBlocks(size_t, off_t);
  size_t GetNumBlocks() { return tuple_blocks_.size(); }
};

} // End of namespace.

#endif
