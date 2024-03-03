#include <iostream>

#include "Interface.h"
#include "ATSP.h"


void Interface::menu() {

    ATSP atsp;
    char menuOption;

    string fileName;
    string maxExecutionTime;
    string populationSize;
    string crossingMethod;
    string crossoverRate;
    string mutationRate;

    do {

        cout << endl << "-----MENU GLOWNE-----\n";
        cout << "[1] Wczytanie danych z pliku\n";
        cout << "[2] Wyswietlenie wczytanych danych\n";
        cout << "[3] Ustawienie parametrow\n";
        cout << "[4] Algorytm Genetyczny (GA)\n";
        cout << "[0] Wyjscie\n";

        cout << "Twoj wybor:";
        cin >> menuOption;

        switch (menuOption) {

            case '0':
                cout << "\nWyjscie\n";
                break;

            case '1':
                cout << "\nOpcja 1: Wczytanie danych z pliku\n";
                // Pliki muszą znajdować się w tym samym folderze co plik .exe,
                // oraz muszą mieć charakterystyczną budowę zgodą ze specyfikacją określoną
                // na stronie: http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/atsp/
                cout << "Podaj nazwe pliku wraz z rozszerzeniem (np. ftv47.atsp):";
                cin >> fileName;
                atsp.loadATSPFile(fileName);
                break;

            case '2':
                cout << "\nOpcja 3: Wyswietlenie wczytanych danych\n";
                atsp.printDistanceMatrix();
                break;

            case '3':
                char parameterMenuOption;
                char crossingMethodOption;

                do {
                    cout << endl << "\n-----USTAWIENIE PARAMETROW-----\n";
                    cout << "[1] Kryterium stopu\n";
                    cout << "[2] Wielkosc populacji\n";
                    cout << "[3] Metoda krzyzowania\n";
                    cout << "[4] Wspolczynnik krzyzowania\n";
                    cout << "[5] Wspolczynnik mutacji\n";
                    cout << "[0] Wyjscie\n";

                    cout << "Twoj wybor:";
                    cin >> parameterMenuOption;

                    switch (parameterMenuOption) {
                        case '0':
                            cout << "\nWyjscie\n";
                            break;

                        case '1':
                            cout << "\nOpcja 1: Kryterium stopu\n";
                            cout << "Podaj maksymalny czas wykonania [s] (np. 60):";
                            cin >> maxExecutionTime;
                            break;

                        case '2':
                            cout << "\nOpcja 2: Wielkosc populacji\n";
                            cout << "Podaj wielkosc populacji (np. 100):";
                            cin >> populationSize;
                            break;

                        case '3':
                            cout << "\nOpcja 3: Metoda krzyzowania\n";
                            cout << "[1] Metoda OX\n";
                            cout << "[2] Metoda PMX\n";

                            cout << "Twoj wybor (np. 1):";
                            cin >> crossingMethodOption;

                            if (crossingMethodOption == '1') {
                                crossingMethod = "OX";
                            } else if (crossingMethodOption == '2') {
                                crossingMethod = "PMX";
                            }
                            break;

                        case '4':
                            cout << "\nOpcja 4: Wspolczynnik krzyzowania\n";
                            cout << "Podaj wspolczynnik krzyzowania (np. 0.8):";
                            cin >> crossoverRate;
                            break;

                        case '5':
                            cout << "\nOpcja 5: Wspolczynnik mutacji\n";
                            cout << "Podaj wspolczynnik mutacji (np. 0.01):";
                            cin >> mutationRate;
                            break;

                        default:
                            cout << "\nPodana opcja nie istnieje!\n";
                            break;
                    }

                } while (parameterMenuOption != '0');
                break;

            case '4':
                cout << "\nOpcja 4: Algorytm Genetyczny (GA)\n";
                cout << "--------------------------------" << endl;
                cout << "Nazwa pliku: " << fileName << endl;
                cout << "Kryterium stopu: " << maxExecutionTime << "s" << endl;
                cout << "Wielkosc populacji: " << populationSize << endl;
                cout << "Metoda krzyzowania: " << crossingMethod << endl;
                cout << "Wspolczynnik krzyzowania: " << crossoverRate << endl;
                cout << "Metoda mutacji: Insertion" << endl;
                cout << "Wspolczynnik mutacji: " << mutationRate << endl;
                cout << "--------------------------------" << endl;

                atsp.geneticAlgorithm(fileName, crossingMethod, maxExecutionTime, populationSize, crossoverRate,
                                      mutationRate);
                break;

            default:
                cout << "\nPodana opcja nie istnieje!\n";
                break;
        }

    } while (menuOption != '0');
}

