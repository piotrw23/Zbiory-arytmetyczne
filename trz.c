#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

typedef struct {
	int siec;
	int odl;
} para;

int min(int a, int b) {
	return a < b ? a : b;
}

int max(int a, int b) {
	return a > b ? a : b;
}

// funkcja sprawdza czy motele na pozycjach l, i, r sa roznych sieci
bool sprawdz(int l, int i, int r, para *motel) {
	if(l == 0 || r == 0) return false; // wartosc 0 to ,,sztuczna" siec
	return(motel[i].siec != motel[l].siec &&
		motel[l].siec != motel[r].siec &&
		motel[r].siec != motel[i].siec);
}

//funkcja sprawdza czy tablicy nowe jest motel majacy siec ,,siec"
bool czy_juz_jest(int siec, int *nowe, int rozmiar, para *motel) {
    for(int k = 0; k < rozmiar; k++) if(motel[nowe[k]].siec == siec) return true;
    return false;
}

//funkcja znajduje trzy pierwsze motele roznych sieci
bool policz_lewo(int *lewo, para *motel, int n) {
	int licz = 0, i = 1;
	while(licz < 3 && i <= n) {
		if(motel[i].siec != motel[lewo[0]].siec && 
			motel[i].siec != motel[lewo[1]].siec) {
			lewo[licz] = i;
			licz++;
		}
		i++;
	}
	return (licz < 3); // jesli true to znaczy ze nie istnieja motele 3 roznych sieci
}

//funkcja znajduje trzy ostatnie motele roznych sieci
bool policz_prawo(int *prawo, para *motel, int n) {
	int licz = 0, i = n;
	while(licz < 3 && i > 0) {
		if(motel[i].siec != motel[prawo[0]].siec && 
			motel[i].siec != motel[prawo[1]].siec) {
			prawo[licz] = i;
			licz++;
		}
		i--;
	}
	return (licz < 3); // jesli true to znaczy ze nie istnieja motele 3 roznych sieci
}

int najdalsza_trojka(para *motel, int n) {
	if(n < 3)
		return 0; // jesli mniej niz 3 motele to sprzecznosc

	int wyn = 0;
	int lewo[3] = {0, 0, 0}; // zbior 3 moteli roznych sieci najbardziej na lewo
	int prawo[3] = {0, 0, 0}; // zbior 3 moteli roznych sieci najbardziej na prawo

	if(policz_lewo(lewo, motel, n)) // wyznaczamy lewo
		return 0;
	if(policz_prawo(prawo, motel, n)) // wyznaczamy prawo
		return 0;

	//iterujemy po kandydatach na srodkowy motel
	//dla rozwazanego motelu i iterujemy po kandydatach na motel lewy i prawy
	for(int i = 1; i <= n; i++) {
		for(int j = 0; j < 3; j++) {
			for(int k = 0; k < 3; k++) {


				int l = lewo[j], r = prawo[k]; // niezmiennik: l < i < r
				if(i < l || i > r) continue; // gdy niezmiennik nie jest zachowany

				// sprawdzamy czy motel lewy, srodkowy i prawy sa roznych sieci 
				if(sprawdz(l, i, r, motel)) {
					wyn = max(min(motel[i].odl - motel[l].odl, motel[r].odl - motel[i].odl), wyn);
				}
			}
		}
	}
	return wyn;
}

//funkcja oblicza tablice pref
//pref[][i] = zbior 3 moteli roznych sieci bedacych na prefiksie [1...i-1] najblizej pozycji i 
void policz_pref(int **pref, para *motel, int n) {
    int najblizsi[3] = {0, 0, 0}; // zbior ostatnio spotkanych 3 moteli roznych sieci
    for (int i = 1; i <= n; i++) {
    	// z definicji pref[][i] = najblizsi
        pref[0][i] = najblizsi[0];
        pref[1][i] = najblizsi[1];
        pref[2][i] = najblizsi[2];

        // aktualizujemy tablice najblizsi
        // mamy 4 kandydatow do tablicy najblizsi
        int kandydaci[4] = {i, najblizsi[0], najblizsi[1], najblizsi[2]};
        //tablica pomocnicza do wyznaczenia wlasciwej trojki moteli
        int nowe[3] = {0, 0, 0}, licz = 0;
        
        for(int k=0; k<4; k++) {
            int kand = kandydaci[k];
            if(kand == 0) continue;
            //jezeli motel[kand] ma inna siec niz motele w tablicy nowe
            //oraz jesli w tablicy nowe sa mniej niz 3 motele 
            //to dodajemy kandydata
            if(!czy_juz_jest(motel[kand].siec, nowe, licz, motel) && licz < 3) {
                nowe[licz++] = kand;
            }
        }
        //przepisujemy wynik z tablicy pomocniczej
        for(int k = 0; k < 3; k++) najblizsi[k] = nowe[k];
    }
}

