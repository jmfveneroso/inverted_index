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

    double vector_rank_w = 1;
    double anchor_rank_w = 0;
    double page_rank_w = 0;
    unsigned int page_num = 1;
    size_t results_per_page = 10;
    size_t mode = 0;
    while (true) {
      std::cout << "Query >> ";
      std::string input;
      getline(cin, input);
      if (input == "exit") break;
      if (input == "quit") break;
      if (input == "anchor")   { mode = 3; std::cout << "Anchor rank mode." << std::endl; continue; }
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

      if (input.find("vectorrank ") == 0) { 
        vector_rank_w = std::atof(input.substr(11).c_str());
        std::cout << "Vector rank weight: " << vector_rank_w << std::endl; 
        continue;
      }

      if (input.find("anchorrank ") == 0) { 
        anchor_rank_w = std::atof(input.substr(11).c_str());
        std::cout << "Anchor rank weight: " << anchor_rank_w << std::endl; 
        continue;
      }

      if (input.find("pagerank ") == 0) { 
        page_rank_w = std::atof(input.substr(9).c_str());
        std::cout << "Page rank weight: " << page_rank_w << std::endl; 
        continue;
      }

      if (input.find("resultsperpage ") == 0) { 
        results_per_page = std::atoi(input.substr(15).c_str());
        std::cout << "Results per page: " << results_per_page << std::endl; 
        continue;
      }

      std::vector<QueryResult> results;
      if (input.find("page ") == 0) { 
        page_num = std::atoi(input.substr(5).c_str());
      } else {
        Injector::Instance()->ranker()->Search(input, vector_rank_w, anchor_rank_w, page_rank_w);
        page_num = 1; 
      }
      results = Injector::Instance()->ranker()->GetPageOfResults(page_num, results_per_page);

      size_t num_results = Injector::Instance()->ranker()->GetNumResults();
      std::cout << "Page " << page_num << " of " << num_results << " results" << std::endl;
      switch (mode) {
        case 0: {
          for (auto r : results) {
            std::cout << r.rank << ": (" << r.id << ") " << r.title << " [" << r.url << "]" << std::endl;
            std::cout << "Vector rank: " << r.vector_rank << ", page rank: " << r.page_rank << ", anchor rank: " << r.anchor_rank << std::endl;

            bool comma = 0;
            std::cout << "Word positions: ";
            for (auto pos : r.positions) std::cout << (comma++ ? ", " : "") << pos;
            std::cout << std::endl;

            std::cout << r.short_text << std::endl;
            std::cout << std::endl;
          }
          // vector<unsigned int> result = Injector::Instance()->ranker()->BooleanQuery(input);
          break;
        }
        case 3: {
          auto ranked_docs = Injector::Instance()->inverted_index()->GetAnchorRank(input);
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
