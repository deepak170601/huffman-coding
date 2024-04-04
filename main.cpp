#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <bitset>

using namespace std;

// Define a structure for Huffman tree nodes
struct Node
{
    char data;
    int freq;
    Node *left, *right;
    Node(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

// Comparison function for priority queue
struct compare
{
    bool operator()(Node *l, Node *r)
    {
        return (l->freq > r->freq);
    }
};

// Prints Huffman codes from the root of Huffman Tree
void printCodes(Node *root, string str, unordered_map<char, string> &huffmanCode)
{
    if (!root)
        return;
    if (root->data != '$')
        huffmanCode[root->data] = str;
    printCodes(root->left, str + "0", huffmanCode);
    printCodes(root->right, str + "1", huffmanCode);
}

// Builds Huffman Tree and returns root node
Node *buildHuffmanTree(char data[], int freq[], int size)
{
    Node *left, *right, *top;
    priority_queue<Node *, vector<Node *>, compare> minHeap;

    for (int i = 0; i < size; ++i)
        minHeap.push(new Node(data[i], freq[i]));

    while (minHeap.size() != 1)
    {
        left = minHeap.top();
        minHeap.pop();
        right = minHeap.top();
        minHeap.pop();

        top = new Node('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        minHeap.push(top);
    }
    return minHeap.top();
}
// Encode input file using Huffman codes and write to output file
void encodeFile(ifstream &inputFile, ofstream &outputFile, unordered_map<char, string> &huffmanCode)
{
    char ch;
    string encodedBits;
    while (inputFile.get(ch))
    {
        encodedBits += huffmanCode[ch];
    }

    // Calculate the number of padding bits
    int padding = (8 - encodedBits.length() % 8) % 8;

    // Write the number of padding bits at the beginning of the compressed file
    outputFile.put(padding);

    // Write encoded bits to output file
    string buffer; // buffer to store 8 bits before writing to output file
    for (size_t i = 0; i < encodedBits.length(); ++i)
    {
        buffer.push_back(encodedBits[i]);
        if (buffer.length() == 8)
        {
            bitset<8> bits(buffer);
            outputFile.put(char(bits.to_ulong()));
            buffer.clear();
        }
    }

    // Handle remaining bits
    if (!buffer.empty())
    {
        while (buffer.length() < 8)
        {
            buffer.push_back('0');
        }
        bitset<8> bits(buffer);
        outputFile.put(char(bits.to_ulong()));
    }
}

// Decode compressed file using Huffman tree
void decodeFile(ifstream &inputFile, ofstream &outputFile, unordered_map<char, string> &huffmanCode)
{
    // Read the number of padding bits from the beginning of the compressed file
    int padding;
    inputFile.get(reinterpret_cast<char &>(padding));

    string bitBuffer; // Buffer to store bits read from the input file
    char ch;
    while (inputFile.get(ch))
    {
        bitBuffer += bitset<8>(ch).to_string(); // Convert each byte to a string of bits and append to the buffer
    }

    // Remove padding bits
    bitBuffer = bitBuffer.substr(0, bitBuffer.length() - padding);

    // Decode bits and write to output file
    string code;
    for (size_t i = 0; i < bitBuffer.length(); ++i)
    {
        code += bitBuffer[i];
        for (const auto &pair : huffmanCode)
        {
            if (pair.second == code)
            {
                outputFile.put(pair.first);
                code.clear();
                break;
            }
        }
    }
}

int main()
{
    // Input and output file paths
    string inputFile = "C:\\Users\\DEEPAK REDDY\\Desktop\\password manager\\input.txt";
    string compressedFile = "C:\\Users\\DEEPAK REDDY\\Desktop\\password manager\\compressed.txt";
    string decompressedFile = "C:\\Users\\DEEPAK REDDY\\Desktop\\password manager\\decompressed.txt";

    // Open input file
    ifstream input(inputFile, ios::binary);
    if (!input)
    {
        cerr << "Error: Cannot open input file" << endl;
        return 1;
    }

    // Get the size of the input file
    input.seekg(0, ios::end);
    long long inputFileSize = input.tellg();
    input.seekg(0);

    // Calculate frequency of characters in input file
    unordered_map<char, int> freqMap;
    char ch;
    while (input.get(ch))
        freqMap[ch]++;
    input.clear();
    input.seekg(0);

    // Convert frequency map to arrays for Huffman Tree construction
    int size = freqMap.size();
    char data[size];
    int freq[size];
    int i = 0;
    for (const auto &pair : freqMap)
    {
        data[i] = pair.first;
        freq[i] = pair.second;
        i++;
    }

    // Build Huffman tree
    Node *root = buildHuffmanTree(data, freq, size);
    if (!root)
    {
        cerr << "Error: Failed to build Huffman tree" << endl;
        return 1;
    }

    // Generate Huffman codes
    unordered_map<char, string> huffmanCode;
    printCodes(root, "", huffmanCode);

    // Open output file for compressed data
    ofstream output(compressedFile, ios::binary);
    if (!output)
    {
        cerr << "Error: Cannot create output file" << endl;
        return 1;
    }

    // Encode input file using Huffman codes and write to output file
    encodeFile(input, output, huffmanCode);

    // Get the size of the compressed file
    output.seekp(0, ios::end);
    long long compressedFileSize = output.tellp();

    // Close input and output files
    input.close();
    output.close();

    // Print sizes
    cout << "Original file size: " << inputFileSize << " bytes" << endl;
    cout << "Compressed file size: " << compressedFileSize << " bytes" << endl;
    cout << "File compressed successfully" << endl;

    // Read compressed file and decompress
    ifstream compressed(compressedFile, ios::binary);
    if (!compressed)
    {
        cerr << "Error: Cannot open compressed file" << endl;
        return 1;
    }

    // Open output file for decompressed data
    ofstream decompressed(decompressedFile, ios::binary);
    if (!decompressed)
    {
        cerr << "Error: Cannot create output file for decompressed data" << endl;
        return 1;
    }
    // Decode compressed file using Huffman tree
    decodeFile(compressed, decompressed, huffmanCode);

    // Close files
    compressed.close();
    decompressed.close();

    // Open the decompressed file for reading
    ifstream decompressedRead(decompressedFile, ios::binary);
    if (!decompressedRead)
    {
        cerr << "Error: Cannot open decompressed file for reading" << endl;
        return 1;
    }

    // Get the size of the decompressed file
    decompressedRead.seekg(0, ios::end);
    long long decompressedFileSize = decompressedRead.tellg();

    // Close the decompressed file
    decompressedRead.close();

    // Print decompressed file size
    cout << "Decompressed file size: " << decompressedFileSize << " bytes" << endl;

    cout << "File decompressed successfully" << endl;
    return 0;
}
