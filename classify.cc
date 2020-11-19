#include <iostream>
#include <vector>
#include "helper.h"
using namespace std;

map<string, pair<int, int>> get_model(ifstream& spamFile, ifstream& hamFile);
vector<string> getTests(ifstream& testFile);
void classifyTests(vector<string>& messages, map<string, pair<int, int>>& model, ofstream& outputFile);

int main(int argc, char** argv)
{
	string i, is, ih, o;
	if (argc == 5) { // No optional arguments provided.
		i = string(argv[1]); // Assume first argument is testing input.
		is = string(argv[2]);	// Assuming second argument is spam input.
		ih = string(argv[3]); // Assume third argument is ham input.
		o = string(argv[4]); // Assume fourth argument is classification output.
	}
	else {
		string x[8];
		for (int j = 1; j < 9; j++) {
			x[j-1] = string(argv[j]);
		}
		if (x[0][0] == '-' && x[2][0] == '-' && x[4][0] == '-' && x[6][0]) {
			for (int k = 0; k <= 6; k+= 2) {
				if (x[k].size() == 2) {
					if (x[k][1] == 'i') {
						i = x[k+1];
					}
					else if (x[k][1] == 'o') {
						o = x[k+1];
					}
					else {
						cerr << "Unknown argument " << x[k] << endl;
						return -1;
					}
				}
				else if (x[k][1] == 'i' && x[k][2] == 's') {
					is = x[k+1];
				}
				else if (x[k][1] == 'i' && x[k][2] == 'h') {
					ih = x[k+1];
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
	
	if (i.empty()) { cerr << "Input test argument not satisfied" << endl; return -1; }
	if (is.empty()) { cerr << "Input spam argument not satisfied" << endl; return -1; }
	if (ih.empty()) { cerr << "Input ham argument not satisfied" << endl; return -1; }
	if (o.empty()) { cerr << "Output argument not satisfied" << endl; return -1; }
	
	ifstream inputFile(i, ifstream::in);
	ifstream spamFile(is, ifstream::in);
	ifstream hamFile(ih, ifstream::in);
	ofstream classifyFile(o, ofstream::out);
	
	// Get the trained model from file.
	map<string, pair<int, int>> trained_model = get_model(spamFile, hamFile);  //word: <spam_count, ham_count>
	
	// Get messages to classify.
	vector<string> tests = getTests(inputFile);
	
	// The bread and butter. Naive bayes classifier. 
	classifyTests(tests, trained_model, classifyFile); 
	
	return 0;
}

map<string, pair<int, int>> get_model(ifstream& spamFile, ifstream& hamFile)
{
	map<string, pair<int, int>> model;
	if(spamFile.good() && hamFile.good()) {
		string line, word, num;
		int index, count;
		while(getline(spamFile, line)) {
			// Parse word
			index=0;
			while(line[index] != ' ') {
				word += line[index];
				index++;
			}
			// Parse spam count
			++index;
			while(index < line.size()) {
				num += line[index];
				index++;
			}
			
			// Convert spam count to int
			count = stoi(num, nullptr, 10);
			
			// Update our model.
			if (model.count(word) == 0) {
				model.insert(pair<string, pair<int, int>>(word, make_pair(count, 0)));
			}
			else {
				get<0>(model.find(word)->second) += count;
			}
			word.clear();
			num.clear();
			count = 0;
		}
		while(getline(hamFile, line))
		{
			// Parse word
			index=0;
			while(line[index] != ' ') {
				word += line[index];
				index++;
			}
			// Parse spam count
			++index;
			while(index < line.size()) {
				num += line[index];
				index++;
			}
			
			// Convert spam count to int
			count = stoi(num, nullptr, 10);
			
			// Update our model.
			if (model.count(word) == 0) {
				model.insert(pair<string, pair<int, int>>(word, make_pair(0, count)));
			}
			else {
				get<1>(model.find(word)->second) += count;
			}
			word.clear();
			num.clear();
			count = 0;
		}
	}
	
	
	spamFile.close();
	hamFile.close();
	return model;
}

vector<string> getTests(ifstream& testFile)
{
	string message;
	vector<string> messages;
		if (testFile.good()) {
			while(getline(testFile, message)) {
				messages.push_back(message);
			}
		}
		testFile.close();
	
	return messages;
}

void classifyTests(vector<string>& messages, map<string, pair<int, int>>& model, ofstream& outputFile)
{
	// Get the total ammount of spam/ham classifcations from our trained model.
	double spamTotal=0, hamTotal=0;
	for(auto it = model.begin(); it != model.end(); it++) {
		spamTotal += get<0>(it->second);
		hamTotal += get<1>(it->second);
	}
	
	vector<double> spamProb;
	vector<double> hamProb;
	
	// Probability
	double spam, ham, spam_count, ham_count; 
	string word, eval;
	for (auto& message : messages)
	{
		// Clean Message
		eval.assign(message);
		cleanLine(eval);
		toLower(eval);

		// Evaluate Message
		for (auto& c : eval) {
			if (c != ' ') word += c;
			else if(model.count(word)) {
					// Get number of times a word was marked for spam/ham
					spam_count = get<0>(model.find(word)->second);
					ham_count = get<1>(model.find(word)->second);
					
					// Calculate probability that word is spam and ham
					spamProb.push_back( spam_count / (spam_count + ham_count) );
					hamProb.push_back( ham_count / (spam_count + ham_count) );
					word.clear();
			}
			else word.clear();
		}
		
		// Prior Probability
		spam = spamTotal / (spamTotal + hamTotal);
		ham = hamTotal / (hamTotal + spamTotal);
		
		// Calculate probability scores the message being spam or ham.
		for(int index = 0; index < spamProb.size(); ++index) {
			if (spamProb[index] != 0) spam *= spamProb[index];
			if (hamProb[index] != 0) ham *= hamProb[index];
		}
		
		// Output to file
		if (ham >= spam) outputFile << "ham" "\n";    // We predict ham!
		else outputFile << "spam" << "\n"; 		// We predict spam!
		
		eval.clear();
		spamProb.clear();
		hamProb.clear();
	}

	outputFile.close();
}


