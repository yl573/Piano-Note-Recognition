#pragma once
#include "stdafx.h"
#include <iostream>
#include <vector>

class LookUpTable
{
	double notes[88];
	std::vector<std::vector<int>> pattern;
	std::vector<std::vector<int>> general_pattern;
	bool prev[88];
	int prevnote = -1;
	int comparePattern(bool B[88], int index, bool general);
	int notestart = 33;
	void SetGeneralNotePattern();
	bool IsSubset(bool current[88], bool previous[88]);


public:
	LookUpTable();
	void setPatterns();
	double ITF(int index);
	int FTI(double frequency);
	std::string ITS(int index, bool general);
	int findnote(bool[88]);
	int findgeneralnote(bool B[88]);
	int closestHarmonic(double frequency);
};
