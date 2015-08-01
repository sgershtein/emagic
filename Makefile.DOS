CC = bcc
FLAGS =

all:  emagic.exe

clean:
	del emagic.exe *.obj tables\tables.h _mktbl.exe

frequen.obj : frequen.cpp frequen.h recode.h
	$(CC) -c $(FLAGS) frequen.cpp

recode.obj : recode.cpp recode.h tables\tables.h
	$(CC) -c $(FLAGS) recode.cpp

emagic.obj : emagic.cpp emagic.h recode.h frequen.h
	$(CC) -DDOS -c $(FLAGS) emagic.cpp

emagic.exe : frequen.obj emagic.obj recode.obj
	$(CC) $(FLAGS) emagic.obj frequen.obj recode.obj

tables\tables.h : tables\*.tab _mktbl.exe
	cd tables
        ..\_mktbl.exe ai.tab ak.tab aw.tab ik.tab iw.tab kw.tab
        cd ..
        
_mktbl.exe : _mktbl.cpp
	$(CC) $(FLAGS) _mktbl.cpp
