#include "stdafx.h"
#include "LookUpTable.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>

    LookUpTable::LookUpTable()
	{
		double freq = 27.5;
		for (int i = 0; i < 88; i++)
		{
			notes[i] = freq;
			//std::cout << notes[i] << std::endl;
			freq = freq * 1.059464874;
		}
		setPatterns();
		SetGeneralNotePattern();
	}

	void LookUpTable::setPatterns()
	{
		for (int i = 0; i < 88; i++)
		{
			std::vector<int> P(88);
			for (int j = 1; j < 10; j++)
			{
				int a = FTI(ITF(i)*j);
				P[a] = 1;
			}
			pattern.push_back(P);
		}
	}

	int LookUpTable::findnote(bool B[88]) 
	{
		int max = 0;
		int index = 87;
		if (IsSubset(B, prev))
			return prevnote;
		for (int i = 0; i < 88; i++)
		{
			int count = comparePattern(B, i, false);
			if (count > max)
			{
				max = count;
				index = i;
			}
		}
		//
		/*for (int j = 0; j < 88; j++)
		{
			//std::cout << B[j] << " " << pattern[index][j] << std::endl;
		}*/
		//
		return index;
	}

	int LookUpTable::findgeneralnote(bool B[88])//-1 if the same as last note
	{
		int max = 0;
		int index;

		//If still the same note
		if (prevnote > 0 && (IsSubset(B, prev)))
		{
			return -1;
		}

		//If is a new note
		for (int i = 0; i < 12; i++)
		{
			int count = comparePattern(B, i, true);
			if (count > max)
			{
				max = count;
				index = i;
			}
		}

		prevnote = index;
		for (int i = 0; i < 88; i++)
		{
		prev[i] = B[i];
		}
		return index;
	}

	/*int LookUpTable::findgeneralnote(bool B[88])
	{
		int max = 0;
		int index;

		if (prevnote > 0 && (IsSubset(B, prev) || IsSubset(prev, B)))//If still the same note
		{
			for (int i = 0; i < 88; i++)
			{
				prev[i] = B[i];
			}
			return prevnote;
		}
		
		for (int i = 0; i < 88; i++)
		{
			if (prev[i] == 1)
				B[i] = 0;
		}

		for (int i = 0; i < 12; i++)
		{
			int count = comparePattern(B, i, true);
			if (count > max)
			{
				max = count;
				index = i;
			}
		}
		//
		for (int j = 0; j < 88; j++)
		{
			std::cout << B[j] << " " << pattern[index][j] << std::endl;
		}
		
		prevnote = index;
		for (int i = 0; i < 88; i++)
		{
			prev[i] = B[i];
		}
		return index;
	}*/

	void LookUpTable::SetGeneralNotePattern()
	{
		for (int i = 0; i < 12; i++)
		{
			std::vector<int> P(88);
			for (int j = i; j < 88; j += 12)
			{
				for (int k = 0; k < 88; k++)
				{
					if (pattern[j][k] == 1)
						P[k] = 1;					
				}
			}
			general_pattern.push_back(P);
		}
	}


	int LookUpTable::comparePattern(bool B[88], int index, bool general)
	{
		int count = 0;
		if (general)
		{ 
			for (int i = 0; i < 88; i++)
			{
				if (B[i] == general_pattern[index][i])
					count++;
			}
		}
		else
		{
			for (int i = 1; i < 87; i++)
			{
				if (pattern[index][i] == 1 && B[i] == 1)
					count++;
			}
		}
		return count;
	}

	bool LookUpTable::IsSubset(bool current[88], bool previous[88])
	{
		for (int i = 0; i < 88; i++)
		{
			if (current[i] == 1 && previous[i] == 0)
				return false;
		}
		return true;
	}

	double LookUpTable::ITF(int index)
	{
		return notes[index];
	}

	int LookUpTable::FTI(double frequency)
	{
		for (int i = 0; i < 88; i++)
		{
			//std::cout << "notes[i] = " << notes[i] << " frequency = " << frequency << std::endl;
			if (abs(notes[i] - frequency) < 1)
				return i;
		}
		return 87;
	}

	int LookUpTable::closestHarmonic(double frequency)
	{
		int i = 0;
		while (notes[i] < frequency)
			i++;
		if (notes[i] - frequency < frequency - notes[i - 1])
			return i;
		else
			return i - 1;
	}

	std::string LookUpTable::ITS(int index, bool general)
	{
		
		int a = index % 12;
		int b = index / 12;
		std::string out;
		switch (a)
		{
		case 0:
			out = "A";
			break;
		case 1:
			out = "A#";
			break;
		case 2:
			out = "B";
			break;
		case 3:
			out = "C";
			break;
		case 4:
			out = "C#";
			break;
		case 5:
			out = "D";
			break;
		case 6:
			out = "D#";
			break;
		case 7:
			out = "E";
			break;
		case 8:
			out = "F";
			break;
		case 9:
			out = "F#";
			break;
		case 10:
			out = "G";
			break;
		case 11:
			out = "G#";
			break;
		}
		if (general = true)
			return out;
		if (index < 3)
			return out + "0";
		else
			return out + std::to_string(b + 1);
		
	}

