#include "doc_map.hpp"
#include "extractor.hpp"
#include "inverted_index.hpp"
#include "ranker.hpp"
#include "evaluator.hpp"

namespace InvertedIndex {

class Injector {
 static Injector* instance_;
 std::shared_ptr<ILogger> logger_;
 std::shared_ptr<ILexicon> lexicon_;
 std::shared_ptr<IDocCollection> doc_collection_;
 std::shared_ptr<DocMap> doc_map_;
 std::shared_ptr<ITupleSorter> tuple_sorter_;
 std::shared_ptr<Extractor> extractor_;
 std::shared_ptr<IInvertedIndex> inverted_index_;
 std::shared_ptr<Ranker> ranker_;
 std::shared_ptr<Evaluator> evaluator_;

 public:
  Injector() 
    : logger_(std::make_shared<Logger>()),
      lexicon_(std::make_shared<Lexicon>(logger_)),
      doc_collection_(std::make_shared<DocCollection>(logger_)),
      doc_map_(std::make_shared<DocMap>(logger_)),
      tuple_sorter_(std::make_shared<TupleSorter>(logger_, lexicon_)),
      extractor_(std::make_shared<Extractor>(logger_, lexicon_, doc_map_, tuple_sorter_, doc_collection_)),
      inverted_index_(std::make_shared<InvertedIndex>(logger_, doc_collection_, doc_map_, lexicon_, extractor_, tuple_sorter_)),
      ranker_(std::make_shared<Ranker>(inverted_index_, doc_map_, lexicon_, extractor_)),
      evaluator_(std::make_shared<Evaluator>(ranker_, doc_map_)) {
  }

  static Injector* Instance() {
    if (!instance_) instance_ = new Injector;
    return instance_;
  }

  std::shared_ptr<ILogger> logger() { return logger_; }
  std::shared_ptr<ILexicon> lexicon() { return lexicon_; }
  std::shared_ptr<IDocCollection> doc_collection() { return doc_collection_; }
  std::shared_ptr<DocMap> doc_map() { return doc_map_; }
  std::shared_ptr<ITupleSorter> tuple_sorter() { return tuple_sorter_; }
  std::shared_ptr<Extractor> extractor() { return extractor_; }
  std::shared_ptr<IInvertedIndex> inverted_index() { return inverted_index_; }
  std::shared_ptr<Ranker> ranker() { return ranker_; }
  std::shared_ptr<Evaluator> evaluator() { return evaluator_; }
};

Injector* Injector::instance_ = 0;

} // End of namespace.
