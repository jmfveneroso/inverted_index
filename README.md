## Inverted Index

This project implements an inverted index builder in C++.


### Compiling

To compile this project you have to install the cmake tool. You can get it from https://cmake.org/ or, in Ubuntu, you can install it by running:

```
$ sudo apt-get install cmake
```

After installing cmake. Run the following commands at the project's root directory:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

The executable will be written to "build/src/search".

### Running

**Creating the inverted index**

To create the inverted index, run:

```
$ ./search inverted-index <path/to/collection> <output_filename>
```

The program will write the inverted index for the collection specfied in the first argument to the file specified in the second argument.

**Running queries**

After creating the inverted index file you can load it and run queries by running the following command:

```
$ ./search load <path/to/collection> <inverted_index>
```

After that, the program may take a few seconds to load the dictionary and document map. And, after that the following prompt will appear:

```
$ Query >>
```

To run queries, just type any term after the >> sign and press enter. Results should look like this:

```
Query >> gumbo
Results for: gumbo 
Word: gumbo
(10687) pinterest.com/explore/love-this: 75
(11255) pinterest.com/pin/540220917773992684: 140, 142
(23542) otaku-cave.deviantart.com: 527
(98516) burnpit.us: 232

```

The number in parenthesis is the document id. To read document 10687 for example, type:

```
$ Query >> read 10687
```