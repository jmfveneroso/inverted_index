include_directories(${CMAKE_SOURCE_DIR}/lib)
link_directories(${CMAKE_SOURCE_DIR}/lib/gumbo_parser/lib)

set(SRC 
  inverted_index.cpp extractor.cpp
  doc_collection.cpp lexicon.cpp tuple_sorter.cpp
  doc_map.cpp tuple.cpp 
)

# Make a library to generalize the inverted index.
add_library(tp1_lib ${SRC})
target_link_libraries(tp1_lib libgumbo.a)

# Make the search executable
add_executable(search main.cpp)
target_link_libraries(search tp1_lib)
