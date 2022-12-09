
#include "../include/language.h"

struct Tree *LangTreeDeserialize(const char *input_file_name)
{
    ERROR_CHECK(input_file_name == NULL, NULL);

    struct Tree *new_tree = TreeCtor();
    ERROR_CHECK(new_tree == NULL, NULL);

    struct WorkingField *onegin_context = CreateWorkingField(input_file_name);
    ERROR_CHECK(onegin_context == NULL, NULL);

    char *buf = onegin_context->chars_buffer;

    LexStruct *lex_structs = LexicalAnalisis(buf);
    ERROR_CHECK(lex_structs == NULL, NULL);
   /*
    new_tree->root = ReadProgram(lex_structs);
    ERROR_CHECK(new_tree->root == NULL, NULL);*/

    return new_tree;    
}
