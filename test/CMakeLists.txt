################################
# Testing
################################
if (test)
  enable_testing()

  # Include gtest directories
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
  
  # Include project directories
  include_directories(${COMMON_INCLUDES})

  ##############
  # Unit Tests
  ##############
   add_executable(lexiconTest lexicon_test.cpp)
   add_executable(tupleSorterTest tuple_sorter_test.cpp)
   add_executable(invertedFileWriterTest inverted_file_writer_test.cpp)
   add_executable(invertedFileReaderTest inverted_file_reader_test.cpp)
   add_executable(integrationTest integration_test.cpp)
   add_executable(docFinderTest doc_finder_test.cpp)

  # Standard linking to gtest stuff.
   target_link_libraries(lexiconTest gtest gtest_main)
   target_link_libraries(tupleSorterTest gtest gtest_main)
   target_link_libraries(invertedFileWriterTest gtest gtest_main)
   target_link_libraries(invertedFileReaderTest gtest gtest_main)
   target_link_libraries(integrationTest gtest gtest_main)
   target_link_libraries(docFinderTest gtest gtest_main)

  # Extra linking for the project.
   target_link_libraries(lexiconTest tp1_lib)
   target_link_libraries(tupleSorterTest tp1_lib)
   target_link_libraries(invertedFileWriterTest tp1_lib)
   target_link_libraries(invertedFileReaderTest tp1_lib)
   target_link_libraries(integrationTest tp1_lib)
   target_link_libraries(docFinderTest tp1_lib)

  # This is so you can do 'make test' to see all your tests run, instead of
  # manually running the executable runUnitTests to see those specific tests.
   add_test(NAME lexicon-test COMMAND lexiconTest)
   add_test(NAME tuple-sorter-test COMMAND tupleSorterTest)
   add_test(NAME inverted-file-writer-test COMMAND invertedFileWriterTest)
   add_test(NAME inverted-file-reader-test COMMAND invertedFileReaderTest)
   add_test(NAME integration-test COMMAND integrationTest)
   add_test(NAME doc-finder-test COMMAND docFinderTest)
endif()
