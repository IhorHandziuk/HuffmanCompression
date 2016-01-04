This program implements Huffman compressing algotithm
Made by Ihor Handziuk, 2015

To compress files run program in command prompt with parametr -e.
Example: huffman.exe -e file1.txt <file2.txt> <file3.txt>...
(type all the files you want to compress)

Note: if file is not in project root folder type the path to it:
e. g. path_to_the_file\file.txt 

To decompress files(extension .huf) run program in command prompt with parametr -d
Example: huffman.exe -d file1.txt.huf <file2.txt.huf> <file3.txt.huf>...
(type all the files you want to decompress)
Note: ensure you have .table files for all files you want to decompress

To read decompressed file delete ending of it's name (.huf.dec)
Example: input.txt.huf.dec -> input.txt


