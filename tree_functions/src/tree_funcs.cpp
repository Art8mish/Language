
#include "../include/tree.h"

struct Tree *TreeCtor(void)
{
    struct Tree *tree = (struct Tree *) calloc(1, sizeof(struct Tree));
    ERROR_CHECK(tree == NULL, NULL);

    tree->root = NULL;
    
    return tree;
}

int TreeDtor(struct Tree *tree)
{
    ERROR_CHECK(tree == NULL, ERROR_NULL_PTR);

    int node_dtor_err = TreeNodeDtor(tree->root);
    ERROR_CHECK(node_dtor_err, ERROR_TREE_NODE_DTOR);

    free(tree);

    return SUCCESS;
}

int TreeNodeDtor(struct TreeNode *curr_node)
{
    if (curr_node == NULL)
        return SUCCESS;

    if (curr_node->left != NULL)
    {
        int node_dtor_err = TreeNodeDtor(curr_node->left);
        ERROR_CHECK(node_dtor_err, ERROR_TREE_NODE_DTOR);
    }

    if (curr_node->right != NULL)
    {
        int node_dtor_err = TreeNodeDtor(curr_node->right);
        ERROR_CHECK(node_dtor_err, ERROR_TREE_NODE_DTOR);
    }

    FREE_TREE_NODE_VALUE(curr_node);
    free(curr_node);

    return SUCCESS;
}

struct TreeNode *TreeNodeCtor(tree_elem_t arg, struct TreeNode *left_node, struct TreeNode *right_node)
{
    CHECK_TREE_ELEM(arg, NULL);

    struct TreeNode *new_node = (struct TreeNode *) calloc(1, sizeof(struct TreeNode));
    ERROR_CHECK(new_node == NULL, NULL);
    
    new_node->value = arg;

    new_node->left = left_node;
    if (left_node != NULL)
        left_node->parent = new_node;

    new_node->right = right_node;
    if (right_node != NULL)
        right_node->parent = new_node;

    new_node->parent = NULL;

    return new_node;
}

struct TreeNode *TreeNodeCopy(struct TreeNode *curr_node)
{
    if (curr_node == NULL)
        return NULL;

    struct TreeNode *left_node = NULL;
    if (curr_node->left != NULL)
    {
        left_node = TreeNodeCopy(curr_node->left);
        ERROR_CHECK(left_node == NULL, NULL);
    }

    struct TreeNode *right_node = NULL;
    if (curr_node->right != NULL)
    {
        right_node = TreeNodeCopy(curr_node->right);
        ERROR_CHECK(right_node == NULL, NULL);
    }

    tree_elem_t val = NULL;
    CREATE_TREE_NODE_VALUE(val, NULL);
    COPY_TREE_VAl(val, curr_node->value);

    struct TreeNode *new_node = TreeNodeCtor(val, left_node, right_node);
    ERROR_CHECK(new_node == NULL, NULL);

    return new_node;
}

