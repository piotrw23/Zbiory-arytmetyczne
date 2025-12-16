#include "worki.h"
#include <vector>
using namespace std;

struct Lokacja {
    int id; // -1 = biurko, >=0 = id worka
    int suma_przedmiotow = 0; // Łączna liczba przedmiotów w tej lokacji
    przedmiot* head = nullptr; // Głowa listy zawartości
};

vector<przedmiot*> przedmioty; 
vector<worek*> worki;
vector<Lokacja*> lokacje;

int licznik_id = 0;
Lokacja* biurko = nullptr;

// tworzy biurko
void init_biurko() {
    if(!biurko) {
        biurko = new Lokacja;
        biurko->id = -1;
        biurko->suma_przedmiotow = 0;
        biurko->head = nullptr;
        lokacje.push_back(biurko);
    }
}

// dodaje element do listy w danej lokacji
void link_do_lokacji(przedmiot *p, Lokacja *cel) {
    p->lokacja = cel;
    p->prev = nullptr;
    p->next = cel->head;
    
    if(cel->head) {
        cel->head->prev = p;
    }
    cel->head = p;
}

// usuwa element z jego obecnej lokacji
void unlink_z_lokacji(przedmiot *p) {
    
    if(p->prev) {
        p->prev->next = p->next;
    } else { 
        p->lokacja->head = p->next;
    }
    
    if(p->next) {
        p->next->prev = p->prev;
    }
    
    p->prev = nullptr;
    p->next = nullptr;
}

przedmiot *nowy_przedmiot() {
    init_biurko();
    przedmiot *p = new przedmiot;
    przedmioty.push_back(p);
    
    link_do_lokacji(p, biurko);
    
    biurko->suma_przedmiotow++;
    
    return p;
}

worek *nowy_worek() {
    init_biurko();
    worek *w = new worek;
    w->id = licznik_id++;
    worki.push_back(w);
    
    // tworzymy wnetrze worka
    Lokacja *l = new Lokacja;
    l->id = w->id;
    l->suma_przedmiotow = 0;
    l->head = nullptr;
    w->wnetrze = l;
    lokacje.push_back(l);
    
    link_do_lokacji(w, biurko);
    return w;
}

void wloz(przedmiot *co, worek *gdzie) {
    unlink_z_lokacji(co);
    link_do_lokacji(co, gdzie->wnetrze);
    
    // worek 'gdzie' zyskuje przedmiot
    gdzie->wnetrze->suma_przedmiotow++;
}

void wloz(worek *co, worek *gdzie) {
    int wklad = co->wnetrze->suma_przedmiotow;
    
    unlink_z_lokacji(co);
    link_do_lokacji(co, gdzie->wnetrze);
    
    // worek 'gdzie' zyskuje zawartosc worka 'co'
    gdzie->wnetrze->suma_przedmiotow += wklad;
}

void wyjmij(przedmiot *p) {
    // worek traci przedmiot
    p->lokacja->suma_przedmiotow--;
    
    unlink_z_lokacji(p);
    link_do_lokacji(p, biurko);
}

void wyjmij(worek *w) {
    int wklad = w->wnetrze->suma_przedmiotow;
    
    // lokacja traci zawartosc wyjmowanego worka
    w->lokacja->suma_przedmiotow -= wklad;
    
    unlink_z_lokacji(w);
    link_do_lokacji(w, biurko);
}

int w_ktorym_worku(przedmiot *p) {
    if(!p->lokacja) return -1;
    return p->lokacja->id;
}

int w_ktorym_worku(worek *w) {
    if(!w->lokacja) return -1;
    return w->lokacja->id;
}

int ile_przedmiotow(worek *w) {
    return w->wnetrze->suma_przedmiotow;
}

void na_odwrot(worek *w) {
    // zdejmujemy worek w z biurka
    biurko->suma_przedmiotow -= w->wnetrze->suma_przedmiotow;
    unlink_z_lokacji(w);
    
    // kopiujemy wskazniki na biurko i wnetrze worka
    Lokacja *dawne_biurko = biurko; 
    Lokacja *dawny_worek = w->wnetrze;
    
    // swapujemy id
    dawne_biurko->id = w->id;
    dawny_worek->id = -1;
    
    // przepinamy wskazniki
    w->wnetrze = dawne_biurko;
    biurko = dawny_worek;
    
    // wkladamy worek w na nowe biurko
    link_do_lokacji(w, biurko);
    
    // nowe biurko przyjmuje worek w
    biurko->suma_przedmiotow += w->wnetrze->suma_przedmiotow;
}

void gotowe() {
    for(auto p : przedmioty) delete p;
    przedmioty.clear();
    
    for(auto w : worki) delete w;
    worki.clear();
    
    for(auto l : lokacje) delete l;
    lokacje.clear();
    
    biurko = nullptr;
    licznik_id = 0;
}