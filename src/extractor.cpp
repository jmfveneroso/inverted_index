#include "extractor.hpp"
#include "utf8cpp/utf8.h"
#include <algorithm>
#include <sstream>

namespace TP1 {

Extractor::Extractor(
  std::shared_ptr<ILogger> logger,
  std::shared_ptr<ILexicon> lexicon,
  std::shared_ptr<DocMap> doc_map,
  std::shared_ptr<AnchorFile> anchor_file,
  std::shared_ptr<ITupleSorter> tuple_sorter
) : logger_(logger), lexicon_(lexicon), 
    doc_map_(doc_map), anchor_file_(anchor_file),
    tuple_sorter_(tuple_sorter) {
}

std::string Extractor::GetCleanText(GumboNode* node) {
  if (node->type == GUMBO_NODE_TEXT) return std::string(node->v.text.text);

  if (node->type != GUMBO_NODE_ELEMENT || node->v.element.tag == GUMBO_TAG_SCRIPT ||
    node->v.element.tag == GUMBO_TAG_STYLE) return "";

  std::string contents = "";
  for (size_t i = 0; i < node->v.element.children.length; ++i) {
    std::string text = GetCleanText((GumboNode*) node->v.element.children.data[i]);
    if (i != 0 && !text.empty()) contents += " ";
    contents += text;
  }
  return contents;
}

void Extractor::Parse(GumboNode* node) {
  if (node->type == GUMBO_NODE_TEXT) { 
    std::vector<std::string> lexemes = Lexicon::ExtractLexemes(node->v.text.text);
    lexemes_.insert(lexemes_.end(), lexemes.begin(), lexemes.end());
  }

  if (node->type != GUMBO_NODE_ELEMENT || node->v.element.tag == GUMBO_TAG_SCRIPT ||
    node->v.element.tag == GUMBO_TAG_STYLE) return;

  for (size_t i = 0; i < node->v.element.children.length; ++i)
    Parse((GumboNode*) node->v.element.children.data[i]);

  GumboAttribute* href;
  bool is_link = node->v.element.tag == GUMBO_TAG_A &&
    (href = gumbo_get_attribute(&node->v.element.attributes, "href"));

  if (is_link) {
    std::string anchor_text = GetCleanText(node);
    std::vector<std::string> lexemes = Lexicon::ExtractLexemes(anchor_text);
    std::vector<unsigned int> lexeme_ids;
    for (auto lexeme : lexemes) lexeme_ids.push_back(lexicon_->AddLexeme(lexeme));
    hyperlinks_.push_back(Hyperlink(href->value, lexeme_ids));
  }
}

void Extractor::Parse(const string& document) {
  GumboOutput* output = gumbo_parse(document.c_str());
  Parse(output->root);
  gumbo_destroy_output(&kGumboDefaultOptions, output);
}

void Extractor::ExtractFromDoc(RawDocument& doc) {
  Document new_doc(doc.url, doc.file_number, doc.offset);

  unsigned int id = doc_map_->GetDocId(doc.url);
  if (id != 0) {
    std::stringstream ss;
    ss << "Ignoring duplicate document: " << doc.url << ". File number: " 
       << doc.file_number << ", offset: " << doc.offset << ".";
    logger_->Log(ss.str());
    return; 
  }
  unsigned int doc_id = doc_map_->AddDoc(new_doc);

  Parse(doc.content);
  unsigned int word_count = 0;
  for (auto lexeme : lexemes_) {
    unsigned int lexeme_id = lexicon_->AddLexeme(lexeme);
    Tuple tuple = Tuple(lexeme_id, doc_id, ++word_count);
    tuple_sorter_->WriteTuple(tuple);
  }
  anchor_file_->WriteHyperlinks(hyperlinks_);
  hyperlinks_.clear();
  lexemes_.clear();
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
