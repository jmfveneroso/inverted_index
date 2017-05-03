/**
 * Tuple struct
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __TUPLE_H__
#define __TUPLE_H__

#include <iostream>

namespace TP1 {

struct Tuple {
  int lexeme_id;
  int document_id;
  int word_position;

  bool operator< (const Tuple &) const;
  bool operator> (const Tuple &) const;
  bool operator== (const Tuple &) const;
    
  Tuple();
  Tuple(int lexeme_id, int document_id, int word_position);
  Tuple(const Tuple& tuple);
};

} // Namespace TP1.

#endif
