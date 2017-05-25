#include <v8.h>
#include <node.h>
#include <string>
#include <vector>
#include <injector.hpp>

using namespace InvertedIndex;

struct QueryResult {
 public:
  float rank;
  std::string url;
  std::string title;
  std::string short_text;

  QueryResult(
    float rank, std::string url, 
    std::string title, std::string short_text
  ) : rank(rank), url(url), title(title), short_text(short_text) {}
};

v8::Local<v8::Object> PackQueryResult(v8::Isolate* isolate, QueryResult& result) {
  v8::Local<v8::Object> obj = v8::Object::New(isolate);
  obj->Set(v8::String::NewFromUtf8(isolate, "rank"), v8::Number::New(isolate, result.rank));
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

  bool anchor_rank = args[1]->BooleanValue();
  bool page_rank = args[2]->BooleanValue();

  v8::Local<v8::Array> arr = v8::Array::New(isolate);

  std::vector<QueryResult> results;
  results.push_back(QueryResult(2.5, "aaa", "Johnny", "blablabla"));
  results.push_back(QueryResult(12.5, "bbb", "Orleans", "bliblibli"));

  for (unsigned int i = 0; i < results.size(); i++) {
    v8::Local<v8::Object> obj = PackQueryResult(isolate, results[i]);
    arr->Set(i, obj);
  }

  args.GetReturnValue().Set(arr);
}

void init(v8::Local<v8::Object> exports) {
  NODE_SET_METHOD(exports, "loadDocCollection", LoadDocCollection);
  NODE_SET_METHOD(exports, "loadInvertedIndex", LoadInvertedIndex);
  NODE_SET_METHOD(exports, "search", Search);
}

NODE_MODULE(binding, init)
