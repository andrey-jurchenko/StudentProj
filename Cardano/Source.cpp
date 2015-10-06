#include <iostream>
#include <time.h>
#include <vector>
#include <fstream>
#include <string>
#include <Windows.h>

using namespace std;

const int numOfRotate = 3;

string wordsCoder = "";
string wordsPrinting = "";

void ReadMatrixFromFile(char *workMatrix, const int N)
{
	ifstream file("matrix.txt");
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			file >> *(workMatrix + i*N + j);
		}
	}
	file.close();
}

void ReadWordsFromFile()
{
	char temp;
	ifstream file;
	SetConsoleCP(1251);
	file.open("words.txt");
	while (file >> temp)
	{
		wordsCoder += temp;
	}

	file.clear();
	file.seekg(0, ios::beg);
	file.unsetf(ios::skipws);
	while (file >> temp)
	{
		wordsPrinting += temp;
	}

	file.close();
	SetConsoleCP(866);
}

void PrintMatrix(char* workMatrix, const int N)
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			cout << *(workMatrix + i*N + j) << '\t';
		}
		cout << endl;
	}
}

void GetMatrixWithWords(char* workMatrix, const int N)
{
	int k = 0;
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (wordsCoder[k] == '\0')
				break;
			if (wordsCoder[k] == '\n')
				k++;
			if (*(workMatrix + i*N + j) == '0')
			{
				*(workMatrix + i*N + j) = wordsCoder[k];
				k++;
			}
		}
	}
}

int GetCountOfZeroInMatrix(char* workMatrix, const int N)
{
	int count = 0;

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if(*(workMatrix + i*N + j) == '0')
			{
				count++;
			}
		}
	}

	return count;
}

int GetCountOfSymbolsInWords()
{
	int countSymbol = 0;
	for (int i = 0; i < (int)wordsCoder.size(); i++)
	{
		countSymbol++;
	}

	return countSymbol;
}

string RotateMatrix(char* workMatrix, int count, const int N)
{
	int k = 0;
	string str = "";

	int* arrayOne = new int[count*2];
	int* arrayTwo = new int[count*2];

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if(*(workMatrix + i*N + j) == '1')
			{
				arrayOne[k] = i;
				arrayOne[k+1] = j;
				arrayTwo[k] = N-1-j;
				arrayTwo[k+1] = i;
				if (*(workMatrix + (N-1-j)*N + i) != '0')
					str += *(workMatrix + (N-1-j)*N + i);
				k+=2;
			}
		}
	}

	if (k != count*2)
	{
		cout << endl << "Данный трафарет нельзя использовать для кодировки текста" << endl;
		exit (EXIT_FAILURE);
	}

	for (int i = 0; i < count*2; i+=2)
	{
		*(workMatrix + arrayOne[i]*N + arrayOne[i+1]) = 'x';
		*(workMatrix + arrayTwo[i]*N + arrayTwo[i+1]) = '1';
	}

	delete arrayOne;
	delete arrayTwo;

	return str;
}

void CopyArray(char* dest, char* source, const int N, int iter)
{
	if (iter == 0)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				*(dest + i*N + j) = *(source + i*N + j);
			}
		}
	}
	else
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				*(dest + i*N + j) = *(source + i*N+i*iter + j);
			}
		}
	}
	
}

void CoderWordsInMatrix(char* workMatrix, const int N, string& coderString, int count)
{
	GetMatrixWithWords(workMatrix, N);
	cout << endl << endl;
	PrintMatrix(workMatrix, N);

	for (int i = 0; i < numOfRotate; i++)
	{
		coderString += RotateMatrix(workMatrix, count, N);
		coderString += " ";
		cout << endl << endl;
		PrintMatrix(workMatrix, N);
	}
}

void CodingWords(char* workMatrix, char* copyWorkMatrix, int countOfZero, const int N, string& coderString, int count)
{
	//CopyArray(workMatrix, copyWorkMatrix, N, 0);
	CoderWordsInMatrix(workMatrix, N, coderString, count);
	wordsCoder.assign(wordsCoder, countOfZero, wordsCoder.size());
	cout << endl << wordsCoder << endl;
}

bool IsMatrixIsBad(char* workMatrix, const int N)
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (*(workMatrix + i*N + j) != 'x' && *(workMatrix + i*N + j) != '1')
			{
				return true;
			}
		}
	}

	return false;
}

void main()
{
	setlocale(LC_ALL, "rus");

	const int maxN = 6;
	const int midN = 4;
	const int minN = 2;
	const int numArrays = 3;
	int t = 0;
	int count = 0;
	int countSpaces = 0;
	int countOfZero = 0;
	int countOfSymbols = 0;

	int iterations = 0;
	int iter = 0;
	int N = 6;

	char workMatrixMax[maxN][maxN];
	char workMatrixMid[midN][midN];
	char workMatrixMin[minN][minN];
	char copyWorkMatrixMax[maxN][maxN];

	char* workMatrix[numArrays];
	workMatrix[0] = &workMatrixMax[0][0];
	workMatrix[1] = &workMatrixMid[0][0];
	workMatrix[2] = &workMatrixMin[0][0];

	string coderString = "";

	ReadMatrixFromFile(&workMatrixMax[0][0], maxN);
	ReadWordsFromFile();

	cout << "Ваш квадратный трафарет:" << endl;
	PrintMatrix(&workMatrixMax[0][0], maxN);
	CopyArray(&copyWorkMatrixMax[0][0], &workMatrixMax[0][0], maxN , 0);

	cout << endl << "Ваши слова для шифровки:" << endl;
	cout << endl << wordsPrinting << endl;

	while (true)
	{
		countOfSymbols = GetCountOfSymbolsInWords();
		countOfZero = GetCountOfZeroInMatrix(workMatrix[t], N);
		iterations = countOfSymbols / countOfZero;
		count =  N*N - countOfZero;
		if (iterations > 0)
		{
			for (int i = 0; i < iterations; i++)
			{
				if (wordsCoder.size() < 4)
					break;
				CodingWords(workMatrix[t], &copyWorkMatrixMax[t][t], countOfZero, N, coderString, count);
			}

			if (IsMatrixIsBad(workMatrix[t], N) == true)
			{
				cout << endl << "Трафарет нельзя использовать для кодирования текста" << endl;
				exit(EXIT_FAILURE);
			}
		}
		if (wordsCoder.size() < 4)
			break;
		else
		{
			t++;
			iter += 2;
			N -= 2;
			CopyArray(workMatrix[t], &copyWorkMatrixMax[t][t], N, iter);
			cout << endl;
			PrintMatrix(workMatrix[t], N);
		}
	}

	cout << endl << coderString + wordsCoder << endl;
}