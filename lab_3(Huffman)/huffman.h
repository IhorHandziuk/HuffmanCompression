//Huffman algorithm, Ihor Handziuk, 2015

#pragma once
#include <map>
#include <vector>
using namespace std;

//Decrypt data from "in" file to "out" file
void decrypt(map<char, vector<bool>> table, FILE* in, FILE* out);

//Encrypt input file into output file 
//and write encryption table into tableFile
void encrypt(FILE* input, FILE* output, FILE* tableFile);

//Read encryption table from tableFile
map<char, vector<bool>> readTable(FILE* tableFile);
