cc = gcc
bin = livesman
CFLAGS = -O2 -std=c89 -pedantic -Wall -DANSI -Werror
ar = ar
.PHONY: all clean distclean require deps

all: $(bin)
$(bin): main.o hmg_engine.o function.o grap/grap.o mem/mem.o liteList/linklist.o
	$(cc) -o $(bin) main.o hmg_engine.o function.o grap/grap.o mem/mem.o liteList/linklist.o


main.o: main.c
	$(cc) -c $(CFLAGS) main.c
hmg_engine.o: hmg_engine.c
	$(cc) -c $(CFLAGS) hmg_engine.c
function.o:	function.c
	$(cc) -c $(CFLAGS) function.c


grap/grap.o: grap/grap.c
	make -C grap
mem/mem.o: mem/mem.c
	make -C mem 
linkList/linklist.o: linkList/linklist.c
	make -C linkList 


require:
	git clone https://github.com/hwoy/linklist.git && \
git clone https://github.com/hwoy/grap.git && \
&& make deps
deps:
	make -C liteList && make -C grap && make -C mem



clean:
	rm -f $(bin) *.o *~ *.a
distclean:
	make clean && make -C liteList clean && make -C grap clean && make -C mem clean

