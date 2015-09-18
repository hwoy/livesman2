cc = gcc
bin = livesman
CFLAGS = -O2 -std=c89 -pedantic -Wall -DANSI -Werror
ar = ar
.PHONY: all clean distclean require require_private deps cleandeps

all: $(bin)
$(bin): main.o hmg_engine.o function.o grap/grap.o mem/mem.o liteList/linklist.o opt/opt.o
	$(cc) -o $(bin) main.o hmg_engine.o function.o grap/grap.o mem/mem.o liteList/linklist.o opt/opt.o


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
liteList/linklist.o: liteList/linklist.c
	make -C liteList 
opt/opt.o: opt/opt.c
	make -C opt 


require:
	git clone https://github.com/hwoy/liteList.git && \
git clone https://github.com/hwoy/grap.git && \
git clone https://github.com/hwoy/mem.git && \
git clone https://github.com/hwoy/opt.git

require_private:
	git clone ssh://git@server/share/data/src/liteList.git && \
git clone ssh://git@server/share/data/src/grap.git && \
git clone ssh://git@server/share/data/src/mem.git && \
git clone ssh://git@server/share/data/src/opt.git 

deps:
	make -C liteList && make -C grap && make -C mem && make -C opt

cleandeps:
	rm -rf liteList grap mem opt

clean:
	rm -f $(bin) *.o *~ *.a
distclean:
	make clean && make -C liteList clean && make -C grap clean && make -C mem clean && make -C opt clean

