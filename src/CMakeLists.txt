include_directories(${CMAKE_SOURCE_DIR}/lib)
link_directories(${CMAKE_SOURCE_DIR}/lib/gumbo_parser/lib)

set(SRC 
  inverted_index.cpp ranker.cpp extractor.cpp
  doc_collection.cpp lexicon.cpp tuple_sorter.cpp
  doc_map.cpp tuple.cpp 
)

# Make a library to generalize the inverted index.
add_library(search ${SRC})
target_link_libraries(search libgumbo.a)

# Make the search executable
add_executable(search_engine main.cpp)
target_link_libraries(search_engine search)
