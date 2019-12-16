%: %.cpp
	g++ -std=c++11 $(CPPFLAGS) $(ADDITIONAL_CPPFLAGS) $< -o $@ $(LDFLAGS) -lgmsh

all: mortar-1 square square_coarse build-square node-face
