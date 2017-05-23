#include "doc_map.hpp"
#include <math.h>
#include <cstring>

namespace InvertedIndex {

DocMap::DocMap(std::shared_ptr<ILogger> logger) 
  : logger_(logger) {
}
 
std::string DocMap::TruncateUrl(std::string url) {
  if (url.find("http://")  == 0)  url = url.substr(7);
  if (url.find("https://") == 0)  url = url.substr(8);
  if (url.find("www.")     == 0)  url = url.substr(4);
  while (url[url.size() - 1] == '/') url = url.substr(0, url.size() - 1);
  return url;
}

unsigned int DocMap::GetDocId(const std::string& url) {
  std::string truncated_url = TruncateUrl(url);

  std::map<std::string, unsigned int>::iterator it;
  if ((it = id_map_.find(truncated_url)) == id_map_.end())
    return 0;
  else
    return it->second;
}

unsigned int DocMap::AddDoc(Document& doc) {
  doc.url = TruncateUrl(doc.url);

  unsigned int id = GetDocId(doc.url);
  if (id == 0) {
    id_map_.insert({ doc.url, ++id_counter_ });
    doc_map_.insert({ id_counter_, doc });
    return id_counter_;
  } else
    return id;

}

Document DocMap::GetDocById(unsigned int id) {
  std::map<unsigned int, Document>::iterator it;
  if ((it = doc_map_.find(id)) == doc_map_.end())
    return Document();
  else
    return it->second;
}

void DocMap::AddToVectorNorm(unsigned int id, double weight) {
  doc_map_[id].vector_norm += weight;
}

void DocMap::SqrtVectorNorms() {
  for (auto& it : doc_map_) {
    it.second.vector_norm = sqrt(it.second.vector_norm);
  }
}

void DocMap::AddOutboundLink(unsigned int id, unsigned int outbound_id) {
  doc_map_[id].outbound_links.insert(outbound_id);
}

void DocMap::CalculatePageRank() {
  for (size_t i = 0; i < 300; ++i) {
    if (i % 10 == 0) logger_->Log("Page rank iteration: " + std::to_string(i));

    double global_rank = 0;
    for (size_t j = 1; j < doc_map_.size(); ++j) {
      Document& doc = doc_map_[j];

      // Is sink. Add to glboal rank.
      if (doc.outbound_links.size() == 0) {
        global_rank += doc.page_rank / doc_map_.size();
        continue;
      }

      double propagated_rank = doc.page_rank / doc.outbound_links.size();
      for (auto link_id : doc.outbound_links) {
        Document& child = doc_map_[link_id];
        child.next_rank += propagated_rank;
      }
    } 

    double total = 0;
    // Commit new ranks and add global rank.
    for (size_t j = 1; j < doc_map_.size(); ++j) {
      Document& doc = doc_map_[j];
      doc.page_rank = doc.next_rank + global_rank;
      doc.next_rank = 0;
      total += doc.page_rank;
    }
  }
}

void DocMap::Write(FILE* file, off_t offset) {
  fseeko(file, offset, SEEK_SET);
  for (size_t i = 1; i <= doc_map_.size(); ++i) {
    Document& doc = doc_map_[i];

    static char c = '\0';
    static char buffer[10000];
    size_t size = (doc.url.size() <= 10000) ? doc.url.size() : 0;
    std::strncpy(buffer, doc.url.c_str(), size);
    fwrite(buffer, sizeof(char), size, file);
    fwrite(&c, sizeof(char), 1, file);

    fwrite(&doc.file_num, sizeof(size_t), 1, file);
    fwrite(&doc.offset, sizeof(off_t), 1, file);
    fwrite(&doc.page_rank, sizeof(double), 1, file);
    fwrite(&doc.vector_norm, sizeof(double), 1, file);
  }
}

void DocMap::Load(FILE* file, off_t offset, size_t num_docs) {
  fseeko(file, offset, SEEK_SET);
  for (size_t i = 0; i < num_docs; ++i) {
    Document doc;
    char c;
    while (fread(&c, sizeof(char), 1, file) > 0) {
      if (c == '\0') break;
      doc.url += c; 
    }

    size_t result = 0;
    result += fread(&doc.file_num, sizeof(size_t), 1, file);
    result += fread(&doc.offset, sizeof(off_t), 1, file);
    result += fread(&doc.page_rank, sizeof(double), 1, file);
    result += fread(&doc.vector_norm, sizeof(double), 1, file);
    if (result != 4) throw new std::runtime_error("Error loading doc map.");
    AddDoc(doc); 
  }
}

} // End of namespace.
