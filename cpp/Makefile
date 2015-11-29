export CC  = gcc
export CXX = g++
export CFLAGS = -O3 -Wall -lm
BIN = mkUserID mkItemID mkItemDayBin mkUserDayBias mkUserDayInterpolation mkTaxonomyNN make_imfb

all: $(BIN)
clean:
	rm -rf $(BIN) *~
mkUserID: mkUserID.cpp
mkItemID: mkItemID.cpp
mkItemDayBin: mkItemDayBin.cpp
mkUserDayBias: mkUserDayBias.cpp
mkUserDayInterpolation: mkUserDayInterpolation.cpp
mkTaxonomyNN: mkTaxonomyNN.cpp
make_imfb: make_imfb.cpp

$(BIN):
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $(filter %.cpp %.o %.c, $^)