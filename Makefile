cc = gcc
bin = livesman
CFLAGS = -O2 -std=c89 -pedantic -Wall -DANSI -Werror
ar = ar
.PHONY: all clean require

all: $(bin)
$(bin): main.o hmg_engine.a grap/grap.o function.o
	$(cc) -o $(bin) main.o hmg_engine.a function.o grap/grap.o
main.o: main.c
	$(cc) -c $(CFLAGS) main.c
hmg_engine.o: hmg_engine.c
	$(cc) -c $(CFLAGS) hmg_engine.c
hmg_engine.a: hmg_engine.o linklist/linklistc.o linklist/linklistx.o
	$(ar) rc hmg_engine.a hmg_engine.o linklist/linklistc.o linklist/linklistx.o
function.o:	function.c
	$(cc) -c $(CFLAGS) function.c
require:
	git clone https://github.com/hwoy/linklist.git && \
git clone https://github.com/hwoy/grap.git && \
make -C spec && make -C linklist && make -C grap

clean:
	rm -f $(bin) *.o *~ *.a
