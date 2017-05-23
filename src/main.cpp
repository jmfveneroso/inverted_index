#include <iostream>
#include "injector.hpp"

using namespace std;
using namespace InvertedIndex;

int CommandRun(int argc, char* argv[]) {
  std::string what = argv[1];
  if (what == "extractor") {
    if (argc < 4) {
      std::cout << "usage: search extractor <collection_dir> <out_file>" << endl;
      return 1;
    }
    Injector::Instance()->inverted_index()->Extract(argv[2], argv[3]);
    return 0;
  }

  if (what == "inverted-index") {
    if (argc < 4) {
      std::cout << "usage: search inverted-index <collection_dir> <out_file>" << endl;
      return 1;
    }
    Injector::Instance()->inverted_index()->CreateIndexForCollection(argv[2], argv[3]);
    return 0;
  }

  if (what == "sorter") {
    if (argc < 3) {
      std::cout << "usage: search sorter <out_file>" << endl;
      return 1;
    }
    Injector::Instance()->inverted_index()->Sort(argv[2]);
    return 0;
  }

  if (what == "anchor") {
    if (argc < 4) {
      std::cout << "usage: search anchor <collection_dir> <out_file>" << endl;
      return 1;
    }
    Injector::Instance()->inverted_index()->CreateAnchorIndex(argv[2], argv[3]);
    return 0;
  }

  if (what == "load") {
    if (argc < 4) {
      std::cout << "usage: search load <documents-directory> <inverted-index>" << endl;
      return 1;
    }
    Injector::Instance()->doc_collection()->Init(argv[2]);
    Injector::Instance()->inverted_index()->Load(argv[3]);

    size_t mode = 0;
    while (true) {
      std::cout << "Query >> ";
      std::string input;
      getline(cin, input);
      if (input == "exit") break;
      if (input == "boolean") { mode = 0; std::cout << "Boolean mode." << std::endl; continue; }
      if (input == "vector") { mode = 1; std::cout << "Vector mode." << std::endl; continue; }
      if (input == "pagerank") { mode = 2; std::cout << "Page rank mode." << std::endl; continue; }
      if (input.find("read ") == 0) { 
        unsigned int doc_id = std::atoi(input.substr(5).c_str());
        Injector::Instance()->extractor()->ReadDoc(doc_id);
        continue;
      }

      if (input.find("lexeme ") == 0) { 
        unsigned int lexeme_id = std::atoi(input.substr(7).c_str());
        std::cout << Injector::Instance()->lexicon()->GetLexemeById(lexeme_id).lexeme << std::endl;
        continue;
      }

      switch (mode) {
        case 0: {
          vector<unsigned int> result = Injector::Instance()->boolean_model()->BooleanQuery(input);
          break;
        }
        case 1: {
          std::vector< std::pair<unsigned int, double> > ranked_docs;
          ranked_docs = Injector::Instance()->boolean_model()->RankByVectorModel(input);
          if (ranked_docs.size() == 0) std::cout << "No results found." << std::endl;
          for (auto ranked_doc : ranked_docs) {
            Document doc = Injector::Instance()->doc_map()->GetDocById(ranked_doc.first);
            std::cout << ranked_doc.second << ": " << doc.url << std::endl;
          }
          break;
        }
        case 2: {
          std::vector< std::pair<unsigned int, double> > ranked_docs;
          ranked_docs = Injector::Instance()->boolean_model()->RankByPageRank(input);
          if (ranked_docs.size() == 0) std::cout << "No results found." << std::endl;
          for (auto ranked_doc : ranked_docs) {
            Document doc = Injector::Instance()->doc_map()->GetDocById(ranked_doc.first);
            std::cout << ranked_doc.second << ": " << doc.url << std::endl;
          }
          break;
        }
        default: break;
      }
    }
    return 0;
  }
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc >= 2) {  
    if (CommandRun(argc, argv) == 0) return 0;
  }

  std::cout << "usage: search <command>" << endl;
  std::cout << "Available commands:" << endl;
  std::cout << "    search extractor <documents-directory> <out_file>" << endl;
  std::cout << "    search sorter <out_file>" << endl;
  std::cout << "    search anchor <documents-directory> <out_file>" << endl;
  std::cout << "    search inverted-index <documents-directory> <out_file>" << endl;
  std::cout << "    search load <documents-directory> <inverted-index>" << endl;
}