//funkcja oblicza tablice suf
//suf[][i] = zbior 3 moteli roznych sieci bedacych na sufiksie [n...n-i+1] najblizej pozycji i 
void policz_suf(int **suf, para *motel, int n) {
    int najblizsi[3] = {0, 0, 0}; // zbior ostatnio spotkanych 3 moteli roznych sieci
    for (int i = n; i >= 1; i--) {
    	//z definicji suf = najblizsi
        suf[0][i] = najblizsi[0];
        suf[1][i] = najblizsi[1];
        suf[2][i] = najblizsi[2];

        //aktualizujemy tablice najblizsi
        //mamy 4 kandydatow do tablicy najblizsi
        int kandydaci[4] = {i, najblizsi[0], najblizsi[1], najblizsi[2]};
        //tablica pomocnicza do wyznaczenia wlasciwej trojki moteli
        int nowe[3] = {0, 0, 0}, licz = 0;

        for(int k = 0; k < 4; k++) {
            int kand = kandydaci[k];
            if(kand == 0) continue; 
            //jezeli motel[kand] ma inna siec niz motele w tablicy nowe
            //oraz jesli w tablicy nowe sa mniej niz 3 motele 
            //to dodajemy kandydata
            if(!czy_juz_jest(motel[kand].siec, nowe, licz, motel) && licz < 3) {
                nowe[licz++] = kand;
            }
        }
        //przepisujemy wynik z tablicy pomocniczej
        for(int k = 0; k < 3; k++) najblizsi[k] = nowe[k];
    }
}

int najblizsza_trojka(para *motel, int n) {
	if(n < 3)
		return 0; 

	int wyn = INT_MAX;
	int *pref[3]; // min_pref[i] = zbior 3 moteli roznych sieci na prefiksie [1...i]
	int *suf[3]; // min_pref[i] = zbior 3 moteli roznych sieci na sufiksie [n...i-n+1]
	for(int i = 0; i < 3; i++) {
		pref[i] = malloc((size_t)(n + 1) * sizeof(int));
		suf[i] = malloc((size_t)(n + 1) * sizeof(int));
	}

	policz_pref(pref, motel, n);
	policz_suf(suf, motel, n);

	//iterujemy po kandydatach na srodkowy motel
	//dla rozwazanego motelu i iterujemy po kandydatach na motel lewy i prawy
	for(int i = 2; i <= n-1; i++) {
		for(int j = 0; j < 3; j++) {
			for(int k = 0; k < 3; k++) {
				int l = pref[j][i], r = suf[k][i];

				//jesli motele i, l, r sa roznych sieci to aktualiujemy wynik
				if(sprawdz(l, i, r, motel)) {
					wyn = min(max(motel[i].odl - motel[l].odl, motel[r].odl - motel[i].odl), wyn);
				}
			}
		}
	}
	for(int i = 0; i < 3; i++) {
		free(pref[i]);
		free(suf[i]);
	}
	if(wyn == INT_MAX) return 0;
	return wyn;
}

int main() {
	int n;
	scanf("%d", &n);
	para* motel = malloc((size_t) (n + 1) * sizeof(para));
	motel[0] = (para) {0, 0};
	for(int i = 1; i <= n; i++) {
		scanf("%d %d", &motel[i].siec, &motel[i].odl);
	}

	printf("%d %d", najblizsza_trojka(motel, n), najdalsza_trojka(motel, n));
	free(motel);
	return 0;
}