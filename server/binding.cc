#include <v8.h>
#include <node.h>
#include <string>
#include <vector>
#include <injector.hpp>

using namespace InvertedIndex;

v8::Local<v8::Object> PackQueryResult(v8::Isolate* isolate, QueryResult& result) {
  v8::Local<v8::Object> obj = v8::Object::New(isolate);
  obj->Set(v8::String::NewFromUtf8(isolate, "id"), v8::Number::New(isolate, result.id));
  obj->Set(v8::String::NewFromUtf8(isolate, "rank"), v8::Number::New(isolate, result.rank));
  obj->Set(v8::String::NewFromUtf8(isolate, "vector_rank"), v8::Number::New(isolate, result.vector_rank));
  obj->Set(v8::String::NewFromUtf8(isolate, "anchor_rank"), v8::Number::New(isolate, result.anchor_rank));
  obj->Set(v8::String::NewFromUtf8(isolate, "page_rank"), v8::Number::New(isolate, result.page_rank));
  obj->Set(v8::String::NewFromUtf8(isolate, "url"), v8::String::NewFromUtf8(isolate, result.url.c_str()));
  obj->Set(v8::String::NewFromUtf8(isolate, "title"), v8::String::NewFromUtf8(isolate, result.title.c_str()));
  obj->Set(v8::String::NewFromUtf8(isolate, "shortText"), v8::String::NewFromUtf8(isolate, result.short_text.c_str()));
  return obj;
}

void LoadDocCollection(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::String::Utf8Value str(args[0]->ToString());
  std::string filename(*str);
  Injector::Instance()->doc_collection()->Init(filename);
}

void LoadInvertedIndex(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::String::Utf8Value str(args[0]->ToString());
  std::string filename(*str);
  Injector::Instance()->inverted_index()->Load(filename);
}

void Search(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope scope(isolate);

  v8::String::Utf8Value str(args[0]->ToString());
  std::string query(*str);

  double vector_rank_w = args[1]->NumberValue();
  double anchor_rank_w = args[2]->NumberValue();
  double page_rank_w = args[3]->NumberValue();

  Injector::Instance()->ranker()->Search(query, vector_rank_w, anchor_rank_w, page_rank_w);
}

void GetPage(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope scope(isolate);

  unsigned int page_num = args[0]->NumberValue();
  unsigned int results_per_page = args[1]->NumberValue();

  v8::Local<v8::Array> arr = v8::Array::New(isolate);

  std::vector<QueryResult> results;
  results = Injector::Instance()->ranker()->GetPageOfResults(page_num, results_per_page);

  for (unsigned int i = 0; i < results.size(); i++) {
    v8::Local<v8::Object> obj = PackQueryResult(isolate, results[i]);
    arr->Set(i, obj);
  }

  args.GetReturnValue().Set(arr);
}

void GetNumResults(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  size_t num_results = Injector::Instance()->ranker()->GetNumResults();
  args.GetReturnValue().Set(v8::Number::New(isolate, num_results));
}

void GetDocumentText(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();

  unsigned int doc_id = args[0]->NumberValue();
  std::string html = Injector::Instance()->extractor()->GetRawText(doc_id);
  args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, html.c_str()));
}

void init(v8::Local<v8::Object> exports) {
  NODE_SET_METHOD(exports, "loadDocCollection", LoadDocCollection);
  NODE_SET_METHOD(exports, "loadInvertedIndex", LoadInvertedIndex);
  NODE_SET_METHOD(exports, "search", Search);
  NODE_SET_METHOD(exports, "getPage", GetPage);
  NODE_SET_METHOD(exports, "getNumResults", GetNumResults);
  NODE_SET_METHOD(exports, "getDocumentText", GetDocumentText);
}

NODE_MODULE(binding, init)
