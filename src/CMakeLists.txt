# Make a library to generalize the inverted index.
add_library(tp1_lib doc_finder.cpp extractor.cpp inverted_file_writer.cpp inverted_file_reader.cpp tuple_sorter.cpp lexicon.cpp tuple.cpp)
target_link_libraries(tp1_lib gumbo)

# Make the search executable
add_executable(search main.cpp)
target_link_libraries(search tp1_lib)
