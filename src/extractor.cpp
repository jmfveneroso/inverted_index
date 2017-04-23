#include "extractor.hpp"

namespace TP1 {

Extractor::Extractor(std::shared_ptr<ILexicon> lexicon) 
  : lexicon_(lexicon) {
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
      if (i != 0 && !text.empty()) {
        contents.append(" ");
      }
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

void Extractor::ExtractFromDoc(int doc_id, const string& document) {
  GumboOutput* output = gumbo_parse(document.c_str());
  string text = GetCleanText(output->root);
  
  int word_count = 0;
  vector<string> lexemes = Lexicon::ExtractLexemes(text);
  vector<string>::iterator it = lexemes.begin();
  for (; it != lexemes.end(); it++) {
    int lexeme_id = lexicon_->AddLexeme(*it);

    Tuple t = Tuple(lexeme_id, doc_id, ++word_count);
    fwrite(&t, sizeof(Tuple), 1, output_file_);
  }
  
  gumbo_destroy_output(&kGumboDefaultOptions, output);
  num_tuples_ += lexemes.size();
}

void Extractor::Extract(
  const std::string& input_filename, const std::string& output_filename
) {
  if ((input_file_ = std::fopen(input_filename.c_str(), "rb")) == NULL)
    throw new std::runtime_error("Error opening input file: " + input_filename);
  if ((output_file_ = std::fopen(output_filename.c_str(), "wb")) == NULL)
    throw new std::runtime_error("Error opening output file: " + output_filename);
  fseeko(input_file_, 3, SEEK_SET);

  int doc_id = 0, pipes = 0;
  std::string content; 
  char buffer[1024];
  size_t num_read = 0; 
  while ((num_read = fread(buffer, sizeof(char), 1024, input_file_)) != 0) {
    for (size_t i = 0; i < num_read; ++i) {
      if (buffer[i] == '|') {
        ++pipes;
        continue;
      }

      if (pipes == 1) { // End of url.
        // doc.url = text;
        content = std::string();
      } else if (pipes == 3) { // End of document content.
        ExtractFromDoc(doc_id++, content);
        content = std::string();
      }
      content += buffer[i];
      pipes = 0;
    }
  }
  std::fclose(input_file_); 
  std::fclose(output_file_); 
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
