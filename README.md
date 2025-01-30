# Računalna animacija - Laboratorijske vježbe
Ovaj repozitorij sadrži sve 3 laboratorijske vježbe iz predmeta računalna animacija.
Za kompajliranje potrebno je imati alat cmake.

**Pokretanje**:
1. Generiranje build build sustava: `cmake -B build .`
2. Ako se koristi `make` onda je za kompajliranje potrebno napraviti `cd build && make`
3. Pokretanje iz `build` direktorija: `./lab1/lab1`, `./lab2/lab2`, `./lab3/lab3`
 >Napomena: Na MacOS operacijskom sustavu, potrebno je upisati apsolutnu putanju do izvršne datoteke.

**Opis laboratorijskih vježbi**:
1. Animacija kretanja objekata po B-krivulji.
2. Simulacija tkanine i fizike sudara s kuglama. Kugle mogu interagirati s tkaninom i podom. Koristi se AABB optimizacija prije detaljnog izračuna kolizije tkanine i kugle.
3. Skeletalna animacija i implementacija animiranih sjena.
