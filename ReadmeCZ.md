# Finalni verze zapoctoveho programu Tunneler

1. Uvod
2. Koncept
3. Vstup a vystup
4. Kompilace a spusteni

---

1. Uvod
Tunneler je hra pro 2-8 hracu. Kazdy hrac ovlada jeden tank, kterym muze kopat tunely, 
strilet a pohybovat se po herni mape.Cilem hry je najit a zlikvidovat vsechny protihrace.

2. Koncept
Program je rozdelen na serverovou cast a klientskou cast. Komunikace probiha pres TCP sockety.
Ukolem serveru je navazat spojeni s predem stanovenym poctem klientu (max. 8) a odstartovat hru.
Behem hry funguje server jako centrum pro distribuci zmen generovanych jednotlivymi klienty.

Kazdy z klientu ovlada jeden tank (v pripade splitscreen modu dva) a pri kazdem impulsu casovace odesila svuj aktualni stav 
na server a prijima odpoved ve ktere jsou i aktualni stavy ostatnich hracu. 
Na zaklade teto odpovedi meni stav hry a zobrazuje vysledek.

3. Vstup a vystup
Program vyuziva knihovnu SDL pro zobrazovani grafiky, timer a zpracovani stisku klaves.
Ovladani tanku:
normalni mod: sipky - pohyb, Left CTRL - strelba
splitscreen mod: 1. hrac - stejne jako normalni mod, 2. hrac - klavesy ASDW - pohyb, Right CTRL - strelba

4. Kompilace a spusteni
Pro kompilaci a spusteni klientske casti programu je nutna knihovna SDL volne dostupna na www.libsdl.org
Kompilace by mela byt mozna v rozumnem linuxovem prostredi s pouzitim prilozeneho Makefile.

Hra je spustitelna v normalnim modu nebo ve splitscreen modu, ktery umoznuje hru dvou hracu.
Je prilozena testovaci mapa (soubor ``new_map``)

nejprve je potreba spustit server napr.:

```
./server -m new_map -l 1 -c 3
```

(viz. server --help) 

pote lze spustit klienta; klient pozaduje tyto parametry:
- ip adresu serveru
- nazev souboru s mapou (predpoklada se, ze mapu maji vsichni klienti stejnou)

```
./tunneler -a ip_address -m map_file
```

(viz. tunneler --help)

Priklad spusteni:
V jednom terminalu spustte:
```
#./server -m new_map
```

V jinem terminalu spustte:
```
#./tunneler -a 127.0.0.1 -m new_map -s
```
