#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <time.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <random>

using namespace std;

vector<char> wynik_DNA;
vector<int> wymieszane_wierzcholki;
vector<pair<int,int>> ulozone_wierzcholki;
vector<int> nie_pasuje_bo_uzyty;
vector<int> zuzyte_wierzcholki_id;

struct Mrowka {
    std::vector<int> sciezka;
    float feromony;
};

vector<Mrowka> mrowki;

int main() {
    bool czy_generowac = true; // jesli chcemy wygenerowac nowy lancuch
    //PARAMETRY
    char zasady_azotowe[4] = {'A', 'T', 'C', 'G'};
    float dlugosc_DNA = 80;
    dlugosc_DNA = dlugosc_DNA + 1;
    float dlugosc_okienka = 4;
    float procent_pozytywnych = 10;
    float procent_negatywnych = 10;
    float wspolczynnik_wyparowywania = 0.1;
    int liczba_iteracji = 5;

    bool dodano = false;

    vector<char> lancuch_DNA;
    int rand_index = rand() % +4;

    if (czy_generowac == true) {

        ofstream plik("wygenerowany_lancuch.txt");

        if (!plik.is_open()) {
            cerr << "Nie można otworzyć pliku do zapisu." << endl;
            return 1;
        }
        //GENEROWANIE LANCUCHA DNA

        srand(time(NULL));
        cout << "Wygenerowany lancuch: " << endl;
        for (int i = 0; i < dlugosc_DNA; i++) {
            rand_index = rand() % +4;
            lancuch_DNA.push_back(zasady_azotowe[rand_index]);
            cout << lancuch_DNA[i] << " ";
            plik << lancuch_DNA[i];
        }

        plik.close();

        cout << endl;
    } else {
        ifstream plik("wygenerowany_lancuch.txt");

        char znak;
        while (plik >> znak) {
            lancuch_DNA.push_back(znak);
        }

        // Zamknij plik
        plik.close();
    }



    //GENEROWANIE OKIENEK
    vector<vector<char>> okienka;
    vector<char> okienka_temp;
    vector<vector<char>> okienka_kopia;
    int liczba_okienek = dlugosc_DNA - (dlugosc_okienka - 1);
    int index = 0;
    for (int i = 0; i < liczba_okienek; i++) //tyle jest okienek
    {
        index = i;
        for (int j = 0; j < dlugosc_okienka; j++) //taka maja dlugosc okienka
        {
            okienka_temp.push_back(lancuch_DNA[index]);
            index++;
        }
        okienka.push_back(okienka_temp);
        okienka_temp.clear();
    }



    //GENEROWANIE BLEDOW NEGATYWNYCH - USUWANIE NUKLEOTYDOW
    float liczba_bledow_negatywnych = round(liczba_okienek * (procent_negatywnych / 100));
    for (int i = 0; i < liczba_bledow_negatywnych; i++) {
        rand_index = rand() % +(liczba_okienek);
        std::swap(okienka[rand_index], okienka[liczba_okienek - 1]);
        okienka.pop_back();
        liczba_okienek--;
    }
    dlugosc_DNA = dlugosc_DNA - liczba_bledow_negatywnych;


    //GENEROWANIE BLEDOW POZYTYWNYCH - DODAWANIE NUKLEOTYDOW
    int rand2_index;
    float liczba_bledow_pozytywnych = round(liczba_okienek * (procent_pozytywnych / 100));
    for (int i = 0; i < liczba_bledow_pozytywnych; i++) {
        for (int j = 0; j < dlugosc_okienka; j++) //generowanie losowego okienka
        {
            rand_index = rand() % +4;
            rand2_index = rand() % +(liczba_okienek - 1);
            okienka_temp.push_back(zasady_azotowe[rand_index]);
        }
        okienka.insert(okienka.begin() + rand2_index, okienka_temp);
        liczba_okienek++;
        okienka_temp.clear();
    }
    dlugosc_DNA = dlugosc_DNA + liczba_bledow_pozytywnych;

    cout << "dlugosc DNA wynosi " << dlugosc_DNA << endl;

    //Wyswietlanie okienek
    cout << endl;
    for (int i = 0; i < liczba_okienek; i++) {
        cout << "Okienko nr: " << i << " | ";
        for (auto it = okienka[i].begin(); it != okienka[i].end(); it++) {
            cout << *it;
        }

        cout << endl;
    }


    // dodanie kopii wektora okienka
    okienka_kopia = okienka;

    //Startowy wierzcholek



    //Kolejne wierzcholki
    int pomoc = 0; // mozna pomyslec nad lepsza nazwa
    int pomoc2 = liczba_okienek; // mozna pomyslec nad lepsza nazwa


    // rozwiazanie z wagami
    // oznaczenie wag kolejnych wierzcholkow dla rozwazanego w danym momencie wierzcholka

    int id_obecnego_wierzcholka = 0; // zeby moc porownac do tego wierzcholka w ponizeszej petli (j)

    vector<int> waga_wierzcholkow;
    vector<int> uzyte_losowe;
    float suma_odleglosci = 3; // bo 1 miedzy wszystkimi polaczeniami 1. okienka; wg tego liczymy trafnosc obranej drogi
    int temp_dlugosc;

    pomoc2 = liczba_okienek;
    waga_wierzcholkow.clear();
    int licznik_ite;
    int id_wierzcholka = 0;
    int licznik_zasad = 0;
    bool nie_zero = false;
    int nadmiar;
    bool czy_cofnieto = false;
    bool czy_mrowka_dotarla;
    int liczba_dodanych_okienek = 0;
    int realna_dlugosc_DNA = dlugosc_okienka;
    int suma_nadlozonej_drogi = 0;
    float max_suma_odleglosci = (dlugosc_DNA - 1) * (dlugosc_okienka - 1) * (dlugosc_okienka - 1);
    float min_suma_odleglosci = 2 * (dlugosc_DNA - 1);

    int iteracja = 0;
    random_device rd;
    mt19937 g(rd());

    uniform_real_distribution<float> dist(0.0, 100.0);

    bool droga_po_feromonach = false;
    int wierzcholek_po_feromonach;

    while (iteracja < liczba_iteracji) {

        wynik_DNA.clear();
        realna_dlugosc_DNA = dlugosc_okienka;
        zuzyte_wierzcholki_id.clear();
        nie_pasuje_bo_uzyty.clear();

        for (auto it = okienka[0].begin(); it != okienka[0].end(); it++) {
            wynik_DNA.push_back(*it);
        }
        zuzyte_wierzcholki_id.push_back(0);
        okienka = okienka_kopia;

        struct Mrowka mrowka;
        mrowka.sciezka.push_back(0);

        while (realna_dlugosc_DNA < dlugosc_DNA) {

            droga_po_feromonach = false;

            czy_mrowka_dotarla = true;
            cout << endl;

            okienka_temp = okienka[id_obecnego_wierzcholka];

            cout << "AKTUALNY WIERZCHOLEK (WAGI LICZYMY DLA NIEGO): " << wynik_DNA.size() << " " << endl;
            //     if (wynik_DNA.size() >= dlugosc_DNA) {
            //         nadmiar = wynik_DNA.size() - dlugosc_DNA;
            //        wynik_DNA.erase(wynik_DNA.end() - nadmiar, wynik_DNA.end());
            //     }


            for (auto &n: okienka_temp) {
                cout << n;
            }
            cout << endl;

            for (int i = 0; i < liczba_okienek; i++) {
                if (i == id_obecnego_wierzcholka)
                    waga_wierzcholkow.push_back(0);
                else {
                    //cout << "i: " << i << endl;
                    for (int j = 0; j < dlugosc_okienka; j++) {
                        if (equal(okienka[id_obecnego_wierzcholka].begin() + j,
                                  okienka[id_obecnego_wierzcholka].end(), okienka[i].begin(), okienka[i].end() - j) &&
                            count(zuzyte_wierzcholki_id.begin(), zuzyte_wierzcholki_id.end(), i) == 0) {
                            waga_wierzcholkow.push_back(j);
                            break;
                        } else if (j == dlugosc_okienka - 1 ||
                                   count(zuzyte_wierzcholki_id.begin(), zuzyte_wierzcholki_id.end(), i) != 0) {
                            waga_wierzcholkow.push_back(0);
                            break;
                        }
                    }
                }

                // wypisywanie wierzchokow, wag oraz ich id

                licznik_ite = 0;
                okienka_temp = okienka[i];


                for (auto ite = okienka_temp.begin(); ite != okienka_temp.end(); ite++) {
                    if (count(zuzyte_wierzcholki_id.begin(), zuzyte_wierzcholki_id.end(), i) == 0) {
                        if (licznik_ite != dlugosc_okienka - 1) {
                            ite--;
                            licznik_ite++;
                        } else {
                            cout << "wierzcholek: ";
                            for (int j = 0; j < dlugosc_okienka; j++) {
                                cout << *(ite + j);
                            }
                            cout << " waga wierzcholka: " << waga_wierzcholkow[i] << endl;
                            if (waga_wierzcholkow[i] != 0) {

                            }
                            licznik_ite = 0;
                            break;
                        }
                    }
                }

                okienka_temp = okienka[i];
            }

            id_wierzcholka = 0;

            // wybieranie kolejnego wierzcholka

            int min_waga = dlugosc_okienka; // niedopasowanie!
            int index2;
            nie_zero = false;

            for (auto &n: waga_wierzcholkow) {
                if (n != 0 && n < min_waga) {
                    min_waga = n;
                    nie_zero = true;
                }
            }

            shuffle(mrowki.begin(), mrowki.end(), g);

            Mrowka feromony_tej_mrowki;

            for (auto &n: mrowki) {
                if (dist(g) < (n.feromony * 10)) {

                    auto it_mrowka = find(n.sciezka.begin(), n.sciezka.end(), id_obecnego_wierzcholka);

                    if (it_mrowka != n.sciezka.end() && next(it_mrowka) != n.sciezka.end() ) {
                        wierzcholek_po_feromonach = *(next(it_mrowka));

                        if (find(zuzyte_wierzcholki_id.begin(), zuzyte_wierzcholki_id.end(), wierzcholek_po_feromonach) !=
                            zuzyte_wierzcholki_id.end()) {
                            continue;
                        }
                        else {
                            droga_po_feromonach = true;
                            feromony_tej_mrowki = n;
                            for (auto ite = okienka_temp.begin(); ite != okienka_temp.end(); ite++) {

                            }
                            cout << "FEROMONY ZADZIALALY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                            break;
                        }

                    }
                }
            }

            for (auto &el: waga_wierzcholkow) {

                if (el >= min_waga) {
                    wymieszane_wierzcholki = waga_wierzcholkow;
                    int id = 0;
                    for (auto it = wymieszane_wierzcholki.begin(); it != wymieszane_wierzcholki.end(); ++it) {

                        ulozone_wierzcholki.emplace_back(make_pair(*it, id));

                        id++;
                    }


                    // Wymieszaj wektor par
                    std::shuffle(ulozone_wierzcholki.begin(), ulozone_wierzcholki.end(), g);

                    for (auto &n: ulozone_wierzcholki) {
                        if (droga_po_feromonach) {
                            if (n.second == wierzcholek_po_feromonach) {
                                index2 = n.second;
                                ulozone_wierzcholki.clear();
                                id = 0;
                                for (auto &m : wymieszane_wierzcholki) {
                                    ulozone_wierzcholki.emplace_back(make_pair(m, id));
                                    id++;
                                }
                                break;
                            }
                        } else {
                            if (n.first >= el) {
                                index2 = n.second;
                                ulozone_wierzcholki.clear();
                                id = 0;
                                for (auto &m: wymieszane_wierzcholki) {
                                    ulozone_wierzcholki.emplace_back(make_pair(m, id));
                                    id++;
                                }
                                break;
                            }
                        }
                    }
                    if (find(zuzyte_wierzcholki_id.begin(), zuzyte_wierzcholki_id.end(), index2) !=
                        zuzyte_wierzcholki_id.end()) {
                        nie_pasuje_bo_uzyty.push_back(index2);
                        continue;
                    } else {
                        zuzyte_wierzcholki_id.push_back(index2);
                        for (auto &t: ulozone_wierzcholki) {
                            if (t.second == index2) {
                                int waga_minus = 0;
                                for (auto &y: okienka[index2]) {
                                    if (t.first > waga_minus) {
                                        waga_minus++;
                                    } else {
                                        for (auto it = okienka[index2].begin() - waga_minus + dlugosc_okienka;
                                             it != okienka[index2].end(); it++) {
                                            wynik_DNA.push_back(*it);
                                            realna_dlugosc_DNA = realna_dlugosc_DNA + 1;
                                            cout << "realna dlugosc DNA: " << realna_dlugosc_DNA << endl;
                                            cout << "Dodano do rozwiazania: " << *it << endl;
                                            temp_dlugosc = waga_minus;
                                            dodano = true;
                                        }
                                        if (waga_minus != 0) {
                                            suma_odleglosci = suma_odleglosci + waga_minus * waga_minus;
                                            suma_nadlozonej_drogi = suma_odleglosci + waga_minus - 1;
                                        }
                                        cout << "waga_minus: " << waga_minus << endl;
                                        cout << "suma odleglosci: " << suma_odleglosci << endl;
                                        break;
                                    }

                                }
                            }
                        }
                        id_obecnego_wierzcholka = index2;
                        cout << "id obecnego wierzcholka: " << id_obecnego_wierzcholka << endl;
                        okienka_temp = okienka[index2];
                        waga_wierzcholkow.clear();
                        break;
                    }
                }
                zuzyte_wierzcholki_id.push_back(index2);
            }

            if (!nie_zero) {
                czy_mrowka_dotarla = false;
                cout << "mrowka nie dotarla" << endl;
                realna_dlugosc_DNA = dlugosc_okienka - 1;
                suma_odleglosci = dlugosc_okienka - 1;
                suma_odleglosci = dlugosc_okienka - 1;
                break;
            }
            mrowka.sciezka.push_back(index2);
            mrowka.feromony = min_suma_odleglosci / suma_odleglosci;
            czy_mrowka_dotarla = true;
        }
        mrowki.push_back(mrowka);
        iteracja++;
        realna_dlugosc_DNA = dlugosc_okienka - 1;
        suma_odleglosci = dlugosc_okienka - 1;
        cout << "iteracja: " << iteracja << endl;
    }

    //  mrowka.sciezka.clear();
    //  mrowka.feromony = 0.0;


    //wypisanie wyniku
    cout << endl << endl << "Rozwiazanie: " << endl;
    for (int i = 0; i < dlugosc_DNA; i++)
    {
        cout << wynik_DNA[i] << " ";
    }

    //porownanie oryginalnej sekwencji oraz jej odtworzenia na podstawie okienek
    cout << endl << endl;
    int liczba_poprawnych_zasad = 0;
    for (int i = 0; i < dlugosc_DNA; i++) {
        if (lancuch_DNA[i] == wynik_DNA[i]) {
            liczba_poprawnych_zasad++;
        }
    }

    float poprawne_zasady_float = float (liczba_poprawnych_zasad);
    float dlugosc_DNA_float = float (dlugosc_DNA);
    float poprawnosc_rozwiazania = (poprawne_zasady_float/dlugosc_DNA_float) * 100;
    int dlugosc_trasy = 0;
//    cout << "Poprawnosc rozwiazania: " << poprawnosc_rozwiazania << endl;
    cout << "suma_odleglosci: " << suma_odleglosci << endl;
    //   cout << "suma nadlozonej drogi: " << suma_nadlozonej_drogi << endl;
    for (const auto& mrowka : mrowki) {
        std::cout << "Sciezka: ";
        for (int n : mrowka.sciezka) {
            std::cout << n << " ";
        }
        std::cout << "| Feromony: " << mrowka.feromony << std::endl;
    }

    return 0;
}
//END (main)