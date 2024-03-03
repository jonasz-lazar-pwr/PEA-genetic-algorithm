#include <windows.h>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <random>
#include <unordered_map>

#include "ATSP.h"

// Funkcja inicjalizująca macierz odległości
void ATSP::initializeDistanceMatrix(const int& newDimension) {
    // Uaktualnienie nowego rozmiaru macierzy odległości
    V = newDimension;

    // Inicjalizacja macierzy o odpowiednich wymiarach
    distanceMatrix = vector<vector<int>>(V, vector<int>(V));
}

// Funkcja pomocnicza służąca do czyszczenia macierzy
void ATSP::clearDistanceMatrix() {
    // Zerowanie rozmiaru macierzy odległości
    V = 0;

    // Czyszczenie macierzy odległości
    for (auto& innerVector : distanceMatrix) {
        innerVector.clear();
    }
    distanceMatrix.clear();
}

// Funkcja wyświetlająca zawartość macierzy odległości
void ATSP::printDistanceMatrix() {

    // Ustawienie szerokość wyświetlanego pola
    int fieldWidth = 4;

    // Pętle iterujące po każdym elemencie macierzy
    for (const auto& row: distanceMatrix) {
        for (const auto& distance: row) {
            cout << setw(fieldWidth) << distance;
        }
        cout << endl;
    }
}

// Funkcja służąca do wczytywania pliku ATSP do tablicy dwuwymiarowej
void ATSP::loadATSPFile(const string& fileName) {

    //Otwórz plik do odczytu
    ifstream file;
    file.open(fileName, ifstream::in);

    if (file.is_open()) {
        string line;

        // Pętla iterująca po każdym wierszu tekstu w pliku
        while (getline(file, line)) {

            // Sprawdzenie, czy w obecnej linii znajduje się informacja o rozmiarze grafu
            if (line.find("DIMENSION: ") != string::npos) {
                size_t pos = line.find(": ");

                // Inicjalizacja macierzy odległości na podstawie rozmiaru
                initializeDistanceMatrix(stoi(line.substr(pos + 1)));

                // Rozpoczęcie procesu wczytywania odległości do macierzy
            } else if (line.find("EDGE_WEIGHT_SECTION") != string::npos) {
                for (int i = 0; i < V; i++) {
                    for (int j = 0; j < V; j++) {
                        file >> distanceMatrix[i][j];
                        if (i == j) {
                            // Ustawianie -1 na głównej przekątnej
                            distanceMatrix[i][j] = -1;
                        }
                    }
                }
            }
        }
    } else {
        // Komunikat o błędzie w przypadku problemu z otwarciem pliku
        cerr << "ERROR while opening the file." << endl;
    }
    file.close();
}

