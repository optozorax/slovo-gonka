//#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#define min(a, b) (((a) < (b)) ? (a) : (b))

int main() {
	using namespace std;

	cout << "This is program to add words from 'dictionary.txt' file to 'words.txt' file." << endl;
	cout << "Do you want words to be added accidentaly? (y/n)" << endl;

	char c;
	cin >> c;

	bool random = (c == 'y') || (c == 'Y');

	cout << "How many words you want to add? " << endl;
	int count;
	cin >> count;

	vector<wstring> dict;
	vector<wstring> words;

	// Read words file
	wifstream wfin;
	wfin.open("dictionary.txt", ios_base::in);
	wchar_t buffer[500] = {};

	if (wfin) {
		while (!wfin.eof()) {
			wfin.getline(buffer, 500);
			wstring word(buffer);
			dict.push_back(word);
		}

		wfin.close();

		wfin.open("words.txt", ios_base::in);

		if (wfin) {
			while (!wfin.eof()) {
				wfin.getline(buffer, 500);
				wstring word(buffer);
				words.push_back(word);
			}

			wfin.close();
		}

		for (int i = 0; i < min(count, dict.size()); ++i) {
			int randPos;
			if (random) 
				randPos = rand() * dict.size() / RAND_MAX;
			else
				randPos = 1;
			words.push_back(dict[randPos]);
			dict.erase(dict.begin() + randPos);
		}

		wofstream wfout;
		wfout.open(L"words.txt", ios_base::out);

		for (int i = 0; i < words.size() - 1; ++i)
			wfout << words[i] << endl;
		wfout << words[words.size() - 1];

		wfout.close();

		wfout.open(L"dictionary.txt", ios_base::out);

		for (int i = 0; i < dict.size() - 1; ++i)
			wfout << dict[i] << endl;
		wfout << dict[dict.size() - 1];

		wfout.close();
	} else
		cout << "'dictionary.txt' file is not exists.";

	system("pause");
}