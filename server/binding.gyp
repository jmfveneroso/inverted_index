{
  "targets": [
    {
      "target_name": "binding",
      "cflags": ["-Wall", "-Werror", "-fPIC", "-pedantic", "-std=c++11"],
      "sources": [ "binding.cc" ],
      "include_dirs": [
        "../include", 
        "../lib/gumbo_parser/include", 
        "../lib"
      ],
      "libraries": [
        "/home/joao/Code/inverted_index/build/src/libsearch.a",
        "/home/joao/Code/inverted_index/lib/gumbo_parser/lib/libgumbo.so"
      ]
    }
  ]
}
