#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include "helper.h"
using namespace std;

map<string, pair<int, int>> read_csv(ifstream& filename);
void cleanInfrequentClassifications(map<string, pair<int, int>>& words);
void outputProbability(map<string, pair<int, int>>& words, ofstream& hamFile, ofstream& spamFile);

int main(int argc, char** argv)
{
	if (argc < 4 && (argc != 7 || argc != 4)) {	
		cerr << "Inccorect argument count" << endl;
		return -1;
	}
	string i, os, oh;
	if (argc == 4) { // No optional arguments provided. 
		i = string(argv[1]); // Assume first argument is input. 
		os = string(argv[2]); // Assume second argument is spam output.
		oh = string(argv[3]); // Assume third argument is ham output.
	}
	else {
		string x[6];
		for (int j = 1; j < 7; j++) {
			x[j-1] = string(argv[j]);
		}
		if (x[0][0] == '-' && x[2][0] == '-' && x[4][0] == '-') {
			for (int k = 0; k <= 4; k += 2) {
				if (x[k][1] == 'i') {
					i = x[k+1];
				}
				else if (x[k][1] == 'o' && x[k][2] == 's') {
					os = x[k+1];
				}
				else if (x[k][1] == 'o' && x[k][2] == 'h') {
					oh = x[k+1];
				}
				else {
					cerr << "Unknown argument " << x[k] << endl;
					return -1;
				}
			}
		}
		else {
			cerr << "Incorrect argument structure" << endl;
			return -1;
		}
	}
	
	if (i.empty()) {
		cerr << "Input argument not satisfied" << endl;
		return -1;
	}
	if (os.empty()) {
		cerr << "Output spam argument not satisfied" << endl;
		return -1;
	}
	if (oh.empty()) {
		cerr << "Output ham argument not satisfied" << endl;
		return -1;
	}
	
	

	ifstream inputFile(i, ifstream::in);    // Input File Name
	ofstream spamFile (os, ofstream::out);  // Output Spam File
	ofstream hamFile  (oh, ofstream::out);  // Output Ham File
	
	map<string, pair<int, int>> trained_model = read_csv(inputFile); // <word: <spam_count, ham_count>>
	cleanInfrequentClassifications(trained_model); 		   // Remove classifcations where spam_count + ham_count < 3
	removeStopWords(trained_model); 				   // Remove words that likely play no part to the meaning of a message.
	outputProbability(trained_model, spamFile, hamFile);            // Output probability files for classifcation
	
	return 0;
}

map<string, pair<int, int>> read_csv(ifstream& filename)
{
	string line, word;
	map<string, pair<int, int>> words;
	if( filename.good() ) {
		while(getline(filename, line)) {
			for(int index = 0; index < line.size() && line[index] == ','; ++index) line.replace(index, 1, ""); // Remove commas at the beginning of row (if any)
			if (line[0] == 's' || line[0] == 'S') { // Mark words for spam
				line.erase(0, 5); // Remove spam, from line
				cleanLine(line);
				toLower(line);
				for (int i  = 0; i < line.size(); i++) {
					if (line[i] != ' ') word += line[i];
					else if (word.size()) {
						if (words.count(word) == 0) { // Word does not exist in map.
							words.insert(pair<string, pair<int, int>>(word, make_pair(1, 0)));
						}
						else { // Word exists in map.
							get<0>(words.find(word)->second) += 1; // Increment spam count
						}
						word.erase(word.begin(), word.end());
					}
				}
			}
			else if (line[0] == 'h' || line[0] == 'H') { // Mark words for not spam.
				line.erase(0, 4); // Remove ham, from line
				cleanLine(line);
				toLower(line);
				for (int i = 0; i < line.size(); i++) {
					if (line[i] != ' ') word += line[i];
					else if (word.size()) {
						if (words.count(word) == 0) { // Word does not exist in map.
							words.insert(pair<string, pair<int, int>>(word, make_pair(0, 1)));
						}
						else {// Word exists in map.
							get<1>(words.find(word)->second) += 1; // Increment ham count
						}
						word.erase(word.begin(), word.end());
					}
				}
			}
		}	
	}
	else {
		cerr << " Cannot read file ! " << endl;
	}
	filename.close();
	return words;
}

void cleanInfrequentClassifications(map<string, pair<int, int>>& words)
{
	auto it = words.begin();
	auto temp = words.begin();
	while (it != words.end()) {
		if ((get<0>(it->second) + get<1>(it->second)) < 3) {
			temp = it;
			it++;
			words.erase(temp);
		}
		else it++;
	}
}

void outputProbability(map<string, pair<int, int>>& words, ofstream& hamFile, ofstream& spamFile)
{
	for (auto it = words.begin(); it != words.end(); ++it) {
		hamFile << it->first << " " << get<0>(it->second) << endl;
		spamFile << it->first << " " << get<1>(it->second) << endl;
	}
	hamFile.close(); spamFile.close();
}


