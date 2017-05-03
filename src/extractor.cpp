#include "extractor.hpp"
#include <algorithm>

namespace TP1 {

Extractor::Extractor(
  std::shared_ptr<ILogger> logger,
  std::shared_ptr<ILexicon> lexicon,
  std::shared_ptr<IDocFinder> doc_finder
) : logger_(logger), lexicon_(lexicon), doc_finder_(doc_finder) {
}

std::string Extractor::GetCleanText(GumboNode* node) {
  if (node->type == GUMBO_NODE_TEXT) return std::string(node->v.text.text);

  if (
    node->type == GUMBO_NODE_ELEMENT &&
    node->v.element.tag != GUMBO_TAG_SCRIPT &&
    node->v.element.tag != GUMBO_TAG_STYLE
  ) {
    std::string contents = "";
    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
      const std::string text = GetCleanText((GumboNode*) children->data[i]);
      if (i != 0 && !text.empty()) contents.append(" ");
      contents.append(text);
    }
    return contents;
  }
  return "";
}

std::string Extractor::GetCleanText(const string& document) {
  GumboOutput* output = gumbo_parse(document.c_str());
  string text = GetCleanText(output->root);
  gumbo_destroy_output(&kGumboDefaultOptions, output);
  return text;
}

void Extractor::FlushBlock() {
  if (tuples_in_block_ == 0) return;

  sort(tuple_block_, tuple_block_ + tuples_in_block_, std::less<Tuple>());
  for (size_t i = 0; i < tuples_in_block_; ++i) {
    fwrite(&tuple_block_[i], sizeof(Tuple), 1, output_file_);
  }
  
  // Write the padding if the block was not completely filled. This condition
  // will only be called for the last block.
  for (size_t i = tuples_in_block_; i < MAX_TUPLES; ++i) {
    static Tuple t(0, 0, 0);
    fwrite(&t, sizeof(Tuple), 1, output_file_);
  }

  logger_->Log(
    std::to_string(++num_blocks_) + " blocks with " + 
    std::to_string(MAX_TUPLES) + " tuples written."
  );
}

void Extractor::WriteTuple(Tuple& tuple) {
  if (tuples_in_block_ == MAX_TUPLES) {
    FlushBlock();
    tuples_in_block_ = 0;
  }
  tuple_block_[tuples_in_block_++] = tuple;
}

void Extractor::ExtractFromDoc(int doc_id, const string& document) {
  GumboOutput* output = gumbo_parse(document.c_str());
  string text = GetCleanText(output->root);
  
  int word_count = 0;
  vector<string> lexemes = Lexicon::ExtractLexemes(text);
  vector<string>::iterator it = lexemes.begin();
  for (; it != lexemes.end(); it++) {
    int lexeme_id = lexicon_->AddLexeme(*it);
    Tuple t = Tuple(lexeme_id, doc_id, ++word_count);
    WriteTuple(t);
  }
  
  gumbo_destroy_output(&kGumboDefaultOptions, output);
  num_tuples_ += lexemes.size();
}

void Extractor::Extract(
  const std::string& input_filename, const std::string& output_filename
) {
  if ((input_file_ = std::fopen(input_filename.c_str(), "rb")) == NULL)
    throw new std::runtime_error("Error opening file: " + input_filename);
  if ((output_file_ = std::fopen(output_filename.c_str(), "wb")) == NULL)
    throw new std::runtime_error("Error opening file: " + output_filename);

  char buffer[1024];
  int doc_id = 0, pipes = 0;
  size_t num_read = 0; 
  size_t offset = 0; 
  Document doc;
  std::string content;

  logger_->Log("Started tuple extraction.");
  fseeko(input_file_, 3, SEEK_SET);
  while ((num_read = fread(buffer, sizeof(char), 1024, input_file_)) != 0) {
    for (size_t i = 0; i < num_read; ++i) {
      ++offset;

      if (buffer[i] == '|') { ++pipes; continue; }

      if (pipes == 1) { // End of url.
        doc.url = content;
        doc.offset = offset + 1;
        content = std::string();
      } else if (pipes == 3) { // End of document content.
        doc_finder_->InsertDocument(doc);
        ExtractFromDoc(doc_id++, content);
        content = std::string();
        if (doc_id % 1000 == 0)
          logger_->Log(
            std::to_string(num_tuples_ ) + " tuples extracted from " + 
            std::to_string(doc_id) + " documents."
          );
      }
      content += buffer[i];
      pipes = 0;
    }
    if (doc_id == 3000) break;
  }
  FlushBlock();
  std::fclose(input_file_); 
  std::fclose(output_file_); 
  logger_->Log("Finished tuple extraction.");
}

void Extractor::Print(const std::string& filename) {
  FILE* f = std::fopen(filename.c_str(), "rb");

  int i = 0;
  Tuple t;
  while (fread(&t, sizeof(Tuple), 1, f) != 0) {
    std::cout << "Tuple " << ++i << ": {" << t.lexeme_id << ", "
              << t.document_id << ", " << t.word_position << "}" << std::endl;
  }
}

} // End of namespace.