// Metoda do uruchamiania algorytmu genetycznego dla problemu ATSP
void ATSP::geneticAlgorithm(const string& fileName,
                            const string& crossingMethod,
                            const string& maxExecutionTimeFactor,
                            const string& populationSizeFactor,
                            const string& crossoverRateFactor,
                            const string& mutationRateFactor) {

    // Pomiar częstotliwości zegara
    long long int frequency, startTime, endTime;
    QueryPerformanceFrequency((LARGE_INTEGER *) &frequency);

    // Konwersja parametrów wejściowych na odpowiednie typy
    const double maxExecutionTime = stod(maxExecutionTimeFactor);
    const int populationSize = stoi(populationSizeFactor);
    const double crossoverRate = stod(crossoverRateFactor);
    const double mutationRate = stod(mutationRateFactor);

    // Aktualna populacja chromosomów
    vector<vector<int>> currentPopulation;

    // Wartości przystosowania dla każdego chromosomu w populacji
    vector<double> fitnessValues;

    // Inicjalizacja wektora przechowującego prawdopodobieństwa
    vector<double> probabilities;

    // Inicjalizacja wektora wektorów przechowującego rodziców
    vector<vector<int>> parents;

    // Inicjalizacja wektora wektorów przechowującego potomstwo
    vector<vector<int>> offspring;

    // Inicjalizacja populacji początkowej
    for (int i = 0; i < populationSize; i++) {
        vector<int> chromosome;
        generateRandomChromosome(chromosome);
        currentPopulation.push_back(chromosome);
    }

    // Sortowanie populacji początkowej względem kosztu trasy
    sortByCost(currentPopulation);

    // Inicjalizacja wektora przechowującego najlepszy chromosom (trasę)
    vector<int> bestChromosome = currentPopulation[0];

    // Początkowy czas wykonania algorytmu
    startTime = read_QPC();

    // Pętla główna algorytmu, wykonująca się do momentu przekroczenia czasu wykonania
    while (((1.0 * (read_QPC() - startTime)) / frequency) <= maxExecutionTime) {

        // Obliczenie wartości przystosowania dla każdego chromosomu w populacji
        double fitnessSum = 0.0;

        for (const auto &chromosome: currentPopulation) {
            double fitnessValue = (1.0 / calculateCost(chromosome));
            fitnessSum += fitnessValue;
            fitnessValues.push_back(fitnessValue);
        }

        // Na podstawie przystosowania, obliczenie wartości prawdopodobieństwa osobnika
        double sumOfProbabilities = 0.0;

        for (const auto value: fitnessValues) {
            double probability = sumOfProbabilities + (value / fitnessSum);
            sumOfProbabilities = probability;
            probabilities.push_back(probability);
        }

        // Selekcja rodziców na podstawie funkcji przystosowania (ruletka)
        int parentIndex;
        for (int i = 0; i < populationSize; i++) {

            // Wylosowanie rodzica
            parentIndex = rouletteWheel(probabilities);

            // Dodaj wybranych rodziców do wektora rodziców
            parents.push_back(currentPopulation[parentIndex]);
        }

        // Krzyżowanie (crossover)
        for (int i = 0; i < parents.size(); i++) {

            if (generateRandomDouble(0.0, 1.0) <= crossoverRate) {

                vector<int> child;

                // Wybór metody krzyżowania (OX lub PMX)
                if (crossingMethod == "OX") {
                    child = crossoverOX(parents[i], parents[(i + 1) % parents.size()]);
                } else if (crossingMethod == "PMX") {
                    child = crossoverPMX(parents[i], parents[(i + 1) % parents.size()]);
                }

                // Dodaj potomstwo do nowej populacji
                offspring.push_back(child);

            } else {
                // Jeśli nie krzyżujemy, to skopiuj rodziców do potomstwa
                offspring.push_back(parents[i]);
            }
        }

        // Mutacja (mutation)
        for (auto &chromosome: offspring) {

            // Sprawdzenie czy ma zajść mutacja na podstawie współczynnika mutacji
            if (generateRandomDouble(0.0, 1.0) <= mutationRate) {
                // Wywołanie funkcji mutacji wstawieniowej
                insertionMutation(chromosome);
            }
        }

        // Sukcesja (wybranie najlepszej puli osobników z dwóch pokoleń)
        succession(currentPopulation, parents, offspring);

        // Aktualizacja najlepszego chromosomu, jeśli znaleziono lepszy
        if (calculateCost(currentPopulation[0]) < calculateCost(bestChromosome)) {
            bestChromosome = currentPopulation[0];
        }

        // Wyczyszczenie wektorów pomocniczych przed kolejną iteracją
        fitnessValues.clear();
        probabilities.clear();
        parents.clear();
        offspring.clear();
    }

    // Zakończenie pomiaru czasu
    endTime = read_QPC();

    // Wyświetlenie wyników
    cout << "Najlepsza trasa znaleziona algorytmem GA: ";
    for (int gene : bestChromosome) {
        cout << gene << " -> ";
    }
    cout << bestChromosome[0] << endl;

    cout << "--------------------------------" << endl;
    cout << "Koszt najlepszej trasy: " << calculateCost(bestChromosome) << endl;
    cout << "--------------------------------" << endl;
    cout << "Czas wykonania: " << ((1.0 * (endTime - startTime)) / frequency) << "s" << endl;
    cout << "--------------------------------" << endl;
    cout << endl;
}

