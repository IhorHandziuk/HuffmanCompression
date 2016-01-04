#include "huffman.h"

//node for building Huffman tree
struct node {
	unsigned char ch;
	node *left;
	node *right;
	node(char ch_) {
		ch = ch_;
		left = nullptr;
		right = nullptr;
	}
};

//This union will be used for writing and reading files 
union Byte {
	unsigned char b;
	struct {
		unsigned  char b0 : 1,
			b1 : 1,
			b2 : 1,
			b3 : 1,
			b4 : 1,
			b5 : 1,
			b6 : 1,
			b7 : 1;
	} code;
	bool at(int index) {
		switch (index) {
		case 0: return code.b0; break;
		case 1: return code.b1; break;
		case 2: return code.b2; break;
		case 3: return code.b3; break;
		case 4: return code.b4; break;
		case 5: return code.b5; break;
		case 6: return code.b6; break;
		case 7: return code.b7; break;
		default: return code.b0;
		}
	}
};

//Building Huffman tree
node* buildTree(multimap<int, node*> chars) {
	//We get 2 elements with the smallest number of
	//repetitions and band them until we have one node
	while (chars.size() != 1) {
		pair<int, node*> bindNode;
		node* curr = new node(' ');

		bindNode.first = chars.begin()->first;
		curr->left = chars.begin()->second;
		chars.erase(chars.begin());

		bindNode.first += chars.begin()->first;
		curr->right = chars.begin()->second;
		chars.erase(chars.begin());

		bindNode.second = curr;
		chars.insert(bindNode);
	}
	return chars.begin()->second;
}

void buildTable(node *treeRoot, map<char, vector<bool>>& table, vector<bool> code = vector<bool>(0), bool value = false) {
	//The code of the symbol is forming in a such way:
	//We start at the root of Huffman tree. If we go 
	//to the left son, we add to code 0; if we go to the
	//right son, we add 1. If current node is a leaf, then 
	//the code of char in leaf is a sequence that we get eirlier
	if (treeRoot) {
		code.push_back(value);
		//in Huffman tree there are no nodes that have one son, but don't have another
		if (!treeRoot->left) {
			//erase zero that we push at start
			if (code.size() > 1) code.erase(code.begin());
			table[treeRoot->ch] = code;
		}
		else {
			buildTable(treeRoot->left, table, code, false);
			buildTable(treeRoot->right, table, code, true);
		}
	}
}

//Check if the table contains char with buf value 
char ñontains(map <char, vector<bool>> table, vector<bool> buf) {
	for (auto i : table)
		if (i.second == buf) return i.first;
	return 0;
}

//Deleting Huffman tree
void deleteTree(node* treeRoot) {
	if (!treeRoot->left && !treeRoot->right) delete treeRoot;
	else if (treeRoot->left) deleteTree(treeRoot->left);
	else if (treeRoot->right) deleteTree(treeRoot->right);
}

//Write encryption table to the tableFile
void writeTable(map<char, vector<bool>> table, FILE* tableFile) {
	for (auto i : table) {
		fwrite(&i.first, sizeof(char), 1, tableFile);
		for (auto j = i.second.begin(); j != i.second.end(); j++) {
			char curr = *j;
			fwrite(&curr, sizeof(bool), 1, tableFile);

		}
		fwrite(&i.first, sizeof(char), 1, tableFile); //dublicate the char for correct decryption
	}
}

//Read table from the tableFile
map<char, vector<bool>> readTable(FILE* tableFile) {
	map<char, vector<bool>> res;
	char ch;
	char insteadBool;
	while (fread(&ch, sizeof(char), 1, tableFile)) {
		vector<bool> curr;
		while (fread(&insteadBool, sizeof(bool), 1, tableFile) && ((int)insteadBool < 2)) {
			curr.push_back((bool)insteadBool);
		}
		res.insert(pair<char, vector<bool>>(ch, curr));
	}
	return res;
}

void decrypt(map<char, vector<bool>> table, FILE* in, FILE* out) {
	long long charN;
	Byte myByte;
	char value;
	vector<bool> buf;
	fread(&charN, sizeof(long long), 1, in);
	while (fread(&myByte.b, sizeof(unsigned char), 1, in)) {
		for (int i = 0; i < 8; i++) {
			buf.push_back(myByte.at(i));
			if ((value = ñontains(table, buf)) && charN) {
				fwrite(&value, sizeof(value), 1, out);
				buf.clear();
				charN--;
			}
		}
	}
}

void encrypt(FILE* input, FILE* output, FILE* tableFile) {
	map<char, int> symbols; //map with <symbol, count of appearing>
	multimap<int, node*> chars; //chars is like symbols, but with swaped key - value 

	long long charsN = 0;
	char currCh;

	//read input data into symbols
	while ((currCh = fgetc(input)) != EOF) {
		int symSize = symbols.size();
		symbols.insert(pair<char, int>(currCh, 1));
		if (symSize == symbols.size()) {
			symbols[currCh]++;
		}
		charsN++;
	}

	//make map of chars from symbols 
	for (auto i : symbols) {
		node* curr = new node(i.first);
		chars.insert(pair<int, node*>(i.second, curr));
	}

	//make code table using Huffman tree
	node* treeRoot = buildTree(chars);
	map<char, vector<bool>> table;
	buildTable(treeRoot, table);

	//To see the Huffman code uncomment next lines:
	/*for (auto i: table) {
	cout << i.first << " ";
	for (auto j: i.second) cout << j;
	cout << endl;
	}*/

	//write code of the char instead the char in compressed file
	rewind(input);
	Byte myByte;
	int k = 0;
	fwrite(&charsN, sizeof(long long), 1, output);
	while ((currCh = fgetc(input)) != EOF) {
		for (auto i : table[currCh]) {
			if (k > 7) {
				fwrite(&myByte, sizeof(Byte), 1, output);
				k = 0;
			}
			switch (k) {
			case 0: myByte.code.b0 = i; break;
			case 1: myByte.code.b1 = i; break;
			case 2: myByte.code.b2 = i; break;
			case 3: myByte.code.b3 = i; break;
			case 4: myByte.code.b4 = i; break;
			case 5: myByte.code.b5 = i; break;
			case 6: myByte.code.b6 = i; break;
			case 7: myByte.code.b7 = i; break;
			}
			k++;
		}
	}
	fwrite(&myByte, sizeof(Byte), 1, output);
	deleteTree(treeRoot);
	fclose(output);
	writeTable(table, tableFile);
}

