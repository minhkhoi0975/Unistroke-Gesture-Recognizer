#pragma once
#include <random>
#include <ctime>


class Random
{
private:
	// random_device device;  // No good.
	std::mt19937 generator;

public:

	Random()
	{
		generator.seed((unsigned int)(time(nullptr) + (int)this));
	}

	void Seed(int newSeed)
	{
		generator.seed(newSeed);
	}

	int Integer(int low, int high)
	{
		std::uniform_int_distribution<int> distribution(low, high);
		return distribution(generator);
	}

	float Float(float low, float high)
	{
		std::uniform_real_distribution<float> distribution(low, high);
		return distribution(generator);
	}

};
