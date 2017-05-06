include_directories(${CMAKE_SOURCE_DIR}/lib)
link_directories(${CMAKE_SOURCE_DIR}/lib/gumbo_parser/lib)

# Make a library to generalize the inverted index.
add_library(tp1_lib doc_finder.cpp extractor.cpp inverted_file_writer.cpp inverted_file_reader.cpp tuple_sorter.cpp doc_collection.cpp lexicon.cpp tuple.cpp)
target_link_libraries(tp1_lib libgumbo.a)

# Make the search executable
add_executable(search main.cpp)
target_link_libraries(search tp1_lib)
