#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const double EPS = 1e-6;

typedef struct {
	double x, y;
} Point;

// struktura przechowujaca opis kartki
typedef struct {
	char type;		// typ kartki
	int k; 			// Z - nr kartki ktora zgieto
	Point p1;		// P - lewy dolny; Z : pierwszy punkt; K - srodek 
	Point p2;		// P - prawy gorny; Z: drugi punkt
	double r; 		// K - promien
} Card;

//iloczyn wektorowy do sprawdzenia po ktorej stronie prostej ab jest p
double cross_product(Point a, Point b, Point p) {
	return (b.x - a.x) * (p.y - a.y) + (a.y - b.y) * (p.x - a.x);
}

// funkcja ktora odbija punkt a wzgl. prostej pq
Point reflect_point(Point a, Point p, Point q) {
	Point res;

	double A = p.y - q.y;
	double B = q.x - p.x;
	double C = -A * p.x - B * p.y;

	double factor = -2 * (A * a.x + B * a.y + C) / (A * A + B * B);

	res.x = a.x + factor * A;
	res.y = a.y + factor * B;

	return res;
}


// glowna funkcja
int count_layers(int id, Point p, Card* cards) {
	Card current = cards[id];

	// 1. przypadek bazowy - prostokat
	if(current.type == 'P') {
		// czy p lezy wewnatrz prostokata
		if( p.x >= current.p1.x - EPS && p.x <= current.p2.x + EPS &&
			p.y >= current.p1.y - EPS && p.y <= current.p2.y + EPS) {
			return 1;
		}
		return 0;
	}

	// 2. przypadek bazowy - kolo
	else if(current.type == 'K') {
		double dx = p.x - current.p1.x; 
		double dy = p.y - current.p1.y;

		// czy p lezy wewnatrz kola
		if(dx * dx + dy * dy <= current.r * current.r + EPS) {
			return 1;
		}
		return 0;
	}

	// 3. przypadek - zgiecie
	else {
		double cp = cross_product(current.p1, current.p2, p);

		// jesli cp < 0 to p jest w obszarze ktory zostal zgiety
		if(cp < -EPS) {
			return 0;
		}
		// jesli cp > 0  to p jest w obszarze do ktorego doszla nowa warstwa
		else if(cp > EPS) {
			Point reflected = reflect_point(p, current.p1, current.p2);
			return count_layers(current.k, p, cards) + count_layers(current.k, reflected, cards);
		}
		// jesli cp = 0 to p lezy na zgieciu
		else {
			return count_layers(current.k, p, cards);
		}
	}

	return 0;
}

int main() {
	int n, q;
	scanf("%d %d", &n, &q);

	Card* cards = (Card*) malloc((unsigned)(n + 1) * sizeof(Card));

	for(int i = 1; i <= n; i++) {
		char ttype;
		scanf(" %c", &ttype);
		cards[i].type = ttype;

		if(cards[i].type == 'P') {
			scanf("%lf %lf %lf %lf", &cards[i].p1.x, &cards[i].p1.y, &cards[i].p2.x, &cards[i].p2.y);
		}
		else if(cards[i].type == 'K') {
			scanf("%lf %lf %lf", &cards[i].p1.x, &cards[i].p1.y, &cards[i].r);
		}
		else {
			scanf("%d %lf %lf %lf %lf", &cards[i].k, &cards[i].p1.x, &cards[i].p1.y, &cards[i].p2.x, &cards[i].p2.y);
		}
	}

	for(int j = 0; j < q; j++) {
		int k; Point p;
		scanf("%d %lf %lf", &k, &p.x, &p.y);

		int res = count_layers(k, p, cards);
		printf("%d\n", res);
	}
	free(cards);

	return 0;
}