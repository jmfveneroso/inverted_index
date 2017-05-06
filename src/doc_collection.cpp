#include <sstream>
#include <algorithm>  
#include "doc_collection.hpp"

namespace TP1 {

DocCollection::DocCollection(std::shared_ptr<ILogger> logger) 
  : logger_(logger) {
}

bool DocCollection::OpenNextFile() {
  if (file_counter_ > max_counter_) return false;

  std::stringstream ss;
  ss << directory_ << "/html_" << file_counter_++;
  logger_->Log(ss.str());
  if (file_) fclose(file_);
  file_ = fopen(ss.str().c_str(), "rb+");
  if (!file_)
    throw std::runtime_error(
      "Error opening file " + ss.str()
    );

  fseeko(file_, 3, SEEK_SET);
  offset_ = 3;
  return true;
}

void DocCollection::TrimUrl(std::string& s) {
  s.erase(
    s.begin(), 
    std::find_if(s.begin(), s.end(), 
    std::not1(std::ptr_fun<int, int>(std::isspace)))
  );
  s.erase(
    std::find_if(s.rbegin(), s.rend(),
    std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end()
  );
}

void DocCollection::Init(const std::string& directory) {
  directory_ = directory;
  OpenNextFile();
}

bool DocCollection::GetNextDoc(RawDocument* doc) {
  doc->url = doc->content = std::string();

  char c;
  size_t num_read = 0;
  while ((num_read = fread(&c, sizeof(char), 1, file_)) > 0) {
    offset_++;
    if (c == '|') break;
    doc->url += c;
  }

  if (num_read == 0) {
    if (!OpenNextFile()) return false;
    return GetNextDoc(doc);
  }
  TrimUrl(doc->url);

  while (fread(&c, sizeof(char), 1, file_) > 0) {
    offset_++;
    if (c == '|') {
      fseeko(file_, 2, SEEK_CUR);
      return true;
    }
    doc->content += c;
  }

  if (num_read == 0)
    throw std::runtime_error(
      "Reached end of file without finishing document content."
    );

  return true;
}

} // End of namespace.
