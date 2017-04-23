/**
 * TupleSorter object
 * @author: jmfveneroso@gmail.com.br
 */

#include "tuple_sorter.h"

using namespace std;
using namespace TP1;

TupleSorter::TupleSorter (std::shared_ptr<ILexicon> lexicon) 
  : lexicon_(lexicon), num_tuples_(0) {
  temp_file_ = tmpfile();
}

/**
 * Get clean text out of HTML.
 *
 */
string TupleSorter::CleanText (GumboNode* node) {
  if (node->type == GUMBO_NODE_TEXT) {
    return std::string(node->v.text.text);
  } else if (
    node->type == GUMBO_NODE_ELEMENT &&
    node->v.element.tag != GUMBO_TAG_SCRIPT &&
    node->v.element.tag != GUMBO_TAG_STYLE
  ) {
    std::string contents = "";
    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
      const std::string text = CleanText((GumboNode*) children->data[i]);
      if (i != 0 && !text.empty()) {
        contents.append(" ");
      }
      contents.append(text);
    }
    return contents;
  } else {
    return "";
  }
}

/**
 * Store tuples extracted from a document into an output file.
 *
 */
void TupleSorter::ExtractTuplesFromDoc (int doc_id, const string& document) {
  GumboOutput* output = gumbo_parse(document.c_str());
  
  string text = TupleSorter::CleanText(output->root);
  
  int word_count = 0;
  vector<string> lexemes = Lexicon::ExtractLexemes(text);
  for (vector<string>::iterator it = lexemes.begin(); it != lexemes.end(); it++) {
    int lexeme_id = lexicon_->AddLexeme(*it);
    Tuple t(lexeme_id, doc_id, ++word_count);
    fwrite(&t, sizeof(Tuple), 1, temp_file_);
  }
  
  gumbo_destroy_output(&kGumboDefaultOptions, output);
  num_tuples_ += lexemes.size();
}

bool wayToSort(Tuple t1, Tuple t2) {
  if (t1.lexeme_id != t2.lexeme_id)
    return t1.lexeme_id < t2.lexeme_id;
  else if (t1.document_id != t2.document_id)
    return t1.document_id < t2.document_id;
  else if (t1.word_position != t2.word_position)
    return t1.word_position < t2.word_position;
  else
    return true;
}

void TupleSorter::SortBlocks(const std::string& filename) {
  FILE* file = fopen(filename.c_str(), "rb+");

  int num_tuples = 0;
  Tuple t;
  while (fread(&t, sizeof(Tuple), 1, file) != 0) ++num_tuples;

  int num_blocks = (num_tuples / MAX_TUPLES) + ((num_tuples % MAX_TUPLES) ? 1 : 0);
  int padding = MAX_TUPLES - (num_tuples % MAX_TUPLES);

  fseeko(file, 0, SEEK_SET);
  // fseek(temp_file_, 0, SEEK_SET);
  
  startbar();
  vector<Tuple> tuples;
  // Read unsorted tuples from disk, sort them and write them back.
  for (int i = 0; i < num_blocks; i++) {
    for (int j = 0; j < MAX_TUPLES; j++) {
      if (fread(&t, sizeof(Tuple), 1, file) == 0) t = Tuple(0, 0, 0);
      tuples.push_back(t);
    }
    
    if (i == num_blocks - 1) {
      sort(tuples.begin(), tuples.end() - padding, less<Tuple>());
      fseek(file, -sizeof(Tuple) * (MAX_TUPLES - padding), SEEK_CUR);
    } else {
      sort(tuples.begin(), tuples.end(), less<Tuple>());
      fseek(file, -sizeof(Tuple) * MAX_TUPLES, SEEK_CUR);
    }
    
    for (int j = 0; j < MAX_TUPLES; j++)
      fwrite(&tuples[j], sizeof(Tuple), 1, file);
    
    tuples.clear();
    loadbar(i, num_blocks - 1);
  }
 
  fclose(file); 
  // fseek(temp_file_, 0, SEEK_SET);
  
  cout << endl;
}

/**
 * Read tuples from sorted blocks in disk filling a priority queue.
 *
 */
void TupleSorter::FillQueue () {
  int blocks_to_be_read = MAX_TUPLES / num_blocks_;
  blocks_to_be_read = (blocks_to_be_read != 0) ? blocks_to_be_read : 1;

  for (int i = 0; i < num_blocks_; i++) {
    if (tuple_blocks[i].tuples_in_memory == 0 && tuple_blocks[i].tuples_in_disk > 0) {
      fsetpos(temp_file_, &tuple_blocks[i].pos);
      
      int num_to_read = (tuple_blocks[i].tuples_in_disk > blocks_to_be_read) ? blocks_to_be_read : tuple_blocks[i].tuples_in_disk;
      
      int actually_read = fread(tuples_, sizeof(Tuple), num_to_read, temp_file_);
      
      tuple_blocks[i].tuples_in_disk -= actually_read;
      tuple_blocks[i].tuples_in_memory += actually_read;
      
      fgetpos(temp_file_, &tuple_blocks[i].pos);

      for (int j = 0; j < actually_read; j++) {
        if (tuples_[j].lexeme_id != 0 || tuples_[j].document_id != 0 || tuples_[j].word_position != 0) {
          tuple_queue_.push(MergeTuple(i, tuples_[j]));
        } else {
          tuple_blocks[i].tuples_in_memory--;
        }
      }
    }
  }
}

void TupleSorter::Sort(const std::string& filename) {
  temp_file_ = fopen(filename.c_str(), "rb+");

  num_tuples_ = 0;
  Tuple t;
  while (fread(&t, sizeof(Tuple), 1, temp_file_) != 0) { if (t.lexeme_id != 0) ++num_tuples_; }

  num_blocks_ = (num_tuples_ / MAX_TUPLES) + ((num_tuples_ % MAX_TUPLES) ? 1 : 0);
  if (num_blocks_ == 0) return;
  
  tuple_blocks = new TupleBlock[num_blocks_];
  
  fseeko(temp_file_, 0, SEEK_SET);
  
  for (int i = 0; i < num_blocks_; i++) {
    tuple_blocks[i].tuples_in_disk = MAX_TUPLES;
    tuple_blocks[i].tuples_in_memory = 0;
    
    fgetpos(temp_file_, &tuple_blocks[i].pos);
    fseek(temp_file_, MAX_TUPLES * sizeof(Tuple), SEEK_CUR);
  }
  
  // SortBlocks();
  
  int blocks_to_be_read = MAX_TUPLES / num_blocks_;
  blocks_to_be_read = (blocks_to_be_read != 0) ? blocks_to_be_read : 1;
  tuples_ = new Tuple[blocks_to_be_read];
  
  FillQueue();
}

/**
 * Pop the smallest tuple.
 *
 */
MergeTuple TupleSorter::PopTuple () {
  MergeTuple tuple = tuple_queue_.top();
  tuple_blocks[tuple.block_num].tuples_in_memory--;
  tuple_queue_.pop();
  FillQueue();
  return tuple;
}

void TupleSorter::Clear () {
  delete tuples_;
}
