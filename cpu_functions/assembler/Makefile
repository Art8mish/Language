CC = g++

TARGET = assembler

CFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations \
         -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual	      \
         -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op                 \
         -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192         \
         -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE				          \

PREF_ASM_SRC = ./src/
PREF_ONG_SRC = ./../onegin_functions/

PREF_OBJ = ./obj/

ASM_SRC = $(wildcard $(PREF_ASM_SRC)*.cpp)
ONG_SRC = $(wildcard $(PREF_ONG_SRC)*.cpp)

ASM_OBJ = $(patsubst $(PREF_ASM_SRC)%.cpp, $(PREF_OBJ)%.o, $(ASM_SRC))
ONG_OBJ = $(patsubst $(PREF_ONG_SRC)%.cpp, $(PREF_OBJ)%.o, $(ONG_SRC))

OBJ = $(ASM_OBJ) $(ONG_OBJ)
#SRC = $(CPU_SRC) $(STK_SRC)

all : $(TARGET).exe

$(TARGET).exe : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET).exe

$(PREF_OBJ)%.o : $(PREF_ASM_SRC)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(PREF_OBJ)%.o : $(PREF_ONG_SRC)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm $(TARGET).exe $(PREF_OBJ)*.o
