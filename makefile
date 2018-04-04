LIBS := thirdparty/libwebrtc.a -lpthread
CFLAGS := -std=c++11 -DWEBRTC_POSIX -fno-rtti -D_GLIBCXX_USE_CXX11_ABI=0
INCS := -I thirdparty/include
bin := a.out opusdec opusenc dec rs isacdec isacenc
all: $(bin)

a.out: main.o
	g++ $^ -o $@ $(LIBS)
opusdec: opusdec.o
	g++ $^ -o $@ $(LIBS)
opusenc: opusenc.o
	g++ $^ -o $@ $(LIBS)
isacdec: isacdec.o
	g++ $^ -o $@ $(LIBS)
isacenc: isacenc.o
	g++ $^ -o $@ $(LIBS)
dec: dec.o
	g++ $^ -o $@ $(LIBS)
rs: resample.o
	g++ $^ -o $@ $(LIBS)

%.o:%.cpp
	g++ -c $(CFLAGS) $(INCS) -o $@ $<

clean:
	rm -f *.o $(bin)
