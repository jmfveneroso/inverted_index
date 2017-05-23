#ifndef _BIT_BUFFER_HPP_
#define _BIT_BUFFER_HPP_

#include <map>
#include <iostream>
#include <vector>
#include <limits.h>
#include <math.h>

namespace InvertedIndex {

class BitBuffer {
  std::vector<unsigned char> bytes;
  unsigned int written_bits = 0;
  unsigned int read_cursor = 0;

  void WriteBit(bool bit) {
    unsigned char bit_offset = written_bits++ % 8;
    if (bit_offset == 0) bytes.push_back(0); 
    if (bit == 1) bytes.back() |= (1 << (7 - bit_offset));
  }

  void WriteUnary(unsigned int num) {
    for (unsigned int i = 0; i < num - 1; ++i) WriteBit(1);
    WriteBit(0);
  }

  void WriteRemainder(unsigned int num_bits, unsigned int num) {
    if (num_bits == 0) return;
    for (unsigned int mask = 1 << (num_bits - 1); mask > 0; mask >>= 1)
      WriteBit(num & mask);
  }

  void WriteEliasGamma(unsigned int num) {
    unsigned int base = log2(num) + 1;
    WriteUnary(base);
    WriteRemainder(base - 1, num);   
  }

  void WriteEliasDelta(unsigned int num) {
    unsigned int base = log2(num) + 1;
    WriteEliasGamma(base);
    WriteRemainder(base - 1, num);   
  }

  bool ReadBit() {
    unsigned char bit_offset = read_cursor % 8;
    return bytes[read_cursor++ / 8] & (1 << (7 - bit_offset));
  }

  unsigned int ReadUnary() {
    unsigned int unary = 1;
    while (ReadBit()) ++unary;
    return unary;
  }

  void ReadRemainder(unsigned int num_bits, unsigned int* num) {
    for (unsigned int mask = 1 << num_bits; mask > 0; mask >>= 1)
      if (ReadBit()) *num |= mask;
  }

  unsigned int ReadEliasGamma() {
    unsigned int base = ReadUnary() - 1;
    if (base == 0) return 1;

    unsigned int num = 1 << base;
    ReadRemainder(base - 1, &num);
    return num;
  }

  unsigned int ReadEliasDelta() {
    unsigned int base = ReadEliasGamma() - 1;
    if (base == 0) return 1;

    unsigned int num = 1 << base;
    ReadRemainder(base - 1, &num);
    return num;
  }

 public:
  void WriteInt(unsigned int num) {
    if (num == 0) throw new std::runtime_error("Zero is not compressible with elias delta.");
    WriteEliasDelta(num);
  }

  void WriteByte(unsigned char byte) {
    WriteRemainder(8, byte);
    written_bits += 8;
  }

  unsigned int ReadInt() {
    return ReadEliasDelta(); 
  }

  unsigned char ReadByte(size_t i) {
    return bytes[i]; 
  }

  size_t Size() {
    return bytes.size();
  }

  void PrintBits() {
    for (read_cursor = 0; read_cursor < written_bits;) {
      std::cout << (ReadBit() ? "1" : "0");
      if (read_cursor % 8 == 0) std::cout << " ";
    }
    std::cout << std::endl;
  }
};

} // End of namespace.

#endif
