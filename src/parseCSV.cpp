#include "parseCSV.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

std::vector<std::vector<double>> extractVectors()
{
    std::ifstream file("data.csv");
    std::string line;
    std::vector<std::vector<double>> ret;
    std::getline(file, line);
    int count = 0;
    while (std::getline(file, line)) {
	if (count == 500) {
	    break;
	}
        std::istringstream lineStream(line);
	std::string value;
	std::vector<double> vec;
	while (std::getline(lineStream, value, ',')) {
	    vec.push_back(std::stod(value));
	}
	ret.push_back(vec);
	count++;
    }
    return ret;
}

std::vector<std::vector<double>> extractAccel(std::vector<std::vector<double>> data)
{
    std::vector<std::vector<double>> ret;
    for (auto i = data.begin(); i != data.end(); i++) {
        std::vector<double> vec = {i->at(1), i->at(2)};
	ret.push_back(vec);
    }
    return ret;
}

std::vector<double> extractOmega(std::vector<std::vector<double>> data)
{
    std::vector<double> ret;
    for (auto i = data.begin(); i != data.end(); i++) {
        double vec = i->at(6);
	ret.push_back(vec);
    }
    return ret;
}

std::vector<std::vector<double>> extractDisplacement(std::vector<std::vector<double>> data)
{
    std::vector<std::vector<double>> ret;
    for (auto i = data.begin(); i != data.end(); i++) {
        std::vector<double> vec = {i->at(7), i->at(8)};
	ret.push_back(vec);
    }
    return ret;
}

std::vector<std::vector<double>> riemannSum(std::vector<std::vector<double>> data)
{
    std::vector<std::vector<double>> ret;
    int count = 0;
    for (auto i = data.begin(); i != data.end(); i++) {
	double partialX = 0.01 * i->at(0);
	double partialY = 0.01 * i->at(1);
	if (count > 0) {
	    partialX += ret.at(count - 1).at(0);
	    partialY += ret.at(count - 1).at(1);
	}
	std::vector<double> vec = {partialX, partialY};
	ret.push_back(vec);
	count++;
    }
    return ret;
}

std::vector<double> riemannSum(std::vector<double> data)
{
    std::vector<double> ret;
    int count = 0;
    for (auto i = data.begin(); i != data.end(); i++) {
	double partialX = 0.01 * (*i);
	if (count > 0) {
	    partialX += ret.at(count - 1);
	}
	double vec = partialX;
	ret.push_back(vec);
	count++;
    }
    return ret;
}
std::vector<std::vector<double>> rotateAccel(std::vector<std::vector<double>> accel, std::vector<double> omega)
{
    std::vector<std::vector<double>> ret;
    std::vector<double> theta = riemannSum(omega);
    auto i = theta.begin();
    auto j = accel.begin();
    for (; i != theta.end() && j != accel.end(); i++, j++) {
        double accelX = cos(-1 * (*i)) * j->at(0)  - sin(-1 * (*i)) * j->at(1);
        double accelY = sin(-1 * (*i)) * j->at(0)  + cos(-1 * (*i)) * j->at(1);
	std::vector<double> vec = {accelX, accelY};
	ret.push_back(vec);
    }
    return ret;
}

void prepareData(std::vector<std::vector<double>> &accel, std::vector<double> &omega, std::vector<std::vector<double>> &displacement, std::vector<std::vector<double>> &trueAccel)
{
    std::vector<std::vector<double>> raw = extractVectors();
    std::cout << "Read file!" << std::endl;
    omega = extractOmega(raw);
    std::cout << "Loaed omegas!" << std::endl;
    accel = extractAccel(raw);
    std::cout << "Loaded accel!" << std::endl;
    accel = rotateAccel(accel, omega);
    std::cout << "Loaded rotation!" << std::endl;
    displacement = extractDisplacement(raw);
    std::cout << "Loaded displacement!" << std::endl;
    trueAccel = extractAccel(raw);
    std::cout << "Loaded truth!" << std::endl;
}

