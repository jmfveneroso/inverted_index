#include <iostream>
#include "injector.hpp"

int CommandRead(int argc, char* argv[]) {
  if (argc >= 3) {
    std::string what = argv[2];
    if (what == "tuples") {
      if (argc < 4) {
        std::cout << "usage: search read tuples <filename>" << endl;
        return 1;
      }

      Injector::Instance()->extractor()->Print(argv[3]);
      return 0;
    }

    if (what == "lexicon") {
      if (argc < 4) {
        std::cout << "usage: search read lexicon <filename>" << endl;
        return 1;
      }

      // Injector::Instance()->lexicon()->LoadFromFile(argv[3]);
      // Injector::Instance()->lexicon()->Print();
      return 0;
    }

    if (what == "inverted-index") {
      if (argc < 4) {
        std::cout << "usage: search read inverted-index <filename>" << endl;
        return 1;
      }
      Injector::Instance()->inverted_index()->Load(argv[3]);
      return 0;
    }

    if (what == "document") {
      if (argc < 5) {
        std::cout << "usage: search read document <filename> <id>" << endl;
        return 1;
      }
      // Injector::Instance()->doc_finder()->LoadFromFile(argv[3]);
      // Injector::Instance()->doc_finder()->PrintDoc(atoi(argv[4]));
      return 0;
    }
  }

  std::cout << "usage: search read <command>" << endl;
  std::cout << "Available commands:" << endl;
  std::cout << "    search read tuples <filename>" << endl;
  std::cout << "    search read lexicon <filename>" << endl;
  std::cout << "    search read inverted-index <filename>" << endl;
  std::cout << "    search read document <filename> <id>" << endl;
  return 1;
}

int CommandRun(int argc, char* argv[]) {
  if (argc >= 3) {
    std::string what = argv[2];
    if (what == "extractor") {
      if (argc < 6) {
        std::cout << "usage: search run extractor <in_file> <out_file> <lexicon>" << endl;
        return 1;
      }

      // Injector::Instance()->extractor()->Extract(argv[3], argv[4]);
      // Injector::Instance()->lexicon()->WriteToFile(argv[5]);
      return 0;
    }
    if (what == "doc-collection") {
      if (argc < 4) {
        std::cout << "usage: search run doc-collection <directory>" << endl;
        return 1;
      }
      Injector::Instance()->doc_collection()->Init(argv[3]);
      RawDocument doc;
      while (Injector::Instance()->doc_collection()->GetNextDoc(&doc)) {
        std::cout << doc.url << std::endl;
      }

      return 0;
    }
    if (what == "inverted-index") {
      if (argc < 5) {
        std::cout << "usage: search run inverted-index <collection_dir> <out_file>" << endl;
        return 1;
      }
      Injector::Instance()->inverted_index()->CreateIndexForCollection(argv[3], argv[4]);
      return 0;
    }
    if (what == "boolean") {
      if (argc < 5) {
        std::cout << "usage: search run boolean <documents-directory> <inverted-index>" << endl;
        return 1;
      }
      Injector::Instance()->doc_collection()->Init(argv[3]);
      Injector::Instance()->inverted_index()->Load(argv[4]);

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
          Document doc = Injector::Instance()->doc_map()->GetDocById(doc_id);
          Injector::Instance()->doc_collection()->Read(doc.file_num, doc.offset);
          continue;
        }

        switch (mode) {
          case 0: {
            vector<unsigned int> result = Injector::Instance()->boolean_model()->BooleanQuery(input);
            if (result.size() == 0) std::cout << "No results found." << std::endl;
            for (auto doc_id : result) {
              Document doc = Injector::Instance()->doc_map()->GetDocById(doc_id);
              std::cout << doc_id << ": " << doc.url << std::endl;
            }
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
  }

  std::cout << "usage: search run <command>" << endl;
  std::cout << "Available commands:" << endl;
  std::cout << "    search run extractor <in_file> <out_file> <lexicon>" << endl;
  std::cout << "    search run doc-collection <directory>" << endl;
  std::cout << "    search run inverted-index <tuple_file> <lexicon_file> <out_file>" << endl;
  std::cout << "    search run boolean <documents-directory> <inverted-index>" << endl;
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc >= 2) {  
    string command = argv[1];
    if (command == "run")  return CommandRun(argc, argv);
    if (command == "read") return CommandRead(argc, argv);
  }

  std::cout << "usage: search <command> [parameters]" << endl;
  std::cout << "Available commands: " << endl;
  std::cout << "   read" << endl;
  std::cout << "   run" << endl;
}
