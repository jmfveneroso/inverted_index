#include "inverted_file_reader.h"
#include "inverted_file_writer.h"
#include "doc_finder.h"
#include "extractor.hpp"

using namespace std;
using namespace TP1;

class Injector {
 static Injector* instance_;
 std::shared_ptr<ILogger> logger_;
 std::shared_ptr<ILexicon> lexicon_;
 std::shared_ptr<IDocCollection> doc_collection_;
 std::shared_ptr<IInvertedFileReader> inverted_file_reader_;
 std::shared_ptr<IDocFinder> doc_finder_;
 std::shared_ptr<Extractor> extractor_;
 std::shared_ptr<ITupleSorter> tuple_sorter_;
 std::shared_ptr<InvertedFileWriter> inverted_file_writer_;

 public:
  Injector() 
    : logger_(std::make_shared<Logger>()),
      lexicon_(std::make_shared<Lexicon>(logger_)),
      doc_collection_(std::make_shared<DocCollection>(logger_)),
      inverted_file_reader_(std::make_shared<InvertedFileReader>(lexicon_)),
      doc_finder_(std::make_shared<DocFinder>(inverted_file_reader_)),
      extractor_(std::make_shared<Extractor>(logger_, lexicon_, doc_finder_)),
      tuple_sorter_(std::make_shared<TupleSorter>(lexicon_)),
      inverted_file_writer_(std::make_shared<InvertedFileWriter>(lexicon_, tuple_sorter_)) {
  }

  static Injector* Instance() {
    if (!instance_) instance_ = new Injector;
    return instance_;
  }
  std::shared_ptr<ILogger> logger() { return logger_; }
  std::shared_ptr<ILexicon> lexicon() { return lexicon_; }
  std::shared_ptr<IDocCollection> doc_collection() { return doc_collection_; }
  std::shared_ptr<ITupleSorter> tuple_sorter() { return tuple_sorter_; }
  std::shared_ptr<Extractor> extractor() { return extractor_; }
  std::shared_ptr<InvertedFileWriter> inverted_file_writer() { return inverted_file_writer_; }
  std::shared_ptr<IInvertedFileReader> inverted_file_reader() { return inverted_file_reader_; }
  std::shared_ptr<IDocFinder> doc_finder() { return doc_finder_; }
};

Injector* Injector::instance_ = 0;
