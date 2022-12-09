CC = g++

TARGET = language

CFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations \
         -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual	      \
         -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op                 \
         -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192         \
         -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE				     	  \

PREF_TREE_SRC = ./tree_functions/src/
PREF_ONGN_SRC = ./onegin_functions/src/

PREF_DEF_SRC = ./src/
PREF_OBJ = ./obj/

 DEF_SRC = $(wildcard $(PREF_DEF_SRC)*.cpp)
TREE_SRC = $(wildcard $(PREF_TREE_SRC)*.cpp)
ONGN_SRC = $(wildcard $(PREF_ONGN_SRC)*.cpp)

 DEF_OBJ = $(patsubst $(PREF_DEF_SRC)%.cpp,  $(PREF_OBJ)%.o, $(DEF_SRC))
TREE_OBJ = $(patsubst $(PREF_TREE_SRC)%.cpp, $(PREF_OBJ)%.o, $(TREE_SRC))
ONGN_OBJ = $(patsubst $(PREF_ONGN_SRC)%.cpp, $(PREF_OBJ)%.o, $(ONGN_SRC))

OBJ = $(DEF_OBJ) $(TREE_OBJ) $(ONGN_OBJ)

all : $(TARGET).exe

$(TARGET).exe : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET).exe

$(PREF_OBJ)%.o : $(PREF_DEF_SRC)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(PREF_OBJ)%.o : $(PREF_TREE_SRC)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	
$(PREF_OBJ)%.o : $(PREF_ONGN_SRC)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm $(TARGET).exe $(PREF_OBJ)*.o
