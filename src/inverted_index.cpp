#include "inverted_index.hpp"
#include "bit_buffer.hpp"
#include <sstream>
#include <unistd.h>
#include <sys/types.h>

namespace InvertedIndex {

InvertedIndex::InvertedIndex(
  std::shared_ptr<ILogger> logger, 
  std::shared_ptr<IDocCollection> doc_collection,
  std::shared_ptr<DocMap> doc_map,
  std::shared_ptr<ILexicon> lexicon,
  std::shared_ptr<Extractor> extractor,
  std::shared_ptr<ITupleSorter> tuple_sorter
) : logger_(logger), doc_collection_(doc_collection), doc_map_(doc_map),
    lexicon_(lexicon), extractor_(extractor),
    tuple_sorter_(tuple_sorter) {
}

void InvertedIndex::ReadHeader() {
  fseeko(output_file_, 0, SEEK_SET);
  if (fread(&header_, sizeof(InvertedIndexHeader), 1, output_file_) != 1)
    throw new std::runtime_error("Error reading header.");
}

void InvertedIndex::WriteHeader() {
  fseeko(output_file_, 0, SEEK_SET);
  if (fwrite(&header_, sizeof(InvertedIndexHeader), 1, output_file_) != 1)
    throw new std::runtime_error("Error writing header.");
}

void InvertedIndex::RunExtractor() {
  RawDocument doc;
  while (doc_collection_->GetNextDoc(&doc)) {
    extractor_->ExtractFromDoc(doc);

    size_t num_docs = doc_map_->GetNumDocs();
    if (num_docs % 100 == 0) {
      size_t num_lexemes = lexicon_->GetNumLexemes();
      std::stringstream ss;
      ss << "Num documents: " << num_docs;
      ss << ", num lexemes: " << num_lexemes;
      logger_->Log(ss.str());
    }
  }
  tuple_sorter_->FlushHoldingBlock();
  size_t num_docs = doc_map_->GetNumDocs();
  size_t num_lexemes = lexicon_->GetNumLexemes();
  std::stringstream ss;
  ss << "Finished extraction with num documents: " << num_docs;
  ss << ", num lexemes: " << num_lexemes;
  logger_->Log(ss.str());
}

void InvertedIndex::Extract(
  const std::string& directory, const std::string& output_filename
) {
  doc_collection_->Init(directory);
  output_file_ = fopen(output_filename.c_str(), "wb+");  
  if (output_file_ == NULL)
    throw new std::runtime_error("Could not open file " + output_filename);

  WriteHeader();

  tuple_sorter_->Init(output_file_);

  RunExtractor();
  header_.num_blocks = tuple_sorter_->GetNumBlocks();

  header_.doc_map_offset = ftello(output_file_);
  logger_->Log("Started writing the documents map.");
  doc_map_->Write(output_file_, header_.doc_map_offset);
  logger_->Log("Finished writing the documents map.");

  header_.lexicon_offset = ftello(output_file_);
  logger_->Log("Started writing the lexicon.");
  lexicon_->Write(output_file_, header_.lexicon_offset);
  logger_->Log("Finished writing the lexicon.");

  // Update header after we acquired all the missing information.
  header_.num_docs = doc_map_->GetNumDocs();
  header_.num_lexemes = lexicon_->GetNumLexemes();
  WriteHeader();

  logger_->Log("Finished writing the inverted index.");
}

void InvertedIndex::WriteAnchorFile() {
  logger_->Log("Started writing anchor index.");
  doc_collection_->Rewind();
  tuple_sorter_->Clear();

  RawDocument doc;
  size_t counter = 1;
  while (doc_collection_->GetNextDoc(&doc)) {
    extractor_->ExtractLinks(doc);

    if (++counter % 100 == 0) {
      std::stringstream ss;
      ss << "Links extracted from " << counter << " documents.";
      logger_->Log(ss.str());
    }
  }
  tuple_sorter_->FlushHoldingBlock();

  tuple_sorter_->SortAnchorTuples();
  logger_->Log("Finished writing anchor index.");
}

void InvertedIndex::CreateIndexForCollection(
  const std::string& directory, const std::string& output_filename
) {
  doc_collection_->Init(directory);
  output_file_ = fopen(output_filename.c_str(), "wb+");  
  if (output_file_ == NULL)
    throw new std::runtime_error("Could not open file " + output_filename);

  WriteHeader();

  tuple_sorter_->Init(output_file_);

  RunExtractor();
  header_.num_blocks = tuple_sorter_->GetNumBlocks();

  tuple_sorter_->Sort();
  header_.anchor_index_offset = ftello(output_file_);

  CalculateVectorNorms();
  WriteAnchorFile();

  logger_->Log("Calculating page rank.");
  doc_map_->CalculatePageRank();
  logger_->Log("Finished calculating page rank.");

  header_.doc_map_offset = ftello(output_file_);
  logger_->Log("Started writing the documents map.");
  doc_map_->Write(output_file_, header_.doc_map_offset);
  logger_->Log("Finished writing the documents map.");

  header_.lexicon_offset = ftello(output_file_);
  logger_->Log("Started writing the lexicon.");
  lexicon_->Write(output_file_, header_.lexicon_offset);
  logger_->Log("Finished writing the lexicon.");

  // Update header after we acquired all the missing information.
  off_t current = ftello(output_file_);
  header_.num_docs = doc_map_->GetNumDocs();
  header_.num_lexemes = lexicon_->GetNumLexemes();
  WriteHeader();

  logger_->Log("Finished writing the inverted index.");

  fseeko(output_file_, 0, SEEK_END);
  off_t end = ftello(output_file_);
  fclose(output_file_);
  
  // Truncate the file if there is more space than needed after writing everything.
  if (current < end) {
    if (truncate(output_filename.c_str(), current + 1) != 0)
      throw new std::runtime_error("Error truncating file.");
    else 
      std::cout << "The file was truncated by " << end - current << " bytes." << std::endl;
  }
}

void InvertedIndex::Sort(
  const std::string& filename
) {
  output_file_ = fopen(filename.c_str(), "rb+"); 
  ReadHeader();

  logger_->Log("Loading "  + std::to_string(header_.num_docs) + " documents...");
  doc_map_->Load(output_file_, header_.doc_map_offset, header_.num_docs);
  logger_->Log("Finished loading document map.");

  logger_->Log("Loading "  + std::to_string(header_.num_lexemes) + " lexemes...");
  lexicon_->Load(output_file_, header_.lexicon_offset, header_.num_lexemes);
  logger_->Log("Finished loading lexicon.");

  tuple_sorter_->Init(output_file_);
  tuple_sorter_->LoadBlocks(header_.num_blocks, sizeof(InvertedIndexHeader));

  tuple_sorter_->Sort();
  header_.anchor_index_offset = ftello(output_file_);
  logger_->Log("Calculating vector norms.");
  CalculateVectorNorms();
  logger_->Log("Finished calculating vector norms.");

  header_.doc_map_offset = ftello(output_file_);
  logger_->Log("Started writing the documents map.");
  doc_map_->Write(output_file_, header_.doc_map_offset);
  logger_->Log("Finished writing the documents map.");

  header_.lexicon_offset = ftello(output_file_);
  logger_->Log("Started writing the lexicon.");
  lexicon_->Write(output_file_, header_.lexicon_offset);
  logger_->Log("Finished writing the lexicon.");
  off_t current = ftell(output_file_);
  WriteHeader();

  if (truncate(filename.c_str(), current + 1) != 0)
    throw new std::runtime_error("Error truncating file.");
  else 
    std::cout << "The file was truncated to " << current << " bytes." << std::endl;
}

void InvertedIndex::CreateAnchorIndex(
  const std::string& directory, const std::string& filename
) {
  doc_collection_->Init(directory);
  output_file_ = fopen(filename.c_str(), "rb+");  
  if (output_file_ == NULL)
    throw new std::runtime_error("Could not open file " + filename);

  ReadHeader();

  logger_->Log("Loading "  + std::to_string(header_.num_docs) + " documents...");
  doc_map_->Load(output_file_, header_.doc_map_offset, header_.num_docs);
  logger_->Log("Finished loading document map.");

  logger_->Log("Loading "  + std::to_string(header_.num_lexemes) + " lexemes...");
  lexicon_->Load(output_file_, header_.lexicon_offset, header_.num_lexemes);
  logger_->Log("Finished loading lexicon.");

  fseeko(output_file_, header_.anchor_index_offset, SEEK_SET);

  logger_->Log("Started writing anchor index.");
  WriteAnchorFile();
  logger_->Log("Finished writing anchor index.");

  logger_->Log("Calculating page rank.");
  doc_map_->CalculatePageRank();
  logger_->Log("Finished calculating page rank.");

  header_.doc_map_offset = ftello(output_file_);
  logger_->Log("Started writing the documents map.");
  doc_map_->Write(output_file_, header_.doc_map_offset);
  logger_->Log("Finished writing the documents map.");

  header_.lexicon_offset = ftello(output_file_);
  logger_->Log("Started writing the lexicon.");
  lexicon_->Write(output_file_, header_.lexicon_offset);
  logger_->Log("Finished writing the lexicon.");
  WriteHeader();
}

void InvertedIndex::Load(const std::string& filename) {
  output_file_ = fopen(filename.c_str(), "rb+"); 
  ReadHeader();

  logger_->Log("The postings lists section has "  + std::to_string(header_.anchor_index_offset - sizeof(InvertedIndexHeader)) + " bytes");

  logger_->Log("Doc map starts at "  + std::to_string(header_.doc_map_offset));
  logger_->Log("Loading "  + std::to_string(header_.num_docs) + " documents...");
  doc_map_->Load(output_file_, header_.doc_map_offset, header_.num_docs);
  logger_->Log("Finished loading document map.");

  logger_->Log("Lexicon starts at "  + std::to_string(header_.lexicon_offset));
  logger_->Log("Loading "  + std::to_string(header_.num_lexemes) + " lexemes...");
  lexicon_->Load(output_file_, header_.lexicon_offset, header_.num_lexemes);

  off_t current = ftell(output_file_);
  logger_->Log("Lexicon ends at "  + std::to_string(current));
  logger_->Log("Finished loading lexicon.");
}

PostingsList InvertedIndex::GetPostingsList(unsigned int lexeme_id) {
  PostingsList result;
  result.lexeme_id = lexeme_id;

  Lexeme lexeme = lexicon_->GetLexemeById(lexeme_id);
  fseeko(output_file_, lexeme.offset, SEEK_SET);

  size_t bytes_to_read;
  off_t next_offset;

  // This is the last lexeme.
  if (lexeme_id == lexicon_->GetNumLexemes()) {
    next_offset = header_.anchor_index_offset;
  } else {
    Lexeme next_lexeme = lexicon_->GetLexemeById(lexeme_id + 1);
    next_offset = next_lexeme.offset;
  }

  bytes_to_read = next_offset - lexeme.offset;
  BitBuffer bit_buffer;
  for (size_t i = 0; i < bytes_to_read; ++i) {
    unsigned char byte;
    if (fread(&byte, sizeof(unsigned char), 1, output_file_) != 1)
      throw new std::runtime_error("Error reading postings list byte.");
    bit_buffer.WriteByte(byte); 
  } 

  size_t doc_frequency = lexeme.doc_frequency; 
  unsigned int doc_id = 0;
  for (size_t i = 0; i < doc_frequency; ++i) {
    doc_id += bit_buffer.ReadInt();
    unsigned int term_frequency = bit_buffer.ReadInt();

    unsigned int word_offset = 0;
    for (size_t j = 0; j < term_frequency; ++j) {
      word_offset += bit_buffer.ReadInt();
      result.word_offsets[doc_id].push_back(word_offset);
    }
  }

  return result;
}

PostingsList InvertedIndex::GetPostingsList(const std::string& lexeme) {
  unsigned int lexeme_id = lexicon_->GetLexemeId(lexeme);
  if (lexeme_id == 0) return PostingsList();
  return GetPostingsList(lexeme_id);
}

std::map<unsigned int, unsigned int> InvertedIndex::GetAnchorRank(unsigned int lexeme_id) {
  std::map<unsigned int, unsigned int> result;
  Lexeme lex = lexicon_->GetLexemeById(lexeme_id);
  if (lex.anchor_offset == 0) return result;

  fseeko(output_file_, lex.anchor_offset, SEEK_SET);

  size_t num_docs;
  if (fread(&num_docs, sizeof(size_t), 1, output_file_) != 1)
    throw new std::runtime_error("Error reading anchor index.");

  for (size_t i = 0; i < num_docs; ++i) {
    unsigned int doc_id, frequency;
    if (fread(&doc_id, sizeof(unsigned int), 1, output_file_) != 1)
      throw new std::runtime_error("Error reading anchor index.");
   
    if (fread(&frequency, sizeof(unsigned int), 1, output_file_) != 1)
      throw new std::runtime_error("Error reading anchor index.");
   
    result[doc_id] = frequency; 
  }
  return result;
}

std::map<unsigned int, unsigned int> InvertedIndex::GetAnchorRank(const std::string& lexeme) {
  unsigned int lexeme_id = lexicon_->GetLexemeId(lexeme);
  if (lexeme_id == 0) {
    std::map<unsigned int, unsigned int> result;
    return result;
  }
  return GetAnchorRank(lexeme_id);
}

void InvertedIndex::CalculateVectorNorms() {
  logger_->Log("Calculating vector norms.");
  size_t num_lexemes = lexicon_->GetNumLexemes();
  for (size_t i = 1; i <= num_lexemes; ++i) {
    PostingsList list = GetPostingsList(i);

    Lexeme lexeme = lexicon_->GetLexemeById(i);
    double idf = log2(1 + (doc_map_->GetNumDocs() / lexeme.doc_frequency));
    for (auto& it : list.word_offsets) {
      double tf = log2(1 + it.second.size());

      // TF x IDF squared.
      double weight = (tf * idf) * (tf * idf);
      doc_map_->AddToVectorNorm(it.first, weight);
    }
  }
  doc_map_->SqrtVectorNorms();
  logger_->Log("Finished calculating vector norms.");
}

} // End of namespace.
