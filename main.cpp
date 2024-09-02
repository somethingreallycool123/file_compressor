#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>

using namespace std;

// Node structure for Huffman tree
struct Node {
    char character;
    int frequency;
    Node* left;
    Node* right;

    Node(char c, int f, Node* l = nullptr, Node* r = nullptr)
        : character(c), frequency(f), left(l), right(r) {}

    bool operator<(const Node& other) const {
        return frequency > other.frequency;
    }
};

// Function to build the Huffman tree
Node* buildHuffmanTree(unordered_map<char, int>& charFreq) {
    priority_queue<Node*> pq;

    // Create leaf nodes and add them to the priority queue
    for (auto& [c, f] : charFreq) {
        pq.push(new Node(c, f));
    }

    // Build the Huffman tree
    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        pq.push(new Node('\0', left->frequency + right->frequency, left, right));
    }

    return pq.top();
}

// Function to encode the file using Huffman codes
unordered_map<char, string> buildHuffmanCodes(Node* root) {
    unordered_map<char, string> huffmanCodes;

    function<void(Node*, string)> traverse = [&](Node* node, string code) {
        if (node->left == nullptr && node->right == nullptr) {
            huffmanCodes[node->character] = code;
            return;
        }

        traverse(node->left, code + "0");
        traverse(node->right, code + "1");
    };

    traverse(root, "");
    return huffmanCodes;
}

// Function to compress the file
void compressFile(const string& inputFile, const string& outputFile, const unordered_map<char, string>& huffmanCodes) {
    ifstream input(inputFile, ios::binary);
    ofstream output(outputFile, ios::binary);

    char c;
    string compressedData;

    while (input.get(c)) {
        compressedData += huffmanCodes.at(c);
    }

    // Write the compressed data to the output file
    int paddingBits = 8 - (compressedData.length() % 8);
    output.put(static_cast<char>(paddingBits));
    
    int val = 0;
    for (int i = 0; i < compressedData.length(); i++) {
        val = (val << 1) + (compressedData[i] - '0');
        if ((i + 1) % 8 == 0) {
            output.put(static_cast<char>(val));
            val = 0;
        }
    }

    if (compressedData.length() % 8 != 0) {
        output.put(static_cast<char>(val << (8 - (compressedData.length() % 8))));
    }
}

int main() {
    string inputFile = "input.txt";
    string outputFile = "compressed.bin";

    // Count the frequency of characters in the input file
    unordered_map<char, int> charFreq;
    ifstream input(inputFile, ios::binary);
    char c;
    while (input.get(c)) {
        charFreq[c]++;
    }
    input.close();

    // Build the Huffman tree
    Node* root = buildHuffmanTree(charFreq);

    // Build the Huffman codes
    unordered_map<char, string> huffmanCodes = buildHuffmanCodes(root);

    // Compress the file
    compressFile(inputFile, outputFile, huffmanCodes);

    cout << "File compressed successfully!" << endl;
    return 0;