int TreeNodeTie(struct TreeNode *parent_node,
                struct TreeNode *curr_node, TreeTiePath tie_path)
{
    ERROR_CHECK(parent_node == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(curr_node   == NULL, ERROR_NULL_PTR);

    int tree_node_dtor_err = 0;
    switch (tie_path)
    {
        case TREE_TIE_LEFT  : 
            {
                tree_node_dtor_err = TreeNodeDtor(parent_node->left);
                ERROR_CHECK(tree_node_dtor_err, ERROR_TREE_NODE_DTOR);

                parent_node->left = curr_node;
                curr_node->parent = parent_node;
                break;
            }

        case TREE_TIE_RIGHT : 
            {
                tree_node_dtor_err = TreeNodeDtor(parent_node->right);
                ERROR_CHECK(tree_node_dtor_err, ERROR_TREE_NODE_DTOR);

                parent_node->right = curr_node;
                curr_node->parent  = parent_node;
                break;
            }
    
        default: 
            {
                return ERROR_WRONG_TREE_INSERT_PATH;
                break;
            }
    }

    return SUCCESS;
}

int TreeInsertLatin(struct TreeNode *parent_node, tree_elem_t arg)
{
    ERROR_CHECK(parent_node == NULL, ERROR_NULL_PTR);
    CHECK_TREE_ELEM(arg, ERROR_INCORRECT_TREE_ELEM);

    ERROR_CHECK(parent_node->left  == NULL && 
                parent_node->right != NULL, ERROR_TREE_INCORRECT_OP);

    if (parent_node->left != NULL && parent_node->right != NULL)
        return SUCCESS;    

    struct TreeNode *curr_node = TreeNodeCtor(arg, NULL, NULL);
    ERROR_CHECK(curr_node == NULL, ERROR_TREE_NODE_CTOR);
    curr_node->parent = parent_node;

    if (parent_node->left == NULL)
    {
        int tree_tie_err = TreeNodeTie(parent_node, curr_node, TREE_TIE_LEFT);
        ERROR_CHECK(tree_tie_err, ERROR_TREE_TIE);

        return SUCCESS;
    }

    if (parent_node->right == NULL)
    {
        int tree_tie_err = TreeNodeTie(parent_node, curr_node, TREE_TIE_RIGHT);
        ERROR_CHECK(tree_tie_err, ERROR_TREE_TIE);
    }

    return SUCCESS;
}

int TreeInsertArabic(struct TreeNode *parent_node, tree_elem_t arg)
{
    ERROR_CHECK(parent_node == NULL, ERROR_NULL_PTR);
    CHECK_TREE_ELEM(arg, ERROR_INCORRECT_TREE_ELEM);

    ERROR_CHECK(parent_node->left  != NULL && 
                parent_node->right == NULL, ERROR_TREE_INCORRECT_OP);

    if (parent_node->left != NULL && parent_node->right != NULL)
        return SUCCESS;

    struct TreeNode *curr_node = TreeNodeCtor(arg, NULL, NULL);
    ERROR_CHECK(curr_node == NULL, ERROR_TREE_NODE_CTOR);
    curr_node->parent = parent_node;

    if (parent_node->right == NULL)
    {
        int tree_tie_err = TreeNodeTie(parent_node, curr_node, TREE_TIE_LEFT);
        ERROR_CHECK(tree_tie_err, ERROR_TREE_TIE);

        return SUCCESS;
    }

    if (parent_node->left == NULL)
    {
        int tree_tie_err = TreeNodeTie(parent_node, curr_node, TREE_TIE_RIGHT);
        ERROR_CHECK(tree_tie_err, ERROR_TREE_TIE);
    }

    return SUCCESS;
}

int TreeInsert(struct Tree *tree, struct TreeNode *parent_node,
               tree_elem_t arg, TreeInsertPath insert_path)
{
    ERROR_CHECK(tree == NULL, ERROR_NULL_PTR);
    CHECK_TREE_ELEM(arg, ERROR_INCORRECT_TREE_ELEM);

    struct TreeNode *curr_node = TreeNodeCtor(arg, NULL, NULL);
    ERROR_CHECK(curr_node == NULL, ERROR_TREE_NODE_CTOR);
    curr_node->parent = parent_node;

    switch (insert_path)
    {
        case TREE_INSERT_FIRST : tree->root = curr_node;
                                 break;

        case TREE_INSERT_LEFT  : ERROR_CHECK(parent_node       == NULL, ERROR_NULL_PTR);
                                 ERROR_CHECK(parent_node->left != NULL, ERROR_TREE_INCORRECT_OP);
                                 parent_node->left = curr_node;
                                 break;

        case TREE_INSERT_RIGHT : ERROR_CHECK(parent_node == NULL, ERROR_NULL_PTR);
                                 ERROR_CHECK(parent_node->right != NULL, ERROR_TREE_INCORRECT_OP);
                                 parent_node->right = curr_node;
                                 break;
        
        default                : return ERROR_WRONG_TREE_INSERT_PATH;
                                 break;
    }

    return SUCCESS;
}


int TreeSerialize(const struct Tree *tree)
{
    ERROR_CHECK(tree == NULL, ERROR_NULL_PTR);

    FILE *tree_f = fopen(TREE_SERIALIZATION_PATH, "a");
    ERROR_CHECK(tree_f == NULL, ERROR_OPENING_FILE);

    if (tree->root != NULL)
    {
        int save_node_err = SerializeNode(tree_f, tree->root);
        FILE_ERROR_CHECK(save_node_err, ERROR_SAVE_NODE, tree_f);
    }

    fprintf(tree_f, "\n");
    int fclose_err = fclose(tree_f);
    ERROR_CHECK(fclose_err != 0, ERROR_CLOSING_FILE);

    return SUCCESS;
}

int SerializeNode(FILE *tree_f, const struct TreeNode *curr_node)
{
    ERROR_CHECK(tree_f    == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(curr_node == NULL, ERROR_NULL_PTR);
    
    bool child_ex = false;
    static unsigned int recur_count = 0;
    recur_count++;

    fprintf(tree_f, "\n");
    for (unsigned int i = 1; i < recur_count; i++)
        fprintf(tree_f, "\t");

    if (curr_node != NULL)
    {
        fprintf(tree_f, "{");
        int print_tree_val_err = PrintTreeValue(tree_f, curr_node);
        ERROR_CHECK(print_tree_val_err, ERROR_PRINT_TREE_VALUE);
    }

    if (curr_node->left != NULL)
    {
        child_ex = true;
        int save_node_err = SerializeNode(tree_f, curr_node->left);
        ERROR_CHECK(save_node_err, ERROR_SAVE_NODE);
    }

    if (curr_node->right != NULL)
    {
        child_ex = true;
        int save_node_err = SerializeNode(tree_f, curr_node->right);
        ERROR_CHECK(save_node_err, ERROR_SAVE_NODE);
    }

    if (child_ex)
        for (unsigned int i = 1; i < recur_count; i++)
            fprintf(tree_f, "\t");

    fprintf(tree_f, "}\n");

    recur_count--;

    return SUCCESS;
}

int PrintTreeValue(FILE *tree_f, const struct TreeNode *curr_node)
{
    ERROR_CHECK(tree_f    == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(curr_node == NULL, ERROR_NULL_PTR);

    switch (curr_node->value->type)
    {
        case T_NIL   :  fprintf(tree_f, " NIL ");
                        break;

        case T_ST    :  fprintf(tree_f, " ST ");
                        break;

        case T_VAR   :  fprintf(tree_f, " VAR ");
                        break;
        
        case T_IF    :  fprintf(tree_f, " IF ");
                        break;
        
        case T_ELSE  :  fprintf(tree_f, " ELSE ");
                        break;

        case T_WHILE :  fprintf(tree_f, " WHILE ");
                        break;

        case T_FUNC  :  fprintf(tree_f, " FUNC ");
                        break;

        case T_RET   :  fprintf(tree_f, " RET ");
                        break;

        case T_CALL  :  fprintf(tree_f, " CALL ");
                        break;

        case T_PARAM :  fprintf(tree_f, " PARAM ");
                        break;

        case T_NUM   :  fprintf(tree_f, " %g ", curr_node->value->num);
                        break;

        case T_EQ    :  fprintf(tree_f, " EQ ");
                        break;

        case T_OP    :  switch (curr_node->value->arithm_op)
                        {
                            case OP_PSN : fprintf(tree_f, " ERROR_ARITHM_OP ");
                                          break;
                            case OP_ADD : fprintf(tree_f, " ADD ");
                                          break;
                            case OP_SUB : fprintf(tree_f, " SUB ");
                                          break;
                            case OP_MUL : fprintf(tree_f, " MUL ");
                                          break;
                            case OP_DIV : fprintf(tree_f, " DIV ");
                                          break;
                            
                            default     : fprintf(tree_f, " ERROR_ARITHM_OP ");
                                          break;
                        }
                        break;

        case T_STR   :  fprintf(tree_f, " \"%s\" ", curr_node->value->str);
                        break; 
        default:
            break;
    }

    return SUCCESS;
}


struct Tree *TreeDeserialize(const char *input_file_name)
{
    ERROR_CHECK(input_file_name == NULL, NULL);

    struct Tree *new_tree = TreeCtor();
    ERROR_CHECK(new_tree == NULL, NULL);

    struct WorkingField *onegin_context = CreateWorkingField(input_file_name);
    ERROR_CHECK(onegin_context == NULL, NULL);

    char *buf = onegin_context->chars_buffer;

    int read_node_err = DeserializeNode(new_tree, new_tree->root, &buf);
    ERROR_CHECK(read_node_err, NULL);

    return new_tree;    
}

#define DESERIALIZE_ITER(insert_path)                                                   \
        do {                                                                            \
            char value[MAX_TREE_STR_SIZE] = { 0 };                                      \
            int read_val_err = ReadValue(value, buf);                                   \
            ERROR_CHECK(read_val_err, ERROR_READ_VALUE);                                \
            struct LangNode *new_val = ProcessTreeValue(value);                         \
            ERROR_CHECK(new_val == NULL, ERROR_PROCESS_TREE_VALUE);                     \
            int insert_err = TreeInsert(new_tree, prev_node, new_val, insert_path);     \
            ERROR_CHECK(insert_err, ERROR_TREE_INSERT);                                 \
        } while (false)  

int DeserializeNode(struct Tree *new_tree, struct TreeNode *prev_node, char **buf)
{
    ERROR_CHECK(new_tree  == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(buf       == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(*buf      == NULL, ERROR_NULL_PTR);

    SKIP_SPACE((*buf), SUCCESS);

    if (((**buf) == '<'))
    {
        SKIP_COMMENT((*buf), ERROR_SYNTAX);
        (*buf)++;
    }

    if (((**buf) == '$'))
    {
        SKIP_EXPANSION((*buf), ERROR_SYNTAX);
        (*buf)++;
    }

    if ((**buf) == '}')
    {
        (*buf)++;
        return SUCCESS;
    }

    if ((**buf) == '{')
    {
        (*buf)++;
        if (prev_node == NULL)
        {
            DESERIALIZE_ITER(TREE_INSERT_FIRST);

            int read_node_err = DeserializeNode(new_tree, new_tree->root, buf);
            ERROR_CHECK(read_node_err, ERROR_READ_NODE);

            return SUCCESS;
        }

        DESERIALIZE_ITER(TREE_INSERT_LEFT);

        int read_node_err = DeserializeNode(new_tree, prev_node->left, buf);
        ERROR_CHECK(read_node_err, ERROR_READ_NODE);
    }

    SKIP_SPACE((*buf), ERROR_SYNTAX);
    if ((**buf) == '}')
    {
        (*buf)++;
        return SUCCESS;
    }

    if ((**buf) == '{')
    {
        (*buf)++;

        DESERIALIZE_ITER(TREE_INSERT_RIGHT);

        int read_node_err = DeserializeNode(new_tree, prev_node->right, buf);
        ERROR_CHECK(read_node_err, ERROR_READ_NODE);
    }

    SKIP_SPACE((*buf), ERROR_SYNTAX);

    if (**buf != '}')
        return ERROR_SYNTAX;

    (*buf)++;

    return SUCCESS;
}

int ReadValue(char *value, char **buf)
{
    ERROR_CHECK( buf  == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(*buf  == NULL, ERROR_NULL_PTR);

    int symb_count = 0;
    int sscanf_ret = sscanf((*buf), " %s%n", value,  &symb_count);
    printf("value: %s (%d)\n", value, sscanf_ret);
    //ERROR_CHECK(sscanf_ret != 2, ERROR_SYNTAX);

    (*buf) += symb_count;

    return SUCCESS;
}


struct LangNode *ProcessTreeValue(char *value)
{
    ERROR_CHECK(value == NULL, NULL);

    struct LangNode *new_struct = (struct LangNode *) calloc(1, sizeof(struct LangNode));
    ERROR_CHECK(new_struct == NULL, NULL);

    if (strcmp(value, "NIL") == 0)
        new_struct->type = T_NIL;

    else if (strcmp(value, "ST") == 0)
        new_struct->type = T_ST;

    else if (strcmp(value, "VAR") == 0)
        new_struct->type = T_VAR;
    

    else if (strcmp(value, "IF") == 0)
        new_struct->type = T_IF;

    else if (strcmp(value, "ELSE") == 0)
        new_struct->type = T_ELSE;

    else if (strcmp(value, "WHILE") == 0)
        new_struct->type = T_WHILE;


    else if (strcmp(value, "FUNC") == 0)
        new_struct->type = T_FUNC;

    else if (strcmp(value, "RET") == 0)
        new_struct->type = T_RET;

    else if (strcmp(value, "CALL") == 0)
        new_struct->type = T_CALL;

    else if (strcmp(value, "PARAM") == 0)
        new_struct->type = T_PARAM;
    

    else if (isdigit(value[0]))
    {
        double num = ConvertStrToNum(value);
        ERROR_CHECK(isnan(num), NULL);

        new_struct->type = T_NUM;
        new_struct->num  = num;
    }

    else if (strcmp(value, "EQ") == 0)
        new_struct->type = T_EQ;

    else if (strcmp(value, "ADD") == 0)
    {
        new_struct->type      = T_OP;
        new_struct->arithm_op = OP_ADD;
    }

    else if (strcmp(value, "SUB") == 0)
    {
        new_struct->type      = T_OP;
        new_struct->arithm_op = OP_SUB;
    }

    else if (strcmp(value, "MUL") == 0)
    {
        new_struct->type      = T_OP;
        new_struct->arithm_op = OP_MUL;
    }

    else if (strcmp(value, "DIV") == 0)
    {
        new_struct->type      = T_OP;
        new_struct->arithm_op = OP_DIV;
    }

    else if (value[0] == '"')
    {
        size_t val_len = strlen(value);
        ERROR_CHECK(value[val_len - 1] != '"', NULL);
        value[val_len - 1] = '\0';
        
        new_struct->type = T_STR;
        new_struct->str  = strdup(value + 1);
        ERROR_CHECK(new_struct->str == NULL, NULL);
    }

    else
        return NULL;

    return new_struct;
}