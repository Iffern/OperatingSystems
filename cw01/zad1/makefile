CC=gcc -Wall

all: static shared main clean

static:
	make clean
	$(CC) -c difflib.c
	ar rcs libdifflib.a difflib.o
	$(CC) main.c -o main -L. -ldifflib

shared:
	$(CC) -fPIC -c difflib.c
	$(CC) -fPIC -shared -o difflib.so difflib.o

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main
test:
	@echo ---Mala liczba prob---
	./main 5 compare_pairs a.txt:b.txt b.txt:c.txt d.txt:e.txt e.txt:f.txt f.txt:g.txt
	@echo ---Srednia liczba prob---
	./main 25 compare_pairs a.txt:b.txt b.txt:c.txt d.txt:e.txt e.txt:f.txt f.txt:g.txt g.txt:h.txt h.txt:i.txt j.txt:k.txt k.txt:l.txt l.txt:m.txt m.txt:a.txt l.txt:b.txt remove_block 0 remove_block 1 remove_block 2 remove_block 7 remove_block 9 compare_pairs a.txt:b.txt b.txt:c.txt d.txt:e.txt e.txt:f.txt f.txt:g.txt g.txt:h.txt h.txt:i.txt j.txt:k.txt k.txt:l.txt l.txt:m.txt m.txt:a.txt l.txt:b.txt remove_block 4 remove_block 3 remove_block 10 remove_block 14 remove_block 15
	@echo ---Duza liczba prob---
	./main 50 compare_pairs a.txt:b.txt b.txt:c.txt d.txt:e.txt e.txt:f.txt f.txt:g.txt g.txt:h.txt h.txt:i.txt j.txt:k.txt k.txt:l.txt l.txt:m.txt m.txt:a.txt l.txt:b.txt k.txt:a.txt i.txt:j.txt e.txt:f.txt f.txt:g.txt g.txt:h.txt h.txt:i.txt j.txt:k.txt a.txt:b.txt b.txt:c.txt d.txt:e.txt e.txt:f.txt f.txt:g.txt e.txt:m.txt a.txt:b.txt b.txt:c.txt d.txt:e.txt e.txt:f.txt f.txt:g.txt g.txt:h.txt h.txt:i.txt j.txt:k.txt k.txt:l.txt l.txt:m.txt m.txt:a.txt l.txt:b.txt k.txt:a.txt i.txt:j.txt e.txt:f.txt f.txt:g.txt g.txt:h.txt h.txt:i.txt j.txt:k.txt a.txt:b.txt b.txt:c.txt d.txt:e.txt e.txt:f.txt f.txt:g.txt e.txt:m.txt
	@echo --Pliki bardzo podobne---
	./main 20 compare_pairs p.txt:pp.txt p.txt:ppp.txt p.txt:pppp.txt pp.txt:ppp.txt ppp.txt:pppp.txt p.txt:pp.txt p.txt:ppp.txt p.txt:pppp.txt pp.txt:ppp.txt ppp.txt:pppp.txt p.txt:pp.txt p.txt:ppp.txt p.txt:pppp.txt pp.txt:ppp.txt ppp.txt:pppp.txt p.txt:pp.txt p.txt:ppp.txt p.txt:pppp.txt pp.txt:ppp.txt ppp.txt:pppp.txt
	@echo --Pliki srednio podobne---
	./main 20 compare_pairs e.txt:f.txt g.txt:h.txt h.txt:i.txt i.txt:j.txt k.txt:l.txt e.txt:f.txt g.txt:h.txt h.txt:i.txt i.txt:j.txt k.txt:l.txt e.txt:f.txt g.txt:h.txt h.txt:i.txt i.txt:j.txt k.txt:l.txt e.txt:f.txt g.txt:h.txt h.txt:i.txt i.txt:j.txt k.txt:l.txt
	@echo --Pliki malo podobne---
	./main 20 compare_pairs a.txt:m.txt b.txt:m.txt c.txt:m.txt d.txt:m.txt h.txt:m.txt a.txt:m.txt b.txt:m.txt c.txt:m.txt d.txt:m.txt h.txt:m.txt a.txt:m.txt b.txt:m.txt c.txt:m.txt d.txt:m.txt h.txt:m.txt a.txt:m.txt b.txt:m.txt c.txt:m.txt d.txt:m.txt h.txt:m.txt
	@echo --Tworzenie tablic o roznych wielkosciach--
	./main 0 create_table 5 create_table 20 create_table 100 create_table 1000 create_table 1000000
