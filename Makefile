INCLUDE_DIRS = .
INCLUDES=$(foreach inc,$(INCLUDE_DIRS),-I$(inc))

librtc.so: sresrtc.o parser.o
	g++ -Wl,-soname,librtc.so $< -shared -o $@
	rm *.o

sresrtc.o: sresrtc.cpp 
	g++ -o $@ $(INCLUDES) -fPIC -c -D RTC_API_VER=0x1a $<

parser.o: parser.cpp
	g++ -o $@ $(INCLUDES) -fPIC -c $<

clean:
	rm librtc.so