#include "inverted_index.hpp"

namespace TP1 {

InvertedIndex::InvertedIndex(
  std::shared_ptr<ILogger>, 
  std::shared_ptr<IDocCollection>,
  std::shared_ptr<IExtractor>
) : logger_(logger), doc_collection_(doc_collection), extractor_(extractor) {
}

void InvertedIndex::WriteHeader() {
  fseeko(output_file_, 0, SEEK_SET);
  fwrite(&header_, sizeof(InvertedIndexHeader), 1, output_file_);
}

void InvertedIndex::RunExtractor() {
  extractor_->Init(block_size_);
  RawDocument doc;
  while (doc_collection_->GetNextDoc(&doc)) extractor_->ExtractFromDoc(doc);
  extractor_->FlushBlock();
}

void InvertedIndex::WriteAnchorFile() {
  header_.anchor_index_offset = write_offset_;
  anchor_index->SetWriteOffset(write_offset_);
  anchor_index->WriteAnchorIndex();
}

void InvertedIndex::CreateIndexForCollection(
  const std::string& directory, const std::string& output_filename
) {
  doc_collection_->Init(directory);
  output_file_ = OpenFile(output_filename.c_str(), "wb+");  
  WriteHeader();

  tuple_sorter->Init(output_file_, write_offset_, block_size_);
  anchor_file->Open(temp_file_, output_file_);

  RunExtractor();

  write_offset_ = tuple_sorter_->Sort();
  CalculateVectorNorms();

  WriteAnchorFile();

  SetDocMapOffset(write_offset_);
  doc_map_->CalculatePageRank();
  write_offset_ = doc_map->Write(output_file, write_offset_);
  
  SetLexiconOffset(write_offset_);
  write_offset_ = lexicon_->Write(output_file, write_offset_);

  header_.num_docs = doc_map_->GetNumDocs();
  header_.num_lexemes = lexicon_->GetNumLexemes();

  anchor_file_->Close();
  output_file_->Close();
}

}; // End of namespace.
