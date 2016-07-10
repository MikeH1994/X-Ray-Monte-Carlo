#ifndef _H_DATAFROMFILE_H_
#define _H_DATAFROMFILE_H_

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <istream>
#include <fstream>
#include <cmath>
#include "RandomGenerator.h"


class DataFromFile {
public:
	DataFromFile(std::string filePath) {
		rand = RandomGenerator(4);
		parseFile(filePath);
		getMax();
	}
	int size() {
		return data_y.size();
	}
	double x(int index) {
		return data_x[index];
	}
	double y(int index) {
		return data_y[index];
	}
	double val(double value) {
		int index = getIndex(value);
		return interpolate(index, value);
	}
	double getMaxX() {
		return max_x;
	}
	double getMaxY() {
		return max_y;
	}
	double getPhoton() {
		// get the energy of a photon that passes the acceptance rejection technique (ie f>curve height) for a synchrotron file 
		double r1;
		double r2;
		double E;
		double f;
		while (true) {
			r1 = this->rand.getRand();
			r2 = this->rand.getRand();
			E = r1 * this->max_x;
			f = r2 * this->max_y;
			if (f < val(E)) {
				break;
			}
		}
		return E;
	}
	double getPhotonFromAttenuation(double length, DataFromFile* synchrotron, int & counter) {
		// get a photon that manages to pass through a material of a given length using equation 
		//1 - exp(-x/lambda(E)) ,where x is the length of the material
		length *= 1E6;//convering from m to microns
		double lambda;
		double E;
		double probability;
		double r;
		while (counter>0) {
			counter--;
			r = this->rand.getRand();
			E = synchrotron->getPhoton();
			lambda = this->val(E);
			probability = 1 - exp(-length / lambda);
			if (r > probability) {
				break;
			}
		}
		if (counter<1) {
			throw (std::string) "NoMorePhotons";
		}
		return E;
	}
	double getPhotonFromAttenuation(double length, DataFromFile* synchrotron) {
		// get a photon that manages to pass through a material of a given length using equation 1 - exp(-x/lambda(E)) 
		// where x is the length of the material
		length *= 1E6;
		double lambda;
		double E;
		double probability;
		double r;
		while (true) {
			r = this->rand.getRand();
			E = synchrotron->getPhoton();
			lambda = this->val(E);
			probability = 1 - exp(-length / lambda);
			if (r > probability) {
				break;
			}
		}
		return E;
	}
	double getAreaUnderData() {
		double area = 0;
		for (int i = 1; i < size(); i++) {
			area += (x(i) - x(i - 1))*(y(i) + y(i - 1));
		}
		area /= 2;
		return area;
	}
	void print() {
		for (int i = 0; i < size(); i++) {
			std::cout << x(i) << ", " << y(i) << std::endl;
		}
	}
	void scalePoints(double scale) {
		for (int i = 0; i < size(); i++) {
			data_y[i] /= scale;
		}
		getMax();
	}
	void normaliseData() {
		double area = getAreaUnderData();
		scalePoints(area);
	}
private:
	RandomGenerator rand;
	std::vector<double> data_x;
	std::vector<double> data_y;
	double max_x;
	double max_y;
	void parseFile(std::string filePath) {
		std::ifstream f(filePath.c_str(), std::ifstream::in);
		if (f.fail()) {
			std::cout << "Was unable to open file" << std::endl;
			throw (std::string) "IOException in parseFile()";
		}

		std::istringstream instream;       // Declare an input string stream
		std::string line;
		int lineNum = 0;
		while (getline(f, line)) {
			bool useLine = !(line.substr(0, 1) == "#" || line.substr(0, 1) == "!");
			if (useLine) {
				instream.clear();          // Reset from possible previous errors
				instream.str(line);        // Use line as source of input
				double x, y;
				lineNum++;
				instream >> x >> y;
				data_x.push_back(x);
				data_y.push_back(y);
			}
		}
		f.close();
	}
	int getIndex(double key) {
		// returns the index i such that x(index-1)<_x<x(index)
		return getIndexBinary(key, 0, size() - 1);
	}
	int getIndexBinary(double key, int min, int max) {
		int midPoint = (min + max) / 2;
		if (midPoint <= 0) {
			return 1;
		}
		if (midPoint >= max - 1) {
			return max;
		}

		if (key > x(midPoint)) {
			if (key < x(midPoint + 1)) {
				return midPoint + 1;
			}
			else return getIndexBinary(key, midPoint + 1, max);
		}
		if (key < x(midPoint)) {
			if (key > x(midPoint - 1)) {
				return midPoint;
			}
			else return getIndexBinary(key, min, midPoint - 1);
		}
		throw "error in getIndexBinary()";
	}
	void getMax() {
		double maxX = x(size() - 1);
		double maxY = y(0);

		for (int i = 0; i < size(); i++) {
			if (y(i) > maxY) {
				maxY = y(i);
			}
		}
		this->max_x = maxX;
		this->max_y = maxY;
	}
	double interpolate(int index, double value) {
		double slope;
		double val = 0;
		if (index == 0) {
			slope = y(index) / x(index);
			val = value*slope;
		}
		else {
			slope = (y(index) - y(index - 1)) / (x(index) - x(index - 1));
			val = y(index - 1) + (value - x(index - 1))*slope;
		}
		return val;
	}
};


#endif
