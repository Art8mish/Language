
#include "../include/language.h"

int main(void)
{


    struct Tree *tree = TreeDeserialize(TREE_DESERIALIZATION_PATH);
    ERROR_CHECK(tree == NULL, 1);

    TREEDUMP(tree, "deserialize");

    int ser_err = TreeSerialize(tree);
    ERROR_CHECK(ser_err, 2);

    int dtor_err = TreeDtor(tree);
    ERROR_CHECK(dtor_err, 3);

    return 0;
}