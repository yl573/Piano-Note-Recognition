#include "stdafx.h"
#include <SFML/Audio.hpp>
#include <iostream>
#include "fftw3.h"
#include <time.h>
#include <cmath>
#include <vector>
#include <LookUpTable.h>
#include <MyRecorder.h>
#include <fstream>

	bool MyRecorder::onStart()
	{
		sf::Time t1 = sf::milliseconds(1000/sampling_frequency);
		sf::SoundRecorder::setProcessingInterval(t1);
		N_in = getSampleRate() / sampling_frequency;
		N_out = N_in / 2 + 1;
		in = (double*)fftw_malloc(sizeof(double) * N_in);
		out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N_out);
		p = fftw_plan_dft_r2c_1d(N_in, in, out, FFTW_ESTIMATE);
		T = new LookUpTable();
		myfile.open("data.txt");
		myfile << "";
		myfile.close();
		return true;
	}

	bool MyRecorder::onProcessSamples(const short* samples, std::size_t sampleCount)
	{
		int f;
		std::vector<double> y;
		t1 = clock();
		if (padcount < padding)
		{
			padcount++;
			return true;
		}
		for (int i = 0; i < N_in; i++)
		{
			in[i] = (double)samples[i];
		}
		fftw_execute(p);
		double ymax = 0;
		int index = 0;
		
		y.erase(y.begin(), y.begin() + y.size());
		for (int i = 0; i < N_out ; i++)
		{
			f = i*sampling_frequency;
			if (f > F_max)
				break;
			//int i_temp = i + F_min / sampling_frequency;
			double amplitude = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]) / N_out;
			y.push_back(amplitude);
			//std::cout << f << "Hz  A = " << amplitude << std::endl;
		}
		//std::cout << clock() - t3 << std::endl;
		if(clock() - t3 < 3000)
		{
			std::cout << "recording noise..." << std::endl;
			recordNoise(y);
		}
		else
		{
			if (  noise.size() > 0 && findmean(y) > findmean(noise)*2)
			{
				std::vector<Harmonic> h = sortIntoHarmonics(y);
				//printHarmonics(h);
			}
			//detectFrequencies(y);
			//std::vector<int> notes = findNotes(harmonics);
		}

		//if (notes.size() > 0)
		//	std::cout << notes[notes.size()-1] << " is detected" << std::endl;
		t2 = clock();
		//std::cout << "time taken " << (double)(t2 - t1) << std::endl;
		//std::cout << std::endl << std::endl;
		return true;
	}

	void MyRecorder::onStop()
	{
		fftw_destroy_plan(p);
		fftw_free(in);
		fftw_free(out);
	}

	void MyRecorder::recordNoise(std::vector<double>& Vin)
	{
		static int count;
		if (noise.empty())
		{
			count = 1;
			for (int i = 0; i < Vin.size(); i++)
			{
				noise.push_back(Vin[i]);
			}
		}
		else
		{
			for (int i = 0; i < Vin.size(); i++)
			{
				noise[i] = (noise[i] * count + Vin[i]) / (count + 1);
				//std::cout << "noise " << noise[i] << std::endl;
			}
		}
	}

	//Origional sort
	/*
	std::vector<Harmonic> MyRecorder::sortIntoHarmonics(std::vector<double>& Vin)
	{
		std::vector<Harmonic> harmonics(88);
		for (int k = 0; k < harmonics.size(); k++)
		{
			harmonics[k].frequency = T->ITF(k);
			harmonics[k].amplitude = 0;
		}
		double freq;
		int i, j;
		myfile.open("data.txt", std::ios::app);
		for (i = 0,j = 0; (i < Vin.size()) && (j < harmonics.size() - 1); i++)
		{
			freq = i*sampling_frequency;
			if (isCloser(freq, harmonics[j].frequency, harmonics[j + 1].frequency))
			{
				harmonics[j].amplitude += Vin[i];//-noise[i];
				myfile << "freq = " << freq << " amp = " << Vin[i] << " added to " << harmonics[j].frequency << std::endl;
			}
			else
			{
				do 
				{
					j++;
				} while (j < harmonics.size() - 2 && !isCloser(freq, harmonics[j].frequency, harmonics[j + 1].frequency));
				harmonics[j].amplitude += Vin[i];// -noise[i];
				myfile << "freq = " << freq << " amp = " << Vin[i] << " added to " << harmonics[j].frequency << std::endl;
			}
		}
		//printHarmonics(harmonics);
		//double m = findmean(harmonics);
		//std::cout << "mean is " << m << std::endl;
		bool current[88] = { 0 };
		int start = 33;
		for (int b = start; b < harmonics.size()-1; b++)
		{
			if (harmonics[b].amplitude > harmonics[b - 1].amplitude 
				&& harmonics[b].amplitude > harmonics[b + 1].amplitude 
				&& harmonics[b].amplitude > 1500 )//* harmonics[b].frequency)
			{
				//std::cout << "peak found at " << harmonics[b].frequency << "  amplitude is " << harmonics[b].amplitude << std::endl;		
				myfile << "peak found at " << harmonics[b].frequency << "  amplitude is " << harmonics[b].amplitude << std::endl;
				current[b] = 1;
			}
		}
		int index = T->findgeneralnote(current);

		myfile << T->ITS(index, true) << std::endl << std::endl;
		myfile.close();
		if (index > 0)
			std::cout << " the note is " << T->ITS(index, true) << std::endl;
		return harmonics;
	}
	*/

	//New sort
	std::vector<Harmonic> MyRecorder::sortIntoHarmonics(std::vector<double>& Vin)
	{
		std::vector<Harmonic> harmonics(88);
		for (int k = 0; k < harmonics.size(); k++)
		{
			harmonics[k].frequency = T->ITF(k);
			harmonics[k].amplitude = 0;
		}
		double freq;
		int i, j;
		myfile.open("data.txt", std::ios::app);
		bool current[88] = { 0 };
		for (i = 1, j = 0; (i < Vin.size()-1) && (j < harmonics.size() - 1); i++)
		{
			freq = i*sampling_frequency;
			myfile << "freq" << freq << " amp" << Vin[i] << std::endl;
			if (Vin[i] > Vin[i - 1]
				&& Vin[i] > Vin[i + 1]
				&& Vin[i] > 500)//* harmonics[b].frequency)
			{
				j = T->closestHarmonic(freq);
				current[j] = 1;
				myfile << "peak found at " << harmonics[j].frequency << "  amplitude is " << Vin[i] << std::endl;
			}
		}		

		int index = T->findgeneralnote(current);

		myfile << T->ITS(index, true) << std::endl << std::endl;
		myfile.close();
		if (index > 0)
			std::cout << " the note is " << T->ITS(index, true) << std::endl;
		return harmonics;
	}

	bool MyRecorder::isCloser(double x, double bottom, double top)//return true if closer to bottom
	{
		if (x - bottom < top - x)
			return true;
		return false;
	}

	double MyRecorder::findmean(std::vector<double>& VecIn) const
	{
		int  total = 0;
		for (int i = 0; i < VecIn.size(); i++)
		{
			int temp = (int)VecIn[i];
			total += temp;
		}
		return total / VecIn.size();
	}

	double MyRecorder::findmean(std::vector<Harmonic>& Hin) const
	{
		double  total = 0;
		for (int i = 0; i < Hin.size(); i++)
		{
			total += Hin[i].amplitude/Hin[i].frequency;
		}
		return total / Hin.size();
	}

	void MyRecorder::printHarmonics(std::vector<Harmonic>& Vin)
	{
		for (int i = 0; i < Vin.size(); i++)
		{
			std::cout << "frequency = " << Vin[i].frequency << " amplitude = " << Vin[i].amplitude << std::endl;
		}
	}

