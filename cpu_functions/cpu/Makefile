CC = g++

TARGET = cpu

CFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations \
         -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual	      \
         -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op                 \
         -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192         \
         -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE				          \

PREF_STK_SRC = ./../stack_functions/src/
PREF_CPU_SRC = ./src/
PREF_OBJ = ./obj/

CPU_SRC = $(wildcard $(PREF_CPU_SRC)*.cpp)
STK_SRC = $(wildcard $(PREF_STK_SRC)*.cpp)

CPU_OBJ = $(patsubst $(PREF_CPU_SRC)%.cpp, $(PREF_OBJ)%.o, $(CPU_SRC))
STK_OBJ = $(patsubst $(PREF_STK_SRC)%.cpp, $(PREF_OBJ)%.o, $(STK_SRC))

OBJ = $(CPU_OBJ) $(STK_OBJ)
#SRC = $(CPU_SRC) $(STK_SRC)

all : $(TARGET).exe

$(TARGET).exe : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o cpu.exe

$(PREF_OBJ)%.o : $(PREF_CPU_SRC)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(PREF_OBJ)%.o : $(PREF_STK_SRC)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm $(TARGET).exe $(PREF_OBJ)*.o
