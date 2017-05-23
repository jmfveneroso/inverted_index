#include "extractor.hpp"
#include "utf8cpp/utf8.h"
#include <algorithm>
#include <sstream>

namespace InvertedIndex {

Extractor::Extractor(
  std::shared_ptr<ILogger> logger,
  std::shared_ptr<ILexicon> lexicon,
  std::shared_ptr<DocMap> doc_map,
  std::shared_ptr<ITupleSorter> tuple_sorter,
  std::shared_ptr<IDocCollection> doc_collection
) : logger_(logger), lexicon_(lexicon), 
    doc_map_(doc_map), tuple_sorter_(tuple_sorter), doc_collection_(doc_collection) {
}

std::string Extractor::GetCleanText(GumboNode* node) {
  if (node->type == GUMBO_NODE_TEXT) return std::string(node->v.text.text);

  if (node->type != GUMBO_NODE_ELEMENT || node->v.element.tag == GUMBO_TAG_SCRIPT ||
    node->v.element.tag == GUMBO_TAG_STYLE) return "";

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

bool Extractor::IsStopWord(const std::string& lexeme){
  static std::set<std::string> stop_words = {
    "de", "a", "o", "que", "e", "do", "da", "em", "um", "para", "e", "com", 
    "nao", "uma", "os", "no", "se", "na", "por", "mais", "as", "dos", "como", 
    "mas", "foi", "ao", "ele", "das", "tem", "seu", "sua", "ou", "ser", "quando", 
    "muito", "ha", "nos", "ja", "esta", "eu", "tambem", "so", "pelo", "pela", "ate", 
    "isso", "ela", "entre", "era", "depois", "sem", "mesmo", "aos", "ter", "seus", 
    "quem", "nas", "me", "esse", "eles", "estao", "voce", "tinha", "foram", "essa", 
    "num", "nem", "suas", "meu", "às", "minha", "tem", "numa", "pelos", "elas", 
    "havia", "seja", "qual", "sera", "nos", "tenho", "lhe", "deles", "essas", "esses", 
    "pelas", "este", "fosse", "dele", "tu", "te", "voces", "vos", "lhes", "meus", "minhas", 
    "teu", "tua", "teus", "tuas", "nosso", "nossa", "nossos", "nossas", "dela", "delas", "esta", 
    "estes", "estas", "aquele", "aquela", "aqueles", "aquelas", "isto", "aquilo", "estou", 
    "esta", "estamos", "estao", "estive", "esteve", "estivemos", "estiveram", "estava", "estavamos", 
    "estavam", "estivera", "estiveramos", "esteja", "estejamos", "estejam", "estivesse", "estivessemos", 
    "estivessem", "estiver", "estivermos", "estiverem", "hei", "há", "havemos", "hao", "houve", 
    "houvemos", "houveram", "houvera", "houveramos", "haja", "hajamos", "hajam", "houvesse", 
    "houvessemos", "houvessem", "houver", "houvermos", "houverem", "houverei", "houvera", "houveremos", 
    "houverao", "houveria", "houveriamos", "houveriam", "sou", "somos", "sao", "era", "eramos", "eram", 
    "fui", "foi", "fomos", "foram", "fora", "foramos", "seja", "sejamos", "sejam", "fosse", "fossemos", 
    "fossem", "for", "formos", "forem", "serei", "sera", "seremos", "serao", "seria", "seriamos", 
    "seriam", "tenho", "tem", "temos", "tem", "tinha", "tinhamos", "tinham", "tive", "teve", "tivemos", 
    "tiveram", "tivera", "tiveramos", "tenha", "tenhamos", "tenham", "tivesse", "tivéssemos", "tivessem", 
    "tiver", "tivermos", "tiverem", "terei", "tera", "teremos", "terao", "teria", "teriamos", "teriam"  
  };

  return stop_words.find(lexeme) != stop_words.end();
}

void Extractor::Parse(GumboNode* node, bool get_links) {
  if (node->type == GUMBO_NODE_TEXT) {  
    if (get_links) return;
    std::vector<std::string> lexemes = Lexicon::ExtractLexemes(node->v.text.text);
    lexemes_.insert(lexemes_.end(), lexemes.begin(), lexemes.end());
  }

  if (node->type != GUMBO_NODE_ELEMENT || node->v.element.tag == GUMBO_TAG_SCRIPT ||
    node->v.element.tag == GUMBO_TAG_STYLE) return;

  GumboVector* children = &node->v.element.children;
  for (unsigned int i = 0; i < children->length; ++i) {
    Parse((GumboNode*) children->data[i], get_links);
  }

  if (!get_links) return;

  GumboAttribute* href;
  bool is_link = node->v.element.tag == GUMBO_TAG_A &&
    (href = gumbo_get_attribute(&node->v.element.attributes, "href"));

  if (!is_link) return;

  std::string anchor_text = GetCleanText(node);
  std::vector<std::string> lexemes = Lexicon::ExtractLexemes(anchor_text);
  for (auto lexeme : lexemes) { 
    if (IsStopWord(lexeme)) continue;

    unsigned int lexeme_id = lexicon_->GetLexemeId(lexeme);

    std::string href_str = href->value;
    std::string normalized_link = NormalizeHyperlink(current_doc_id_, href_str);
    unsigned int doc_id = doc_map_->GetDocId(normalized_link);
    if (doc_id == 0 || current_doc_id_ == doc_id) continue; 

    lexicon_->AddLink(lexeme_id, doc_id);
    doc_map_->AddOutboundLink(current_doc_id_, doc_id);
  }
}

void Extractor::Parse(const std::string& document, bool get_links) {
  GumboOutput* output = gumbo_parse(document.c_str());
  Parse(output->root, get_links);
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
  size_t doc_offset = 0;
  while (doc.content[doc_offset] == ' ') { 
    doc_offset++;
    if (doc_offset >= doc.content.size()) break;
  }

  // Gumbo Parser does not handle XML very well.
  if (doc.content.find("<?xml") == doc_offset) {
    // logger_->Log("Skipping XML document: " + doc.url);
    return;
  }

  Parse(doc.content);
  unsigned int word_count = 0;
  for (auto lexeme : lexemes_) {
    unsigned int lexeme_id = lexicon_->AddLexeme(lexeme);
    Tuple tuple = Tuple(lexeme_id, doc_id, ++word_count);
    tuple_sorter_->WriteTuple(tuple);
  }
  lexemes_.clear();
}

void Extractor::ExtractLinks(RawDocument& doc) {
  current_doc_id_ = doc_map_->GetDocId(doc.url);
  if (current_doc_id_ == 0) {
    logger_->Log("Skipping links from: " + doc.url);
    return;
  }

  size_t doc_offset = 0;
  while (doc.content[doc_offset] == ' ') { 
    doc_offset++;
    if (doc_offset >= doc.content.size()) break;
  }

  // Gumbo Parser does not handle XML very well.
  if (doc.content.find("<?xml") == doc_offset) return;

  Parse(doc.content, true);
  lexemes_.clear();
}

std::string Extractor::TruncateUrl(std::string url) {
  if (url.find("http://")    == 0)   url = url.substr(7);
  if (url.find("https://")   == 0)   url = url.substr(8);
  if (url.find("www.")       == 0)   url = url.substr(4);
  while (url[url.size() - 1] == '/') url = url.substr(0, url.size() - 1);
  return url;
}

std::string Extractor::GetRootUrl(std::string url) {
  url = TruncateUrl(url);
  size_t pos = url.find_first_of('/');
  return url.substr(0, pos - 1);
}

std::string Extractor::NormalizeHyperlink(unsigned int doc_id, std::string& url) {
  // Trim.
  url.erase(
    url.begin(), 
    std::find_if(url.begin(), url.end(), 
    std::not1(std::ptr_fun<int, int>(std::isspace)))
  );
  url.erase(
    std::find_if(url.rbegin(), url.rend(),
    std::not1(std::ptr_fun<int, int>(std::isspace))).base(), url.end()
  );

  if (url.find("/") == 0) {
    Document doc = doc_map_->GetDocById(doc_id);
    if (url.size() == 1) return GetRootUrl(doc.url);
    return GetRootUrl(doc.url) + url;
  }

  return TruncateUrl(url);
}

void Extractor::PrintLexemes(RawDocument& doc) {
  Parse(doc.content);
  unsigned int word_count = 0;
  for (auto lexeme : lexemes_) {
    std::cout << "(" << ++word_count << ") " << lexeme << " ";
  }
  std::cout << std::endl;
  lexemes_.clear();
}

void Extractor::ReadDoc(unsigned int doc_id) {
  Document doc = doc_map_->GetDocById(doc_id);
  std::cout << "Reading document " << doc.url << " from file " << doc.file_num << 
               " and offset " << doc.offset << std::endl;
  RawDocument raw_doc = doc_collection_->Read(doc.file_num, doc.offset);
  PrintLexemes(raw_doc);
}

} // End of namespace.
