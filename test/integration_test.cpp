#include <iostream>
#include <string>
#include <cstdio>
#include <vector>

#include "gtest/gtest.h"

#include "inverted_file_writer.h"
#include "doc_finder.h"

using namespace std;
using namespace TP1;

// TEST(IntegrationTest, Write) {
//   // Test Collection.
//   string input_directory("../../data/toyExample");
//   string index_filename("indexToCompressedColection.txt");
// 
//   RICPNS::CollectionReader collection_reader(input_directory, index_filename);
//   
//   std::shared_ptr<ILexicon> lexicon = std::make_shared<Lexicon>();
//   std::shared_ptr<ITupleSorter> tuple_sorter = std::make_shared<TupleSorter>(lexicon);
//   InvertedFileWriter inverted_file_writer(lexicon, tuple_sorter, "integration_inverted_file_test.dat");
//   
//   // Write inverted file.
//   inverted_file_writer.WriteFromCollection(&collection_reader);
//   
//   FILE *file = fopen("integration_inverted_file_test.dat", "rb");
//   int num_documents, num_lexemes, num_tuples;
//   
//   fread(&num_documents, sizeof(int), 1, file);
//   fread(&num_lexemes, sizeof(int), 1, file);
//   fread(&num_tuples, sizeof(int), 1, file);
//   
//   for (int i = 0; i < num_lexemes; i++) {
//     unsigned short str_size;
//     char lexeme_str[MAX_LEXEME_LENGTH];
//     fread(&str_size, sizeof(unsigned short), 1, file);
//     fread(&lexeme_str, sizeof(char), str_size, file);
//   }
//   
//   for (int i = 0; i < num_lexemes; i++) {
//     int offset;
//     fread(&offset, sizeof(int), 1, file);
//   }
//   
//   for (int i = 0; i < num_tuples; i++) {
//     Tuple t;
//     fread(&t, sizeof(Tuple), 1, file);
//   }
//   
//   char blank;
//   EXPECT_EQ(0, fread(&blank, sizeof(char), 1, file));
//   
//   fclose(file);
//   
//   // Ranking test.
//   Lexicon lexicon_2;
//   InvertedFileReader inverted_file_reader(&lexicon_2, "integration_inverted_file_test.dat");
// }
// 
// TEST(IntegrationTest, Read) {
//   string input_directory("../../data/toyExample");
//   string index_filename("indexToCompressedColection.txt");
//   
//   RICPNS::CollectionReader collection_reader(input_directory, index_filename);
//   
//   Lexicon lexicon;
//   InvertedFileReader inverted_file_reader(&lexicon, "integration_inverted_file_test.dat");
//   DocFinder doc_finder(&inverted_file_reader);
//   
//   vector<int> result = doc_finder.BooleanQuery("a");
//   
//   string urls[29];
//   urls[0] = "http://www.glix.com.br/Educacao/Instituto-Denver-Cursos-dist-ncia-l12320.html";
//   urls[1] = "http://www.geocities.com/tadeu1500/SHM/index_shm.html";
//   urls[2] = "http://subversiva.nuvempimenta.org/";
//   urls[3] = "http://www.okna.com.br/conteudo.php?pagina=4&amp;contID=126";
//   urls[4] = "http://www.resolucaografica.com.br/equipe.htm";
//   urls[5] = "http://numeidumato.zip.net/arch2005-12-11_2005-12-17.html";
//   urls[6] = "http://www.mrjogos.com.br/games-gratis-online/tiro/the-professinals/";
//   urls[7] = "http://projetoatitude.googlepages.com/";
//   urls[8] = "http://esportes.terra.com.br/tenis/brasilopen/interna/0,,OI143097-EI1779,00.html";
//   urls[9] = "http://www.pousadaondaazul.com.br/";
//   urls[10] = "http://www.centraleconcursos.com.br/oque-fazemos.php";
//   urls[11] = "http://www.mrjogos.com.br/games-gratis-online/tiro/tiro-para-o-ouro/";
//   urls[12] = "http://diegoquirino.googlepages.com/";
//   urls[13] = "http://tutomania.com.br/file.php?cod=14782";
//   urls[14] = "http://tutomania.com.br/file.php?cod=20940";
//   urls[15] = "http://tutomania.com.br/file.php?cod=21676";
//   urls[16] = "http://tutomania.com.br/file.php?cod=21809";
//   urls[17] = "http://criticanarede.com/welcome.htm";
//   urls[18] = "http://www.northwestern.edu/newscenter/stories/2009/02/synapses.html";
//   urls[19] = "http://www.toluna.fr/Vous_avez_essaye_avec_des_agraffes-av-751342.html";
//   urls[20] = "http://www.usabilityprofessionals.org/upa_publications/upa_voice/volumes/2007/august/brazil.html";
//   urls[21] = "http://ccsc.wikidot.com/";
//   urls[22] = "http://www.capoeira.jex.com.br/";
//   urls[23] = "http://www.provenatdobrasil.com.br/";
//   urls[24] = "http://br.geocities.com/moisescsilva/links.html";
//   urls[25] = "http://www.znimovel.com.br/simule.asp";
//   urls[26] = "http://www.crea-al.org.br/links.php";
//   urls[27] = "http://www.jucelinosousa.com.br/ultra-compra-texaco/";
//   urls[28] = "http://www.foxgsm.net/forums/smartphone-palm-htc-qtek-e-blackberry/63188-iphone-2.html";
//   
//   for (int i = 0; i < 29; i++) {
//     RICPNS::Document doc;
//     collection_reader.getDocumentById(result[i], doc);
//     EXPECT_EQ(urls[i], doc.getURL());
//     doc.clear();
//   }
// }
