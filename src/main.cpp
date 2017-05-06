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

      Injector::Instance()->lexicon()->LoadFromFile(argv[3]);
      Injector::Instance()->lexicon()->Print();
      return 0;
    }

    if (what == "inverted-index") {
      if (argc < 4) {
        std::cout << "usage: search read inverted-index <filename>" << endl;
        return 1;
      }
      Injector::Instance()->inverted_file_reader()->LoadFromFile(argv[3]);
      Injector::Instance()->inverted_file_reader()->Print();
      return 0;
    }

    if (what == "document") {
      if (argc < 5) {
        std::cout << "usage: search read document <filename> <id>" << endl;
        return 1;
      }
      Injector::Instance()->doc_finder()->LoadFromFile(argv[3]);
      Injector::Instance()->doc_finder()->PrintDoc(atoi(argv[4]));
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

      Injector::Instance()->extractor()->Extract(argv[3], argv[4]);
      Injector::Instance()->lexicon()->WriteToFile(argv[5]);
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
      if (argc < 6) {
        std::cout << "usage: search run inverted-index <tuple_file> <lexicon_file> <out_file>" << endl;
        return 1;
      }
      Injector::Instance()->lexicon()->LoadFromFile(argv[4]);
      Injector::Instance()->inverted_file_writer()->Write(argv[3], argv[5]);
      return 0;
    }
    if (what == "boolean") {
      if (argc < 5) {
        std::cout << "usage: search run boolean <documents-file> <inverted-index>" << endl;
        return 1;
      }
      Injector::Instance()->doc_finder()->LoadFromFile(argv[3]);
      Injector::Instance()->inverted_file_reader()->LoadFromFile(argv[4]);

      while (true) {
        std::cout << "Query >> ";
        std::string input;
        getline(cin, input);
        if (input == "exit") break;
        vector<int> result = Injector::Instance()->doc_finder()->BooleanQuery(input);
        if (result.size() == 0) std::cout << "No results found." << std::endl;
        for (auto doc_id : result) {
          std::cout << doc_id << ": " << Injector::Instance()->doc_finder()->GetUrl(doc_id) << std::endl;
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
  std::cout << "    search run boolean <documents-file> <inverted-index>" << endl;
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
