#ifndef __TUPLE_HPP__
#define __TUPLE_HPP__

#include <iostream>

namespace InvertedIndex {

struct Tuple {
  unsigned int lexeme_id;
  unsigned int document_id;
  unsigned int word_position;

  bool operator< (const Tuple &) const;
  bool operator> (const Tuple &) const;
  bool operator== (const Tuple &) const;
    
  Tuple();
  Tuple(unsigned int lexeme_id, unsigned int document_id, unsigned int word_position);
  Tuple(const Tuple& tuple);
};

} // Namespace TP1.

#endif
