#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "zbior_ary.h"

int q;

zbior_ary ciag_arytmetyczny(int a, int Q, int b) {
	q = Q;
	zbior_ary X = {NULL, 0};
	X.prz = (przedzial*) malloc(sizeof(przedzial));
	X.rozmiar = 1;
	X.prz[0].a = a;
	X.prz[0].b = b;
	int r = ((a % Q) + Q) % Q;
	X.prz[0].r = r;
	return X; 
}

zbior_ary singleton(int a) {
	return ciag_arytmetyczny(a, q, a);
}

unsigned moc(zbior_ary A) {
    long long wyn = 0;
    for(int i = 0; i < (int)A.rozmiar; i++) {
        long long b = (long long)A.prz[i].b;
        long long a = (long long)A.prz[i].a;
            
        wyn += (b - a) / q + 1;
    }
    return (unsigned)wyn;
}

unsigned ary(zbior_ary A) {
	//z definicji struktury 
	return A.rozmiar;
}

// znajduje indeks pierwszego przedziału p takiego, że:
// p.r > res LUB (p.r == res ORAZ p.a >= a_szukane)
int binsearch(zbior_ary A, int res, int b) {
    int poc = 0;
    int kon = (int)A.rozmiar;
    
    while(poc < kon) {
        int sr = poc + (kon - poc) / 2;
        if(A.prz[sr].r < res || (A.prz[sr].r == res && A.prz[sr].a < b)) {
            poc = sr + 1;
        } else {
            kon = sr;
        }
    }
    return poc;
}

bool nalezy(zbior_ary A, int b) {
	// jesli A jest pusty to nie zawiera b
    if(A.rozmiar == 0) return false;

    int res = ((b % q) + q) % q;
    
    int idx = binsearch(A, res, b + 1);
    
    // idx to pierwszy element, że A.prz[i].r > res LUB
    // A.prz[idx].a >= b+1 ORAZ reszty sie zgadzają
    // nas może interesować element tuż przed nim
    if(idx > 0) {
        idx--;
    }

    // teraz idx to najlepszy kandydat na przedział zawierający b więc go sprawdzamy
    // czy ma poprawną resztę
    // czy b jest za początkiem (lub jest początkiem)
    // czy b jest przed końcem (lub jest końcem)
    return (A.prz[idx].r == res && A.prz[idx].a <= b && b <= A.prz[idx].b);
}

void dodaj_przedzial(zbior_ary *C, int a, int b, int r) {
    if(C->rozmiar == 0) {
    	C->prz = (przedzial*) malloc(sizeof(przedzial));
    } else {
    	unsigned n = C->rozmiar;
    	przedzial* pom_tab = (przedzial*) realloc(C->prz, (n + 1) * sizeof(przedzial));
    	C->prz = pom_tab;
    }

    // mamy gwarantowane miejsce wiec dodajemy element
    C->prz[C->rozmiar].a = a;
    C->prz[C->rozmiar].b = b;
    C->prz[C->rozmiar].r = r;
    C->rozmiar++;
}

zbior_ary suma(zbior_ary A, zbior_ary B) {

	// deklarujemy wynik C jako zbior pusty
	zbior_ary C = {NULL, 0};

	unsigned i = 0, j = 0; //wskaźniki na A, B

	while(i < A.rozmiar || j < B.rozmiar) {
		przedzial aktu; // aktualny przedzial

		// sprawdzamy czy A.prz[i].r == B.prz[j].r
		// jeżeli nie to bierzemy przedział o mniejszym r
		// jeżeli tak to bierzemy przedział o mniejszym początku
		if(j >= B.rozmiar || (i < A.rozmiar &&
		 (A.prz[i].r < B.prz[j].r ||
		 (A.prz[i].r == B.prz[j].r && A.prz[i].a <= B.prz[j].a)))) {
			aktu = A.prz[i++];
		} else {
			aktu = B.prz[j++];
		}

		// jeśli wynik jest pusty to dodajemy aktualnie rozważany przedział
		if(C.rozmiar == 0) {
			dodaj_przedzial(&C, aktu.a, aktu.b, aktu.r);
			continue;
		}

		//wynik nie jest pusty -> istnieje ostatni przedział w C 
		przedzial *ostatni = &C.prz[C.rozmiar - 1];

        // sprawdzamy czy możemy scalic aktu i ostatni przedział w C
        if(ostatni->r == aktu.r && (long long)aktu.a <= (long long)ostatni->b + (long long)q) {
            // scalamy: wystarczy ewentualnie rozszerzyć koniec
            if(aktu.b > ostatni->b) {
                ostatni->b = aktu.b;
            }
        } else {
            // brak scalenia (inna reszta lub przerwa)
            // dodajemy aktu jako osobny przedział
            dodaj_przedzial(&C, aktu.a, aktu.b, aktu.r);
        }
    }
    return C;
}

zbior_ary roznica(zbior_ary A, zbior_ary B) {
	zbior_ary C = {NULL, 0};

	unsigned i = 0, j = 0; // wskaźniki na A, B

	while(i < A.rozmiar) {
		przedzial pA = A.prz[i];

		long long biezacy_poc = A.prz[i].a;
		long long biezacy_kon = A.prz[i].b;

		// przesuwamy wskaźnik j pomijając przedziały
		// które mają tą samą resztę i kończą się przed początkiem pA
		// oraz pomijamy przedziały które mają mniejsze r niż pA.r
		while(j < B.rozmiar &&
			(B.prz[j].r < pA.r ||
				(B.prz[j].r == pA.r && B.prz[j].b < biezacy_poc))) {
			j++;
		}

		while(j < B.rozmiar &&
			B.prz[j].r == pA.r &&
			B.prz[j].a <= biezacy_kon) {

			przedzial pB = B.prz[j];
			// przedziały pA, pB przecinają się dzieki poprzedniej while'owi
			// sprawdzamy czy do wyniku można dodać przedział na lewo od pB
			if(pB.a > (int)biezacy_poc) {
				dodaj_przedzial(&C, (int)biezacy_poc, pB.a-q, pB.r); 
			}

			biezacy_poc = (long long)pB.b + (long long)q;

			// jeśli cały pA został pokryty przez pB zwiększamy i bez zmiany j
			if(biezacy_poc > biezacy_kon) {
				break;
			}
			j++;
		}

		// dodajemy ostatni niwycięty fragment pA (jeśli istnieje)
		if(biezacy_poc <= biezacy_kon) {
			dodaj_przedzial(&C, (int)biezacy_poc, (int)biezacy_kon, pA.r);
		}
		i++;
	}
	return C;
}

zbior_ary iloczyn(zbior_ary A, zbior_ary B) {
	//iloczyn(A,B) = A\(A\B)
	zbior_ary tmp = roznica(A, B);
	zbior_ary C = roznica(A, tmp);
	free(tmp.prz);
	return C;
}
