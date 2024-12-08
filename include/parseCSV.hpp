#ifndef PARSECSV_H
#define PRASECSV_H
#include <vector>
std::vector<std::vector<double>> extractVectors();
std::vector<std::vector<double>> extractAccel(std::vector<std::vector<double>>);
std::vector<std::vector<double>> extractOmega(std::vector<double>);
std::vector<std::vector<double>> extractDisplacement(std::vector<std::vector<double>>);
std::vector<std::vector<double>> riemannSum(std::vector<std::vector<double>>);
std::vector<double> riemannSum(std::vector<double>);
std::vector<std::vector<double>> rotateAccel(std::vector<std::vector<double>>, std::vector<double>);
void prepareData(
    std::vector<std::vector<double>>&,
    std::vector<double>&, 
    std::vector<std::vector<double>>&,
    std::vector<std::vector<double>>&
);
#endif
