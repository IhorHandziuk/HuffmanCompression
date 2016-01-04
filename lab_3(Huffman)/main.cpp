#include <thread>
#include <cstring>
#include "huffman.h"

// returning values of main(): 
// 0 - Exit success
// 1 - Incorrect parameters
// 2 - File not open

int main (int argc, char* argv[]) 
{
	if (argc < 3)  return 1;
	bool toEncrypt = true;
	char* arg = new char[256];
	if (!strcmp(argv[1], "-e")) toEncrypt = true;
	else if (!strcmp(argv[1], "-d")) toEncrypt = false;
	else return 1;

	vector<thread> threads(argc - 2);
	for (int i = 2; i < argc; i++) 
	{
		strcpy(arg, argv[i]);
		FILE *input = fopen(arg, "rb");
		FILE* output; 
		FILE* tableFile;
		if(!input) return 2;
		if (toEncrypt) {
			output = fopen(strcat(arg, ".huf"), "wb");
			tableFile = fopen(strcat(arg, ".table"), "wb");
			threads[i-2] = thread(encrypt, input, output, tableFile);
		}
		else {
			tableFile = fopen(strcat(arg, ".table"), "rb");
			if(!tableFile) return 2;
			strcpy(arg, argv[i]);
			output = fopen(strcat(arg, ".dec"), "wb");
			map<char, vector<bool>> table = readTable(tableFile);
			threads[i-2] = thread(decrypt, table, input, output);
		}
	}
	for (int i = 0; i < threads.size(); i++)
		if(threads[i].joinable()) threads[i].join();
	delete arg;
	fcloseall();
	system("pause");
} 