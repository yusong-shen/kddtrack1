export CC  = gcc
export CXX = g++

export CFLAGS = -Wall -O3 -msse2 -I../svd-feature

BIN = kddcup_combine_ugroup
OBJ = apex_svd_data.o
.PHONY: clean all clean_all

all: $(BIN)

export LDFLAGS= -lpthread -lm

apex_svd_data.o:../svd-feature/apex_svd_data.cpp ../svd-feature/apex_svd_data.h
kddcup_combine_ugroup:kddcup_combine_ugroup.cpp apex_svd_data.o

$(BIN) :
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $(filter %.cpp %.o %.c, $^)

$(OBJ) : 
	$(CXX) -c $(CFLAGS) -o $@ $(firstword $(filter %.cpp %.c, $^) )

install:
	cp -f -r $(BIN)  $(INSTALL_PATH)

clean:
	$(RM) $(OBJ) $(BIN) *~
