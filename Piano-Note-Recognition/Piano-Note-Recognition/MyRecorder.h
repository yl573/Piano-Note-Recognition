#pragma once
#include "stdafx.h"
#include <iostream>
#include <vector>
#include "fftw3.h"
#include <LookUpTable.h>
#include <fstream>

struct Harmonic {
	double frequency;
	double amplitude;
};

class MyRecorder : public sf::SoundRecorder
{

private:
	clock_t t1, t2, t3, t4;
	int sampling_frequency = 5;
	fftw_complex *out;
	double *in;
	fftw_plan p;
	int N_in;
	int N_out;
	int F_min = 0;//below 200Hz, 10Hz resolution cannot tell between peaks 
	int F_max = 2500;// has to be < N_out*sampling_frequency (5010)
	std::vector<double> noise;
	int padding = 0, padcount = 0;
	int noteStart = 0;
	LookUpTable* T;
	std::ofstream myfile;

	bool isCloser(double x, double bottom, double top);
	double findmean(std::vector<double>& VecIn) const;
	double findmean(std::vector<Harmonic>& Hin) const;
	void printHarmonics(std::vector<Harmonic>& Vin);

	inline bool isIN(std::vector<Harmonic>& Vin, int F)
	{
		for (int i = 0; i < Vin.size(); i++)
		{
			if (Vin[i].frequency == F)
				return true;
		}
		return false;
	}
	inline int roundTen(double x)
	{
		int a = ((int)x + 5) / 10;
		return 10 * a;
	}


public:
	virtual bool onStart();
	virtual bool onProcessSamples(const short* samples, std::size_t sampleCount);
	virtual void onStop();
	void recordNoise(std::vector<double>& Vin);
	std::vector<Harmonic> sortIntoHarmonics(std::vector<double>& Vin);
};