#pragma once
#include <string>
#include <map>
#include <fstream>
#include <utility>
using namespace std;

void removeStopWords(map<string, pair<int, int>>& words) // Remove stop words from the trained model.
{
	// Import stop words.
	fstream stopStream;
	stopStream.open("stopwords.list", fstream::in);
	string stopWord;
	
	if (stopStream.good()) {
		while(getline(stopStream, stopWord)) {
			if (words.count(stopWord)) {
				words.erase(words.find(stopWord));
			}
		} 
	}
	stopStream.close();
}

void toLower(string& word) // Convert Uppercase ASCII letters to lowercase
{
	for (int i = 0; i < word.size(); i++) {
		if (word[i] >= 'A' && word[i] <= 'Z') word[i] += 32;
	}
}

void cleanLine(string& line) // Remove non letters and non numbers
{
	for (int i = 0; i < line.size(); i++) {
		if ((line[i] >= '0' && line[i] <= '9') || (line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z') || (line[i] == 39)) {} // These characters are allowed.
		else line.replace(i, 1, " "); // Replace non-acceptable character with a space.
	
	}
}

void cleanUp(string& line) // Prepares a string for classifcation.
{
	cleanLine(line);
	toLower(line);
}
