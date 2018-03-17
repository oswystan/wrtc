LIBS := thirdparty/libwebrtc.a -lpthread
CFLAGS := -std=c++11 -DWEBRTC_POSIX -fno-rtti -D_GLIBCXX_USE_CXX11_ABI=0
INCS := -I thirdparty/include

all: a.out opusdec dec

a.out: main.o
	g++ $^ -o $@ $(LIBS)
opusdec: opusdec.o
	g++ $^ -o $@ $(LIBS)
dec: dec.o
	g++ $^ -o $@ $(LIBS)

%.o:%.cpp
	g++ -c $(CFLAGS) $(INCS) -o $@ $<

clean:
	rm -f *.o a.out opusdec dec