// Metoda generująca jedną losową drogę (chromosom)
void ATSP::generateRandomChromosome(vector<int>& chromosome) {

    // Inicjalizacja wektora kolejnych numerów miast
    for (int i = 0; i < V; i++) {
        chromosome.push_back(i);
    }

    // Inicjalizacja generatora liczb pseudolosowych
    random_device rd;
    mt19937 generator(rd());

    // Mieszanie wektora w losowej kolejności
    shuffle(chromosome.begin(), chromosome.end(), generator);
}

// Metoda do sortowania malejącego wektora osobników w populacji
void ATSP::sortByCost(vector<vector<int>>& population) {
    sort(population.begin(), population.end(), [this](const auto &a, const auto &b) {
        return calculateCost(a) < calculateCost(b);
    });
}

// Metoda oblaczająca koszt drogi
int ATSP::calculateCost(const vector<int>& chromosome) {
    int cost = 0;

    for (int i = 0; i < V - 1; ++i) {
        cost += distanceMatrix[chromosome[i]][chromosome[i + 1]];
    }

    // Dodanie kosztu powrotu do pierwszego miasta (cyklu)
    cost += distanceMatrix[chromosome[V - 1]][chromosome[0]];

    return cost;
}

// Metoda do wyboru osobnika na podstawie prawdopodobieństw (metoda koła ruletki)
int ATSP::rouletteWheel(const vector<double>& probabilities) {

    double randomValue = generateRandomDouble(0.0, 1.0);

    // Znajdź osobnika, którego przedział zawiera wylosowaną wartość
    for (int i = 0; i < probabilities.size(); i++) {

        if (randomValue <= probabilities[i]) {
            return i;  // Wybrano osobnika i
        }
    }

    // Jeśli nie wybrano żadnego osobnika (coś poszło nie tak), zwróć -1
    return -1;
}

// Metoda krzyżowania OX (Order Crossover)
vector<int> ATSP::crossoverOX(const vector<int>& parent1, const vector<int>& parent2) {

    int size = parent1.size();

    // Stwórz potomka, inicjalizując go wartościami specjalnymi
    vector<int> child(size, -1);

    // Wygeneruj dwa punkty cięcia losowo
    int cuttingPoint1 = generateRandomInteger(0, size - 1);
    int cuttingPoint2 = generateRandomInteger(0, size - 1);

    // Upewnij się, że punkty cięcia są różne
    while (cuttingPoint1 == cuttingPoint2) {
        cuttingPoint2 = generateRandomInteger(0, size - 1);
    }

    // Upewnij się, że cuttingPoint1 < cuttingPoint2
    if (cuttingPoint1 > cuttingPoint2) {
        swap(cuttingPoint1, cuttingPoint2);
    }

    // Skopiuj segment od rodzica P1 do potomka
    copy(parent1.begin() + cuttingPoint1, parent1.begin() + cuttingPoint2 + 1, child.begin() + cuttingPoint1);

    // Wypełnij resztę potomka genami z rodzica P2 w kolejności, pomijając istniejące geny
    int index = cuttingPoint2 + 1;

    // Sprawdź każdy gen z rodzica P2 w kolejności
    for (int i = cuttingPoint2 + 1; i <= cuttingPoint2 + size; i++) {
        int gene = parent2[i % size];

        // Sprawdź, czy gen nie jest już w potomku
        if (find(child.begin(), child.end(), gene) == child.end()) {
            child[index % size] = gene;
            index++;
        }
    }

    return child;
}

