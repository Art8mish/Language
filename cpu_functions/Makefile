CC = g++

TARGET = $(PREF_EXE)processor

CFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations \
         -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual	      \
         -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op                 \
         -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192         \
         -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE				          \

PREF_OBJ = ./obj/
PREF_EXE = ./assembler/

all : $(TARGET).exe

$(TARGET).exe : $(PREF_OBJ)processor.o
	$(CC) $(CFLAGS) $(PREF_OBJ)processor.o -o $(TARGET).exe

$(PREF_OBJ)processor.o : processor.cpp
	$(CC) $(CFLAGS) -c processor.cpp -o $(PREF_OBJ)processor.o

clean:
	rm $(TARGET).exe $(PREF_OBJ)*.o
