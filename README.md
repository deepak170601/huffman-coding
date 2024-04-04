# Huffman Coding Implementation

This project demonstrates Huffman coding, a popular algorithm used for lossless data compression. Huffman coding assigns variable-length codes to input characters, with shorter codes assigned to more frequent characters, resulting in efficient compression.

## Overview

The implementation consists of the following components:

1. `main.cpp`: The main program that compresses and decompresses files using Huffman coding.
2. `huffman.h` and `huffman.cpp`: Header and source files containing functions for building Huffman trees, generating Huffman codes, encoding, and decoding files.
3. Input files: Sample input files for testing compression.

## Usage

### Compilation
To compile the program, use the following command:

```bash
g++ main.cpp huffman.cpp -o huffman
