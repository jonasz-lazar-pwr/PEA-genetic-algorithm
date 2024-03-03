#ifndef GENETIC_ALGORITHM_ATSP_H
#define GENETIC_ALGORITHM_ATSP_H


#include <iostream>
#include <vector>
#include <set>
#include <limits>

using namespace std;

// Wszystkie metody i ich działanie opisano w pliku ATSP.cpp

class ATSP {
public:
    void initializeDistanceMatrix(const int& newDimension);

    void clearDistanceMatrix();

    void printDistanceMatrix();

    void loadATSPFile(const string& fileName);

    void geneticAlgorithm(const string& fileName,
                          const string& crossingMethod,
                          const string& maxExecutionTimeFactor,
                          const string& populationSizeFactor,
                          const string& crossoverRateFactor,
                          const string& mutationRateFactor);

private:
    // Zmienna określająca rozmiar problemu (liczbę miast)
    int V;

    // Macierz przechowująca odległości między miastami
    vector<vector<int>> distanceMatrix;

    void generateRandomChromosome(vector<int>& chromosome);

    void sortByCost(vector<vector<int>> &population);

    int calculateCost(const vector<int>& chromosome);

    int rouletteWheel(const vector<double> &probabilities);

    vector<int> crossoverOX(const vector<int>& parent1, const vector<int>& parent2);

    vector<int> crossoverPMX(const vector<int> &parent1, const vector<int> &parent2);

    void insertionMutation(vector<int> &chromosome);

    void succession(vector<vector<int>> &currentPopulation,
                    const vector<vector<int>> &parents,
                    const vector<vector<int>> &offspring);

    static int generateRandomInteger(int min, int max);

    static double generateRandomDouble(double min, double max);

    static long long int read_QPC();
};


#endif //GENETIC_ALGORITHM_ATSP_H
