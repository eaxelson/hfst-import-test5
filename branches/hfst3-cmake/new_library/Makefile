
FPIC = -fPIC

CXX = g++
WARNING = -Wall -Wcast-qual -Wconversion -std=c++98 -Wno-deprecated -ansi

CC = g++
CL = $(CC)
CFLAGS = -O3 $(WARNING) $(SGILIB) $(CDT) $(READLINE) $(FPIC)
LDFLAGS =

CXXFLAGS = $(CFLAGS)
LXXFLAGS = $(LDFLAGS)

INSTALL = install
INSTALL_DIR = install -d -m 755
INSTALL_LIBS = $(INSTALL) -s

DESTDIR =
PREFIX = /usr/local/

OBJ = MyFst.o

libmfstl.so: MyFst.o
	$(CL) -shared -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o libmfstl.so

libinstall: dynlib hfiles

dynlib:	libmfstl.so
	$(INSTALL_LIBS) $^ $(PREFIX)lib
	(ldconfig || true)

hfiles: MyFst.h
	-@if [ ! -d $(PREFIX)include/mfstl ]; then mkdir -p $(PREFIX)include/mfstl; fi
	$(INSTALL) $^ $(PREFIX)include/mfstl

