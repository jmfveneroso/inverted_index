/**
 * InvertedFile object
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __INVERTED_FILE_WRITER_H__
#define __INVERTED_FILE_WRITER_H__

#include <cstring>
#include <iostream>
#include <map>

#include "util.h"
#include "tuple.h"
#include "lexicon.h"
#include "tuple_sorter.h"

namespace TP1 {
    
    /**
     * Inverted File Format
     *
     * --------------------
     * Header
     * --------------------
     * int num_documents
     * int num_lexemes
     * int num_tuples
     *
     * --------------------
     * Body
     * --------------------
     * [lexemes]
     * [offsets]
     * [tuples]
     *
     */
    
    struct InvertedFileHeader {
        int num_documents;
        int num_lexemes;
        int num_tuples;

      InvertedFileHeader() {}
        InvertedFileHeader(int num_documents_, int num_lexemes_, int num_tuples_) {
          num_documents = num_documents_;
          num_lexemes = num_lexemes_;
          num_tuples = num_tuples_;
        }
    };
  
    class InvertedFileWriter {
      public:
        InvertedFileWriter(std::shared_ptr<ILexicon> lexicon,
                           std::shared_ptr<ITupleSorter> tuple_sorter);
        ~InvertedFileWriter() {}
      
        static void WriteFromSortedTuples(const string& tuples_filename, const string& lexicon_filename, const string& output_filename);
        void Write(const std::string&, const std::string&);
      
      private:
        std::shared_ptr<ILexicon> lexicon_;
        std::shared_ptr<ITupleSorter> tuple_sorter_;
      
        std::string filename_;
        FILE *output_file_;
      
        int num_documents_;
      
        void WriteHeader();
        void WriteLexicon();
        void WriteTuples();
    };
    
} // Namespace TP1.

#endif
