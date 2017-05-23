#include <sstream>
#include <algorithm>  
#include "doc_collection.hpp"

namespace InvertedIndex {

DocCollection::DocCollection(std::shared_ptr<ILogger> logger)
  : logger_(logger) {
  start_counter_ = file_counter_;
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

bool DocCollection::ReadBit(char* c) {
  offset_++;
  if (char_buffer_.cursor == char_buffer_.num_read) {
    char_buffer_.num_read = fread(char_buffer_.buffer, sizeof(char), 10000, file_);
    char_buffer_.cursor = 0;
    if (char_buffer_.num_read == 0) {
      if (OpenNextFile()) return ReadBit(c);
      else return false;
    }
  } 
  *c = char_buffer_.buffer[char_buffer_.cursor++];
  return true;
}

bool DocCollection::GetNextDoc(RawDocument* doc) {
  doc->url = doc->content = std::string();
  doc->file_number = (file_counter_ == 0) ? file_counter_ : file_counter_ - 1;
  doc->offset = offset_;

  char c;
  while (ReadBit(&c)) {
    if (c == '|') break;
    doc->url += c;
  }

  if (char_buffer_.num_read == 0) return false;
  TrimUrl(doc->url);

  size_t pipes = 0;
  while (ReadBit(&c)) {
    if (c == '|') { 
      ++pipes;
      if (pipes == 3) return true;
      continue;
    } else if (pipes > 0) {
      logger_->Log(
        "There is a trailing pipe at file " + std::to_string(file_counter_) + ": " + 
        std::to_string(offset_) + ". Treating it as content."
      );
      pipes = 0;
    }

    doc->content += c;
  }

  throw std::runtime_error(
    "Reached end of file without finishing document content."
  );
}

RawDocument DocCollection::Read(size_t file_num, size_t offset) {
  std::stringstream ss;
  ss << directory_ << "/html_" << file_num;
  if (file_) fclose(file_);
  file_ = fopen(ss.str().c_str(), "rb+");
  if (!file_) throw std::runtime_error("Error opening file " + ss.str());

  fseeko(file_, offset, SEEK_SET);
  RawDocument doc;
  char_buffer_.num_read = 0;
  char_buffer_.cursor = 0;
  GetNextDoc(&doc);

  return doc;
  // std::cout << doc.content << std::endl;
}

void DocCollection::Rewind() {
  file_counter_ = start_counter_;
  OpenNextFile();
}

} // End of namespace.
