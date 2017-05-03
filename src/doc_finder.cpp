#include <utility>
#include "doc_finder.h"
#include "extractor.hpp"

using namespace std;
using namespace TP1;

DocFinder::DocFinder(std::shared_ptr<IInvertedFileReader> inverted_file_reader)
  : inverted_file_reader_(inverted_file_reader) {}

DocFinder::~DocFinder () {
  // if (file_ != NULL) std::fclose(file_); 
}

void DocFinder::LoadFromFile(const std::string& filename) {
  file_ = std::fopen(filename.c_str(), "rb");
  if (file_ == NULL)
    throw new std::runtime_error("Error opening file: " + filename);

  fseeko(file_, 3, SEEK_SET);

  int doc_id = 0, pipes = 0;
  std::string content; 
  char buffer[1024];
  size_t num_read = 0; 

  std::cout << "Loading urls..." << std::endl;
  while ((num_read = fread(buffer, sizeof(char), 1024, file_)) != 0) {
    for (size_t i = 0; i < num_read; ++i) {
      if (buffer[i] == '|') {
        ++pipes;
        continue;
      }

      if (pipes == 1) { // End of url.
        Document doc(content, 0, 0);
        url_map_.insert({ doc_id++, doc });
        content = std::string();
      } else if (pipes == 3) { // End of document content.
        content = std::string();
      }
      content += buffer[i];
      pipes = 0;
    }
  }
  std::cout << "Finished loading urls." << std::endl;
}

/**
 * Returns a vector with only unique values.
 *
 */
std::vector<int> DocFinder::UniqueDocs(vector<int> v) {
  // Get only unique document ids.
  vector<int>::iterator it;
  sort(v.begin(), v.end());
  it = unique(v.begin(), v.end());
  
  // Resize the vector to fit all elements properly.
  v.resize(std::distance(v.begin(), it));
  
  return v;
}

/**
 * Returns the intersection between two vectors.
 *
 */
std::vector<int> DocFinder::OperatorAnd(vector<int> v1, vector<int> v2) {
  // Sort both vectors.
  std::sort(v1.begin(), v1.end());
  std::sort(v2.begin(), v2.end());
  
  // Get intersection between vectors.
  std::vector<int> v_intersection;
  std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v_intersection));
  
  return v_intersection;
}

/**
 * Returns the union between two vectors.
 *
 */
std::vector<int> DocFinder::OperatorOr(vector<int> v1, vector<int> v2) {
  std::vector<int> result;
  
  // Concat both vectors.
  result.reserve(v1.size() + v2.size());
  result.insert(result.end(), v1.begin(), v1.end());
  result.insert(result.end(), v2.begin(), v2.end());
  
  return DocFinder::UniqueDocs(result);
}

/**
 * Performs a boolean query in an inverted file. It supports AND and OR operators.
 *
 */
vector<int> DocFinder::BooleanQuery(const string& query) {
  // Extract valid lexemes from the query string.
  vector<string> words = Lexicon::ExtractLexemes(query);
  
  vector<int> result;
  
  bool op_and = false, op_or = false;
  for (vector<string>::iterator it = words.begin(); it < words.end(); it++) {    
    if (it->compare("and") == 0 || it->compare("AND") == 0) {
      if (op_and || op_or)
        cout << "invalid query" << endl;
      else
        op_and = true;
    } else if (it->compare("or") == 0 || it->compare("OR") == 0) {
      if(op_and || op_or)
        cout << "invalid query" << endl;
      else
        op_or = true;
    } else {
      vector<Tuple> tuples = inverted_file_reader_->GetTuples(*it);
      
      // Get document ids.
      vector<int> docs;
      for (vector<Tuple>::iterator j = tuples.begin(); j != tuples.end(); ++j) {
        docs.push_back(j->document_id);
      }
      
      if (tuples.size() == 0 && op_and) {
        result.clear();
      } else {
        if (op_and)
          result = DocFinder::OperatorAnd(result, docs);
        else if (op_or)
          result = DocFinder::OperatorOr(result, docs);
        else {
          if(result.size() == 0)
            result = docs;
          else
            result = DocFinder::OperatorOr(result, docs);
        }
      }
      
      op_and = false;
      op_or = false;
    }
  }

  return DocFinder::UniqueDocs(result);
}

void DocFinder::PrintDoc(int id) {
  fseeko(file_, 3, SEEK_SET);

  int doc_id = 0, pipes = 0;
  std::string content; 
  char buffer[1024];
  size_t num_read = 0; 
  while ((num_read = fread(buffer, sizeof(char), 1024, file_)) != 0) {
    for (size_t i = 0; i < num_read; ++i) {
      if (buffer[i] == '|') {
        ++pipes;
        continue;
      }

      if (pipes == 1) { // End of url.
        content = std::string();
      } else if (pipes == 3) { // End of document content.
        if (doc_id == id) { 
          std::cout << Extractor::GetCleanText(content);
          return;
        }
        doc_id++;
        content = std::string();
      }
      content += buffer[i];
      pipes = 0;
    }
  }
}

std::string DocFinder::GetUrl(int id) {
  return url_map_[id].url;
}

void DocFinder::InsertDocument(Document doc) {
  url_map_.insert({ ++num_docs_, doc });
}
