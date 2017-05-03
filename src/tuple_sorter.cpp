/**
 * TupleSorter object
 * @author: jmfveneroso@gmail.com.br
 */

#include "tuple_sorter.h"

using namespace std;
using namespace TP1;

TupleSorter::TupleSorter (std::shared_ptr<ILexicon> lexicon) 
  : lexicon_(lexicon), num_tuples_(0) {
  temp_file_ = tmpfile();
}

/**
 * Read tuples from sorted blocks in disk filling a priority queue.
 *
 */
void TupleSorter::FillQueue () {
  int blocks_to_be_read = MAX_TUPLES / num_blocks_;
  blocks_to_be_read = (blocks_to_be_read != 0) ? blocks_to_be_read : 1;

  for (int i = 0; i < num_blocks_; i++) {
    if (tuple_blocks[i].tuples_in_memory == 0 && tuple_blocks[i].tuples_in_disk > 0) {
      fsetpos(temp_file_, &tuple_blocks[i].pos);
      
      int num_to_read = (tuple_blocks[i].tuples_in_disk > blocks_to_be_read) ? blocks_to_be_read : tuple_blocks[i].tuples_in_disk;
      
      int actually_read = fread(tuples_, sizeof(Tuple), num_to_read, temp_file_);
      
      tuple_blocks[i].tuples_in_disk -= actually_read;
      tuple_blocks[i].tuples_in_memory += actually_read;
      
      fgetpos(temp_file_, &tuple_blocks[i].pos);

      for (int j = 0; j < actually_read; j++) {
        if (tuples_[j].lexeme_id != 0 || tuples_[j].document_id != 0 || tuples_[j].word_position != 0) {
          tuple_queue_.push(MergeTuple(i, tuples_[j]));
        } else {
          tuple_blocks[i].tuples_in_memory--;
        }
      }
    }
  }
}

void TupleSorter::Sort(const std::string& filename) {
  temp_file_ = fopen(filename.c_str(), "rb+");

  num_tuples_ = 0;
  Tuple t;
  while (fread(&t, sizeof(Tuple), 1, temp_file_) != 0) { if (t.lexeme_id != 0) ++num_tuples_; }

  num_blocks_ = (num_tuples_ / MAX_TUPLES) + ((num_tuples_ % MAX_TUPLES) ? 1 : 0);
  if (num_blocks_ == 0) return;
  
  tuple_blocks = new TupleBlock[num_blocks_];
  
  fseeko(temp_file_, 0, SEEK_SET);
  
  for (int i = 0; i < num_blocks_; i++) {
    tuple_blocks[i].tuples_in_disk = MAX_TUPLES;
    tuple_blocks[i].tuples_in_memory = 0;
    
    fgetpos(temp_file_, &tuple_blocks[i].pos);
    fseek(temp_file_, MAX_TUPLES * sizeof(Tuple), SEEK_CUR);
  }
  
  int blocks_to_be_read = MAX_TUPLES / num_blocks_;
  blocks_to_be_read = (blocks_to_be_read != 0) ? blocks_to_be_read : 1;
  tuples_ = new Tuple[blocks_to_be_read];
  
  FillQueue();
}

/**
 * Pop the smallest tuple.
 *
 */
MergeTuple TupleSorter::PopTuple () {
  MergeTuple tuple = tuple_queue_.top();
  tuple_blocks[tuple.block_num].tuples_in_memory--;
  tuple_queue_.pop();
  FillQueue();
  return tuple;
}

void TupleSorter::Clear () {
  delete tuples_;
}