// Metoda krzyżowania PMX (Partially Matched Crossover)
vector<int> ATSP::crossoverPMX(const vector<int>& parent1, const vector<int>& parent2) {

    int size = parent1.size();

    // Stwórz dwa potomków, zainicjalizowane genami rodziców
    vector<int> child1 = parent1;
    vector<int> child2 = parent2;

    // Mapa przechowująca odwzorowanie genów pomiędzy rodzicami
    unordered_map<int, int> mapping1;
    unordered_map<int, int> mapping2;

    // Wygeneruj dwa punkty cięcia losowo
    int cuttingPoint1 = generateRandomInteger(0, size - 1);
    int cuttingPoint2 = generateRandomInteger(0, size - 1);

    // Upewnij się, że punkty cięcia są różne
    while (cuttingPoint1 == cuttingPoint2) {
        cuttingPoint2 = generateRandomInteger(0, size - 1);
    }

    // Ustaw punkt początkowy i końcowy dla krzyżowania
    if (cuttingPoint1 > cuttingPoint2) {
        swap(cuttingPoint1, cuttingPoint2);
    }

    // Przeprowadź mapowanie genów pomiędzy rodzicami w obszarze cięcia
    for (int i = cuttingPoint1; i < cuttingPoint2; ++i) {
        mapping1[parent2[i]] = parent1[i];
        child1[i] = parent2[i];

        mapping2[parent1[i]] = parent2[i];
        child2[i] = parent1[i];
    }

    // Napraw obszar poniżej cięcia w potomku 1
    for (int i = 0; i < cuttingPoint1; ++i) {
        while (mapping1.find(child1[i]) != mapping1.end()) {
            child1[i] = mapping1[child1[i]];
        }
        while (mapping2.find(child2[i]) != mapping2.end()) {
            child2[i] = mapping2[child2[i]];
        }
    }

    // Napraw obszar powyżej cięcia w potomku 1
    for (int i = cuttingPoint2; i < child1.size(); ++i) {
        while (mapping1.find(child1[i]) != mapping1.end()) {
            child1[i] = mapping1[child1[i]];
        }
        while (mapping2.find(child2[i]) != mapping2.end()) {
            child2[i] = mapping2[child2[i]];
        }
    }

    return child1;
}

// Mutacja przez wstawienie (Insertion Mutation)
void ATSP::insertionMutation(vector<int>& chromosome) {

    int size = chromosome.size();

    // Wybierz dwa punkty mutacji losowo
    int mutationPoint1 = generateRandomInteger(0, size - 1);
    int mutationPoint2 = generateRandomInteger(0, size - 1);

    // Upewnij się, że punkty mutacji są różne
    while (mutationPoint1 == mutationPoint2) {
        mutationPoint2 = generateRandomInteger(0, size - 1);
    }

    // Wybierz gen do wstawienia
    int geneToInsert = chromosome[mutationPoint1];

    // Usuń gen z punktu mutacji 1
    chromosome.erase(chromosome.begin() + mutationPoint1);

    // Wstaw gen do punktu mutacji 2
    chromosome.insert(chromosome.begin() + mutationPoint2, geneToInsert);
}

// Metoda do zastępowania gorszych osobników w populacji aktualnej przez lepsze z potomstwa
void ATSP::succession(vector<vector<int>>& currentPopulation, const vector<vector<int>>& parents, const vector<vector<int>>& offspring) {

    int size = currentPopulation.size();

    // Połącz populację rodziców i potomstwa
    currentPopulation = parents;
    currentPopulation.insert(currentPopulation.end(), offspring.begin(), offspring.end());

    // Posortuj po koszcie (od najmniejszego do największego)
    sortByCost(currentPopulation);

    // Wybierz najlepsze osobniki do nowej populacji aktualnej
    currentPopulation.resize(size);
}

// Funkcja do generowania liczby czałkowitej z wybranego zakresu
int ATSP::generateRandomInteger(int min, int max) {

    // Inicjalizacja generatora liczb pseudolosowych
    random_device rd;
    mt19937 generator(rd());

    // Definicja rozkładu prawdopodobieństwa z zakresu [min, max]
    uniform_int_distribution<> distribution(min, max);

    // Wygenerowanie liczby zmiennoprzecinkowej z zakresu [min, max]
    return distribution(generator);
}

// Funkcja do generowania liczby zmiennoprzecinkowej z wybranego zakresu
double ATSP::generateRandomDouble(double min, double max) {

    // Inicjalizacja generatora liczb pseudolosowych
    random_device rd;
    mt19937_64 generator(rd());

    // Definicja rozkładu prawdopodobieństwa z zakresu [min, max]
    uniform_real_distribution<double> distribution(min, max);

    // Wygenerowanie liczby zmiennoprzecinkowej z zakresu [min, max]
    return distribution(generator);
}

long long int ATSP::read_QPC() {
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    return((long long int)count.QuadPart);
}
