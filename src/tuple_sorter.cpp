#include "tuple_sorter.hpp"
#include "bit_buffer.hpp"
#include <sstream>

using namespace std;

namespace InvertedIndex {

TupleSorter::TupleSorter(
  std::shared_ptr<ILogger> logger,
  std::shared_ptr<ILexicon> lexicon
) : logger_(logger), lexicon_(lexicon) {
}

void TupleSorter::Init(FILE* output_file) {
  output_file_ = output_file;
  holding_block_.tuples = new Tuple[MAX_TUPLES];
}

void TupleSorter::FlushHoldingBlock() {
  if (holding_block_.num_tuples == 0) return;

  sort(
    holding_block_.tuples, 
    holding_block_.tuples + holding_block_.num_tuples, 
    std::less<Tuple>()
  );

  off_t offset = ftello(output_file_);
  TupleBlock new_tuple_block(offset);

  for (size_t i = 0; i < holding_block_.num_tuples; ++i) {
    fwrite(&holding_block_.tuples[i], sizeof(Tuple), 1, output_file_);
  }
  
  // Write the padding if the block was not completely filled. This
  // will only be called for the last block.
  for (size_t i = holding_block_.num_tuples; i < MAX_TUPLES; ++i) {
    static Tuple t(0, 0, 0);
    fwrite(&t, sizeof(Tuple), 1, output_file_);
  }

  new_tuple_block.pos = tuple_blocks_.size();
  tuple_blocks_.push_back(new_tuple_block);

  std::stringstream ss;
  ss << "A sorted block was written with " << holding_block_.num_tuples;
  ss << " tuples and " << MAX_TUPLES - holding_block_.num_tuples << " padding. ";
  ss << "Blocks written: " << tuple_blocks_.size();
  ss << ". Total number of tuples: " << MAX_TUPLES * (tuple_blocks_.size() - 1) + holding_block_.num_tuples;
  ss << ". Tuples per block: " << MAX_TUPLES << ".";
  logger_->Log(ss.str());
  holding_block_.num_tuples = 0;
}

void TupleSorter::WriteTuple(Tuple& tuple) {
  if (holding_block_.num_tuples == MAX_TUPLES) {
    FlushHoldingBlock();
  }
  holding_block_.tuples[holding_block_.num_tuples++] = tuple;
}

void TupleSorter::FlushOutputBlock() {
  if (holding_block_.num_tuples == 0) return;

  for (auto& block : tuple_blocks_) {
    if (block.is_output_block) continue;
    if (block.tuples_fetched_from_disk < MAX_TUPLES) continue;

    block.is_output_block = true;
    fseeko(output_file_, block.offset, SEEK_SET);
    for (size_t i = 0; i < holding_block_.num_tuples; ++i) {
      fwrite(&holding_block_.tuples[i], sizeof(Tuple), 1, output_file_);
    }

    block.output_pos = block_map_.size();
    block_map_.push_back(block.pos);
    holding_block_.num_tuples = 0;

    std::stringstream ss;
    ss << "Output block " << block_map_.size() << " was written to ";
    ss << block.pos << ".";
    logger_->Log(ss.str());
    return;
  }

  // Create extra block.
  fseeko(output_file_, 0, SEEK_END);
  off_t offset = ftello(output_file_);
  for (size_t i = 0; i < holding_block_.num_tuples; ++i)
    fwrite(&holding_block_.tuples[i], sizeof(Tuple), 1, output_file_);

  TupleBlock extra_block(offset);
  extra_block.output_pos = block_map_.size();
  block_map_.push_back(tuple_blocks_.size() + extra_blocks_.size());
  extra_blocks_.push_back(extra_block);

  holding_block_.num_tuples = 0;

  std::stringstream ss;
  ss << "Output block " << block_map_.size() << " was written to an extra block. There are ";
  ss << extra_blocks_.size() << " extra blocks in total.";
  logger_->Log(ss.str());
}

void TupleSorter::WriteOutputTuple(MergeTuple& tuple) {
  if (holding_block_.num_tuples == MAX_TUPLES) FlushOutputBlock();
  holding_block_.tuples[holding_block_.num_tuples++] = tuple;
}

void TupleSorter::FillHeap() {
  // The maximum number of tuple to read from each block at the same time.
  size_t batch_size = MAX_TUPLES / tuple_blocks_.size();
  for (auto& block : tuple_blocks_) {
    if (block.tuples_in_memory > 0) continue;
    if (block.tuples_fetched_from_disk == MAX_TUPLES) continue;

    // The block holds less tuples than the batch size.
    if (block.tuples_fetched_from_disk + batch_size > MAX_TUPLES)
      batch_size = MAX_TUPLES - block.tuples_fetched_from_disk;

    fseeko(output_file_, block.offset + block.tuples_fetched_from_disk * sizeof(Tuple), SEEK_SET);
    for (size_t i = 0; i < batch_size; ++i) {
      Tuple tuple;
      size_t num_read = 0;
      if ((num_read = fread(&tuple, sizeof(Tuple), 1, output_file_)) != 1) {
        logger_->Log("Block num: " + std::to_string(block.pos));
        logger_->Log("Block offset: " + std::to_string(block.offset));
        logger_->Log("Block from disk: " + std::to_string(block.tuples_fetched_from_disk));
        logger_->Log("Batch size: " + std::to_string(batch_size));
        logger_->Log("File pos: " + std::to_string(ftell(output_file_)));
        logger_->Log("Num read: " + std::to_string(num_read));
        throw new std::runtime_error("Error reading tuple while sorting.");
      }

      block.tuples_fetched_from_disk++;
      block.tuples_in_memory++;

      tuple_heap_.push(MergeTuple(block.pos, tuple));
    }

    std::stringstream ss;
    ss << batch_size << " tuples read from block " << block.pos;
    ss << ". Tuples in memory: " << block.tuples_in_memory;
    ss << ", tuples fetched from disk: " << block.tuples_fetched_from_disk;
    logger_->Log(ss.str());
  }
}

TupleBlock* TupleSorter::GetBlock(size_t i) {
  if (i > tuple_blocks_.size() + extra_blocks_.size() - 1)
    throw new std::runtime_error(
      "Invalid block position " + std::to_string(i) + "."
    );

  if (i < tuple_blocks_.size())
    return  &tuple_blocks_[i];
  else 
    return &extra_blocks_[i - tuple_blocks_.size()];
}

void TupleSorter::ReadBlock(size_t i) {
  TupleBlock *block = GetBlock(i);
  fseeko(output_file_, block->offset, SEEK_SET);

  if (fread(holding_block_.tuples, sizeof(Tuple), MAX_TUPLES, output_file_) != MAX_TUPLES)
    throw new std::runtime_error("Error reading tuple while sorting.");

  std::stringstream ss;
  ss << "Block " << i << " was transfered to memory.";
  logger_->Log(ss.str());
}

void TupleSorter::CopyBlock(size_t i, size_t j) {
  if (i == j) return;

  if (i > tuple_blocks_.size())
    throw new std::runtime_error(
      "Invalid block position " + std::to_string(i) + " at copy operation."
    );

  TupleBlock* dest_block = GetBlock(j);
  static Tuple tuples[10000];

  off_t read_offset = tuple_blocks_[i].offset;
  off_t write_offset = dest_block->offset;

  size_t transfered_tuples = 0;
  while (transfered_tuples < MAX_TUPLES) {
    // Copy in batches of 10000 tuples.
    size_t batch_size = 10000;
    if (MAX_TUPLES - transfered_tuples < 10000)
      batch_size = MAX_TUPLES - transfered_tuples;

    fseeko(output_file_, read_offset, SEEK_SET);
    size_t num_read = fread(tuples, sizeof(Tuple), batch_size, output_file_);
    if (num_read != batch_size) 
      throw new std::runtime_error(
        "Error reading block " + std::to_string(i) + " during copy operation."
      );
    read_offset = ftello(output_file_);
  
    fseeko(output_file_, write_offset, SEEK_SET);
    size_t num_written = fwrite(tuples, sizeof(Tuple), batch_size, output_file_);
    if (num_written != batch_size) 
      throw new std::runtime_error(
        "Error writing block " + std::to_string(j) + " during copy operation."
      );
    write_offset = ftello(output_file_);
    transfered_tuples += num_written;
  }

  std::stringstream ss;
  ss << "Block " << i << " was copied to position " << j << ".";
  logger_->Log(ss.str());
}

void TupleSorter::FlushPostingsList() { 
  if (output_postings_list_.lexeme_id == 0) return;

  fseeko(output_file_, output_offset_, SEEK_SET);
  lexicon_->SetOffset(output_postings_list_.lexeme_id, output_offset_);
  lexicon_->SetDocFrequency(output_postings_list_.lexeme_id, output_postings_list_.word_offsets.size());

  BitBuffer bit_buffer;
  unsigned int last_doc_id = 0;
  for (auto it : output_postings_list_.word_offsets) {
    unsigned int doc_id = it.first;
    unsigned int d_gap = doc_id - last_doc_id;
    if (d_gap == 0) {
      logger_->Log("Duplicate d_gap in document " + std::to_string(doc_id));
      // throw new std::runtime_error("Invalid dgap.");
      continue;
    }
    bit_buffer.WriteInt(d_gap);

    // Term frequency.
    bit_buffer.WriteInt(it.second.size());

    unsigned int last_w_off = 0;
    for (auto w_off : it.second) {    
      unsigned int w_gap = w_off - last_w_off;
      if (w_gap == 0) {
        logger_->Log("Duplicate w_gap in document " + std::to_string(doc_id));
        // throw new std::runtime_error("Invalid wgap.");
        continue;
      }
      bit_buffer.WriteInt(w_gap);
      last_w_off = w_off;
    }
    last_doc_id = doc_id;
  }

  for (size_t i = 0; i < bit_buffer.Size(); ++i) {
    unsigned char byte = bit_buffer.ReadByte(i);
    size_t bytes_written = fwrite(&byte, sizeof(unsigned char), 1, output_file_);
    if (bytes_written != 1) throw new std::runtime_error("Error flushing posting list.");
  }
  output_offset_ = ftello(output_file_);

  static int counter = 0;
  if (counter++ % 1000 == 0) {
    std::stringstream ss;
    ss << counter << " postings lists flushed, current -> lexeme_id: " << output_postings_list_.lexeme_id;
    ss << " documents: " << output_postings_list_.word_offsets.size();
    ss << " byte size: " << bit_buffer.Size();
    logger_->Log(ss.str());
  }

  output_postings_list_.lexeme_id = 0;
  output_postings_list_.word_offsets.clear();
}

void TupleSorter::ReorderTupleBlocks() {
  output_offset_ = tuple_blocks_[0].offset;

  for (size_t i = 0; i < block_map_.size(); ++i) {
    // Read into memory the block that should be at position i.
    ReadBlock(block_map_[i]);

    // If this block is not an output block then all data can discarded. Else
    // it needs to be copied to another vacant block.
    if (tuple_blocks_[i].is_output_block) {
      // Copy the block that is at position i to the place left vacant by
      // the previos block.
      CopyBlock(i, block_map_[i]);

      // Update the block_map with the new position of block i.
      block_map_[tuple_blocks_[i].output_pos] = block_map_[i];
    } else {
      std::stringstream ss;
      ss << "Block " << i << " is being discarded since it is not an output block.";
      logger_->Log(ss.str());
    }

    // Write the sorted tuples at their final location.
    for (size_t j = 0; j < MAX_TUPLES; ++j) {
      // This is padding.
      Tuple& tuple = holding_block_.tuples[j];
      if (tuple.lexeme_id == 0) break;

      if (tuple.lexeme_id != output_postings_list_.lexeme_id)
        FlushPostingsList();

      output_postings_list_.lexeme_id = tuple.lexeme_id;
      output_postings_list_.word_offsets[tuple.document_id]
        .push_back(tuple.word_position);
    }
    std::stringstream ss;
    ss << "Block " << i << " has been compressed and written to its final position.";
    logger_->Log(ss.str());
  }
  FlushPostingsList();
}

void TupleSorter::LoadBlocks(size_t num_blocks, off_t offset) {
  for (size_t i = 0; i < num_blocks; ++i) {
    TupleBlock new_tuple_block(offset + i * MAX_TUPLES * sizeof(Tuple));
    new_tuple_block.pos = tuple_blocks_.size();
    tuple_blocks_.push_back(new_tuple_block);
  }
}

void TupleSorter::Sort() {
  FillHeap();
  size_t counter = 0;
  while (!tuple_heap_.empty()) {
    MergeTuple tuple = tuple_heap_.top();
    tuple_blocks_[tuple.block_num].tuples_in_memory--;
    tuple_heap_.pop();
    WriteOutputTuple(tuple);
    FillHeap();
    if (++counter % 1000000 == 0) 
      logger_->Log(std::to_string(counter) + " tuples sorted.");
  }
  FlushOutputBlock();
  ReorderTupleBlocks();
  logger_->Log("Finished sorting " + std::to_string(counter) + " tuples.");
  delete[] holding_block_.tuples;
}

} // End of namespace.
