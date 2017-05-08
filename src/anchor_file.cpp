#include "anchor_file.hpp"
#include <algorithm>  
#include <sstream>  

namespace TP1 {

AnchorFile::AnchorFile(
  std::shared_ptr<ILogger> logger,
  std::shared_ptr<DocMap> doc_map,
  std::shared_ptr<ILexicon> lexicon
) : logger_(logger), doc_map_(doc_map), 
    lexicon_(lexicon) {
}

void AnchorFile::Init() {
  file_ = std::tmpfile();
  if (file_ == NULL) 
    throw new std::runtime_error("Error opening temporary file");
}

void AnchorFile::WriteString(const std::string& str) {
  static char c = '\0';
  static char buffer[10000];
  strncpy(buffer, str.c_str(), str.size());
  fwrite(buffer, sizeof(char), str.size(), file_);
  fwrite(&c, sizeof(char), 1, file_);
}

std::string AnchorFile::ReadString() {
  std::string result;
  char c;
  while (fread(&c, sizeof(char), 1, file_) > 0) {
    if (c == '\0') return result;
    result += c; 
  }
  throw new std::runtime_error("Error reading string from temporary file");
}

void AnchorFile::WriteHyperlinks(
  const std::vector<Hyperlink>& hyperlinks
) {
  size_t num_hyperlinks = hyperlinks.size();
  fwrite(&num_hyperlinks, sizeof(size_t), 1, file_);
  num_hyperlinks_ += num_hyperlinks;

  for (auto hyperlink : hyperlinks) {
    WriteString(hyperlink.href);
    WriteString(hyperlink.anchor_text);
  }
}

std::string AnchorFile::TruncateUrl(std::string url) {
  if (url.find("http://")  == 0)  url = url.substr(7);
  if (url.find("https://") == 0)  url = url.substr(8);
  if (url.find("www.")     == 0)  url = url.substr(4);
  if (url[url.size() - 1] == '/') url = url.substr(0, url.size() - 1);
  return url;
}

std::string AnchorFile::GetRootUrl(std::string url) {
  url = TruncateUrl(url);
  size_t pos = url.find_first_of('/');
  return url.substr(0, pos - 1);
}

std::string AnchorFile::NormalizeHyperlink(unsigned int doc_id, std::string& url) {
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

void AnchorFile::ReadHyperlinks() {
  fseeko(file_, 0, SEEK_SET);
  for (unsigned int i = 1; i <= doc_map_->GetNumDocs(); ++i) {
    size_t num_hyperlinks = 0;
    fread(&num_hyperlinks, sizeof(size_t), 1, file_);

    Hyperlink hyperlink;
    for (size_t j = 0; j < num_hyperlinks; ++j) {
      hyperlink.href = ReadString();
      hyperlink.anchor_text = ReadString();

      std::string normalized_link = NormalizeHyperlink(i, hyperlink.href);
      unsigned int doc_id = doc_map_->GetDocId(normalized_link);
      if (doc_id == 0 || i == doc_id) continue; 

      std::vector<std::string> lexemes;
      lexemes = Lexicon::ExtractLexemes(hyperlink.anchor_text);
      for (auto lexeme : lexemes) {
        unsigned int lexeme_id = lexicon_->GetLexemeId(lexeme);
        if (lexeme_id == 0) continue;
        lexicon_->AddLink(lexeme_id, doc_id);
      }
      doc_map_->AddOutboundLink(i, doc_id);
    }

    if (i % 10000 == 0) {
      std::stringstream ss;
      ss << "Document hyperlinks parsed: " << i;
      logger_->Log(ss.str());
    }
  }
  std::stringstream ss;
  ss << "Document hyperlinks parsed: " << doc_map_->GetNumDocs();
  logger_->Log(ss.str());
}

void AnchorFile::Close() {
  fclose(file_);
}

} // End of namespace.
