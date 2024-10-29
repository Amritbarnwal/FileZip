#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <unordered_map>

using namespace std;

// Tree node structure
struct Node {
    char ch;
    int freq;
    Node *left, *right;
};

// Function to allocate a new tree node
Node* getNode(char ch, int freq, Node* left, Node* right) {
    Node* node = new Node();
    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;
    return node;
}

// Comparison object for ordering in the heap
struct comp {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;  // Min-heap by frequency
    }
};

// Traverse the Huffman Tree and store Huffman Codes in a map.
void encode(Node* root, string str, unordered_map<char, string> &huffmanCode) {
    if (root == nullptr) return;

    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
    }

    encode(root->left, str + "0", huffmanCode);
    encode(root->right, str + "1", huffmanCode);
}

// Traverse the Huffman Tree and decode the encoded string
void decode(Node* root, int &index, string str, ofstream &decodedFile) {
    if (root == nullptr) return;

    if (!root->left && !root->right) {
        decodedFile << root->ch;
        return;
    }

    index++;
    if (str[index] == '0')
        decode(root->left, index, str, decodedFile);
    else
        decode(root->right, index, str, decodedFile);
}

// Builds Huffman Tree and encodes input text from input.txt
void buildHuffmanTree() {
    // Read the input text from input.txt
    ifstream inputFile("input.txt");
    string text;
    getline(inputFile, text, '\0');
    inputFile.close();

    // Count frequency of each character
    unordered_map<char, int> freq;
    for (char ch : text) {
        freq[ch]++;
    }

    // Create a priority queue (min-heap)
    priority_queue<Node*, vector<Node*>, comp> pq;

    // Create a leaf node for each character and add it to the priority queue
    for (auto pair : freq) {
        pq.push(getNode(pair.first, pair.second, nullptr, nullptr));
    }

    // Build the Huffman Tree
    while (pq.size() != 1) {
        Node *left = pq.top(); pq.pop();
        Node *right = pq.top(); pq.pop();

        int sum = left->freq + right->freq;
        pq.push(getNode('\0', sum, left, right));
    }

    // Root of the Huffman Tree
    Node* root = pq.top();

    // Encode the characters using the Huffman Tree
    unordered_map<char, string> huffmanCode;
    encode(root, "", huffmanCode);

    // Open encoded.huf to write metadata and encoded text
    ofstream encodedFile("encoded.huf");

    // Write metadata (character frequencies) to the file
    encodedFile << "CHAR_FREQ:\n";
    for (auto pair : freq) {
        encodedFile << pair.first << ":" << pair.second << "\n";
    }
    encodedFile << "DATA:\n";  // Mark the start of encoded binary data

    // Encode the text
    string encodedString;
    for (char ch : text) {
        encodedString += huffmanCode[ch];
    }

    // Write encoded binary data as a single line
    encodedFile << encodedString;
    encodedFile.close();

    // Decode the file to verify correctness (writes to decoded.txt)
    ofstream decodedFile("decoded.txt");
    int index = -1;
    while (index < (int)encodedString.size() - 2) {
        decode(root, index, encodedString, decodedFile);
    }
    decodedFile.close();
}

// main function
int main() {
    buildHuffmanTree();
    return 0;
}
