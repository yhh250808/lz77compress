#include<iostream>
#include <fstream>
#include<string>
#include<vector>
#include<windows.h>
using namespace std;

#define WINDOWSSIZE 16384//滑动窗口大小
#define SOURCESIZE 16384-128//前向缓冲区大小
int fileSize;

void compress(int& sourceStart, int& sourceEnd, int& searchStart, int& searchEnd, int& offset, int& length, char& value, char* fileSource)//压缩函数
{

	if (sourceStart == -1 && sourceEnd == 0) {
		offset = 0; length = 0; value = fileSource[0];
		sourceStart = 0; sourceEnd = 0; searchStart = 1;
		return;
	}
	int i, j;
	vector<int> tempLength;
	for (i = sourceStart; i <= sourceEnd; i++) {
		if (fileSource[i] == fileSource[searchStart]) {
			length++; int k = i;
			for (j = searchStart + 1; j <= searchEnd; j++) {
				if (fileSource[++k] == fileSource[j])length++;
				else break;
			}
			tempLength.push_back(length);
			length = 0;
		}
		else tempLength.push_back(0);
	}
	int maxL = 0, maxV = 0;
	for (i = 0; i < tempLength.size(); i++) 
		if (tempLength[i] > maxV) {
			maxL = i; maxV = tempLength[i];
		}
	if (maxV == 0||maxV == 1) {
		offset = 0; length = 0; value = fileSource[searchStart];
		if (sourceEnd < SOURCESIZE-1) {
			sourceEnd++;
		}
		else {
			sourceStart++; sourceEnd++;
		}
		if (fileSize < WINDOWSSIZE) {
			searchStart++;
		}
		else {
			if (searchEnd < fileSize - 1) {
				searchStart++; searchEnd++;
			}
			else {
				searchStart++;
			}
		}
	}
	else {
		offset = tempLength.size() - maxL;
		length = maxV;
		//offsetV.push_back(offset);
		//lengthV.push_back(length);
		if (sourceEnd >= SOURCESIZE - 1) {
			sourceStart += length; sourceEnd += length;
		}
		else if (sourceEnd + length < SOURCESIZE) {
			sourceEnd += length;
		}
		else {
			sourceStart += (length - SOURCESIZE + sourceEnd);
			sourceEnd += length;
		}

		if (searchEnd == fileSize - 1) {
			searchStart += length;
		}
		else if ((searchEnd + length) < WINDOWSSIZE) {
			searchEnd += length; searchStart += length;
		}
		else {
			searchStart += length;
			searchEnd = fileSize - 1;
		}
	}
}

void compressMenu()
{
	string filePathI;
	string filePathO;

FILE:
	cout << "please input the source file path(the file you want to compress):" << endl;
	cin >> filePathI;
	//filePathI = "D:\\1.txt";
	ifstream fin(filePathI.c_str(), ios::binary);
	if (!fin) {
		cout << "can't open file!! check out your file path" << endl;
		goto FILE;
	}
	cout << "please input the path on where you want to put your file down:" << endl;
	cin>>filePathO;
	//filePathO = "d:\\1.lz77";
	ofstream fout(filePathO.c_str(), ios::binary);
	if (!fout) {
		cout << "can't open file!! check out your file path" << endl;
		goto FILE;
	}
	fin.seekg(0, ios::end);
	int iSize = fin.tellg();
	fileSize = iSize;
	char *fileSource = new char[iSize];
	fin.seekg(0, ios::beg);
	fin.read(fileSource, sizeof(char) * iSize);
	fin.close();
	int sourceStart = -1, sourceEnd = 0, searchStart = 0, searchEnd = min(WINDOWSSIZE - 1, (iSize - 1));
	int offset = 0, length = 0; char value;
	string result;
	while (searchStart < iSize) {
		compress(sourceStart, sourceEnd, searchStart, searchEnd, offset, length, value, fileSource);
		if (offset == 0 && length == 0) {
			result.push_back((char) (offset)); result.push_back((char)(offset));//00
			result.push_back((char) (length)); //0
			result.push_back(value);
		}
		else {
			int higho, lowo;//高位，低位
			higho = offset / 128; lowo = offset % 128;
			result.push_back((char) (higho)); result.push_back((char)(lowo));
			result.push_back((char) (length)); 
		}
		offset = 0; length = 0;
	}

	fout.write(result.data(), sizeof(char) * result.size());
	fout.close();

	cout << "succeed" << endl;
	system("pause");
}

void Decompress()
{
	string filePathI, filePathO;

FILE:
	cout << "please input the file path(the file you want to decompress):" << endl;
	cin >> filePathI;
	//filePathI = "d:\\1.lz77";
	ifstream fin(filePathI.c_str(), ios::binary);
	if (!fin) {
		cout << "can't open file!! check out your file path" << endl;
		goto FILE;
	}
	cout << "please input where you want to put down your decompressed file:" << endl;
	cin>> filePathO;
	//filePathO = "D:\\10.txt";
	ofstream fout(filePathO.c_str(), ios::binary);
	if (!fout) {
		cout << "can't open file!! check out your file path" << endl;
		goto FILE;
	}

	fin.seekg(0, ios::end);
	int iSize = fin.tellg();
	fileSize = iSize;
	char *fileSource = new char[iSize];
	fin.seekg(0, ios::beg);
	fin.read(fileSource, sizeof(char) * iSize);
	fin.close();
	int i = 0, j=0, offset = 0, length = 0; char value = NULL;

	string result;
	while (true) {
		if (i == fileSize)break;
		int higho, lowo;//高位，低位
		higho = (int)fileSource[i++];
		lowo = (int)fileSource[i++];
		offset = higho * 128 + lowo;
		length = (int)fileSource[i++];
		if (offset == 0 && length == 0) {
			value = fileSource[i++];
			result.push_back(value);
			j++;
		}
		else {
			for (int k = 0; k < length; k++) {
				result.push_back(result[j - offset + k]);
			}
			j += length;
		}
	}

	fout.write(result.data(), sizeof(char)*result.size());
	fout.close();
	cout << "succeed" << endl;
	system("pause");
}
int main()
{
	int choice = 1;
	while (choice) {
		cout << "input ypur choice" << endl;
		cout << "1.compress" << endl << "2.decompress" << endl<<"3.exit"<<endl;
		cin >> choice;

		switch (choice) {
		case 1:
			compressMenu();
			system("cls");
			break;
		case 2:
			Decompress();
			system("cls");
			break;
		case 3:
			system("pause");
			return 0;
		default:
			system("pause");
			return 0;
		}
	}
}