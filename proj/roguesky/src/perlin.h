#ifndef perlin_H
#define perlin_H

#include <vector>

class perlin {
	// The permutation vector
	std::vector<int> p;
public:
	// Initialize with the reference values for the permutation vector
	perlin();
	// Generate a new permutation vector based on the value of seed
	perlin(unsigned int seed);
	// Get a noise value, for 2D images z can have any value
	double noise(double x, double y, double z);
private:
	double fade(double t);
	double lerp(double t, double a, double b);
	double grad(int hash, double x, double y, double z);
};

#endif
