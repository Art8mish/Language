
#include "../include/language.h"

int main(void)
{
    setlocale(LC_CTYPE, "rus");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    
    struct Tree *tree = LangTreeDeserialize(CODE_IN_FILE_PATH);
    ERROR_CHECK(tree == NULL, 1);

    TREEDUMP(tree, "blyazdec");

    int ser_err = TreeSerialize(tree);
    ERROR_CHECK(ser_err, 2);

    // struct Tree *scnd_tree = TreeDeserialize(TREE_DESERIALIZATION_PATH);
    // ERROR_CHECK(scnd_tree == NULL, 1);

    // TREEDUMP(scnd_tree, "deserialize");
    // int ser_err = TreeSerialize(scnd_tree);
    // ERROR_CHECK(ser_err, 2);

    int dtor_err = TreeDtor(tree);
    ERROR_CHECK(dtor_err, 3); 

    // const char *str = "ÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖØÙÚÛÜİŞßàáâãäå¸æçèéêëìíîïğñòóôõöøùúûüışÿ";
    // printf("rus_str: %s\n", str);

    // const char *trans_str = RusTranslate(str);
    // printf("eng_str: %s\n", trans_str);

    // free((void *)trans_str);
    return 0;
}