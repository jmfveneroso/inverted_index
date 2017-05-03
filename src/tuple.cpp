#include "tuple.h"

using namespace std;
using namespace TP1;

Tuple::Tuple () :
  lexeme_id(0),
  document_id(0),
  word_position(0) {}

Tuple::Tuple (int lexeme_id, int document_id, int word_position) :
  lexeme_id(lexeme_id),
  document_id(document_id),
  word_position(word_position) {}

Tuple::Tuple (const Tuple& tuple) :
  lexeme_id(tuple.lexeme_id),
  document_id(tuple.document_id),
  word_position(tuple.word_position) {}

bool TP1::Tuple::operator== (const Tuple &m) const {
  if(lexeme_id == m.lexeme_id && document_id == m.document_id && word_position == m.word_position)
    return true;
  else
    return false;  
}

bool TP1::Tuple::operator< (const Tuple &m) const {
  if(lexeme_id != m.lexeme_id)
    return lexeme_id < m.lexeme_id;
  else if (document_id != m.document_id)
    return document_id < m.document_id;
  else if (word_position != m.word_position)
    return word_position < m.word_position;
  else
    return false;  
}

bool TP1::Tuple::operator> (const Tuple &m) const {
  if(lexeme_id != m.lexeme_id)
    return lexeme_id > m.lexeme_id;
  else if (document_id != m.document_id)
    return document_id > m.document_id;
  else if (word_position != m.word_position)
    return word_position > m.word_position;
  else
    return false;  
}
