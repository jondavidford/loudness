NAME=loudness
CC=g++

MAJOR=0
MINOR=1
REVISION=0
BASENAME=lib$(NAME).so
EXECUTABLE=$(BASENAME).$(MAJOR).$(MINOR).$(REVISION)
TARGET_DIR=/usr/local

CFLAGS = -std=c++11 -c -fPIC -g -Wall -O3

#Debug mode or not
DEBUG=0
ifeq ($(DEBUG),1)
    CFLAGS += -DDEBUG
endif

LDFLAGS=-shared -L/usr/local/lib
LIBS=-lfftw3 -lsndfile #-lrt 
INCS=-I.

SOURCES=../src/cnpy/cnpy.cpp \
../src/Modules/AudioFileCutter.cpp \
../src/Support/TrackBank.cpp \
../src/Support/Module.cpp \
../src/Support/Timer.cpp \
../src/Support/AuditoryTools.cpp \
../src/Support/Spline.cpp \
../src/Support/Filter.cpp \
../src/Support/FilterBank.cpp \
../src/Support/Model.cpp \
../src/Modules/FrameGenerator.cpp \
../src/Modules/FIR.cpp \
../src/Modules/IIR.cpp \
../src/Modules/Butter.cpp \
../src/Modules/RoexBankANSIS3407.cpp \
../src/Modules/FastRoexBank.cpp \
../src/Modules/DoubleRoexBank.cpp \
../src/Modules/PowerSpectrum.cpp \
../src/Modules/PowerSpectrumAndSpatialDetection.cpp \
../src/Modules/WeightSpectrum.cpp \
../src/Modules/CompressSpectrum.cpp \
../src/Modules/SpecificPartialLoudnessGM.cpp \
../src/Modules/IntegratedPartialLoudnessGM.cpp \
../src/Models/DynamicPartialLoudnessGM.cpp

OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

install:
	#install the library and link soname
	install -c $(EXECUTABLE) $(TARGET_DIR)/lib
	ln -sf $(TARGET_DIR)/lib/$(EXECUTABLE) $(TARGET_DIR)/lib/$(BASENAME)
	ldconfig
	#copy src folder and delete all but headers
	mkdir -p $(TARGET_DIR)/include/$(NAME)
	cp -r ../src/* $(TARGET_DIR)/include/$(NAME)
	find $(TARGET_DIR)/include/$(NAME) ! -name \*.h -type f -delete 

uninstall:
	rm -rf $(TARGET_DIR)/include/$(NAME)
	rm -rf $(TARGET_DIR)/lib/$(EXECUTABLE)



