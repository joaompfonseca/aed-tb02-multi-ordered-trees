/*
 * AED, Janeiro de 2022
 *
 * Solução do segundo trabalho prático (multi-ordered tree)
 *
 * 103154 - João Fonseca
 * 103183 - Diogo Paiva
 * 103865 - Jorge Silva
 */

#define _GNU_SOURCE // para ter a função strcasestr na função filter_tree_node
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AED_2021_A02.h"

/* ------------------------------------ Estruturas de Dados ------------------------------------- */


/**
 * @brief Estrutura do nó personalizada da árvore
 * 
 * Queremos manter três árvores ordenadas (utilizando os mesmos nós!), logo necessitamos de três ponteiros para a esquerda e três ponteiros para a direita.
 * Ao inserir um novo nó, fazê-mo-lo três vezes (uma vez por cada índice), precisando assim de três raízes.
 */
typedef struct tree_node_s
{
    char name[MAX_NAME_SIZE + 1];                                     // índice 0
    char zip_code[MAX_ZIP_CODE_SIZE + 1];                             // índice 1
    char telephone_number[MAX_TELEPHONE_NUMBER_SIZE + 1];             // índice 2
    char social_security_number[MAX_SOCIAL_SECURITY_NUMBER_SIZE + 1]; // índice 3
    struct tree_node_s *left[N_DATA_TYPES];                           // ponteiros para a esquerda (um para cada índice) - esquerda significa menor
    struct tree_node_s *right[N_DATA_TYPES];                          // ponteiros para a direita (um para cada índice) - direita significa maior
} tree_node_t;

/* ------------------------------------------ Funções ------------------------------------------- */

/**
 * @brief Compara dois nós da árvore.
 *
 * @param node1     Ponteiro para o nó 1
 * @param node2     Ponteiro para o nó 2
 * @param type      Árvore considerada
 * @return          Resultado da comparação
 */
int compare_tree_nodes(tree_node_t *node1, tree_node_t *node2, int type)
{
    int i, c;

    for (i = 0; i < N_DATA_TYPES; i++)
    {
        switch (type)
        {
        case 0:
            c = strcmp(node1->name, node2->name);
            break;
        case 1:
            c = strcmp(node1->zip_code, node2->zip_code);
            break;
        case 2:
            c = strcmp(node1->telephone_number, node2->telephone_number);
            break;
        case 3:
            c = strcmp(node1->social_security_number, node2->social_security_number);
            break;
        }

        if (c != 0)
            return c;                                   // nós diferem nesta árvore
        type = (type == N_DATA_TYPES - 1) ? 0 : type++; // próxima árvore
    }
    return 0;
}

/**
 * @brief Filtra o nó da árvore.
 *
 * @param node      Ponteiro para o nó
 * @param type      Árvore considerada
 * @param filter    Filtro
 * @return          Resultado da filtragem
 */
int filter_tree_node(tree_node_t *node, int type, char *filter)
{
    char *data;

    switch (type)
    {
    case 0:
        data = node->name;
        break;
    case 1:
        data = node->zip_code;
        break;
    case 2:
        data = node->telephone_number;
        break;
    case 3:
        data = node->social_security_number;
    default:
        data = "";
        break;
    }

    return (strcasestr(data, filter)) ? 1 : 0;
}

/**
 * @brief Imprime os dados do nó.
 *
 * @param node      Ponteiro para o nó
 * @param count     Número de nós listados
 */
void visit(tree_node_t *node, int *count)
{
    printf("Person #%d\n", ++(*count));
    printf("name ..................... %s\n", node->name);
    printf("zip code ................. %s\n", node->zip_code);
    printf("telephone number ......... %s\n", node->telephone_number);
    printf("social security number ... %s\n", node->social_security_number);
}
/**
 * @brief Insere nó na árvore, de uma forma recursiva.
 *
 * @param link      Ponteiro para o ponteiro que aponta para o nó atual
 * @param type      Árvore considerada
 * @param node      Ponteiro para o nó a inserir
 */
void tree_insert(tree_node_t **link, int type, tree_node_t *node)
{
    if (*link == NULL)
        *link = node;
    else if (compare_tree_nodes(node, *link, type) <= 0)
        tree_insert(&((*link)->left[type]), type, node); // ramo esquerdo
    else
        tree_insert(&((*link)->right[type]), type, node); // ramo direito
}

/**
 * @brief Procura nó na árvore, de uma forma recursiva.
 *
 * @param link      Ponteiro para o nó atual
 * @param type      Árvore considerada
 * @param node      Ponteiro para o nó a procurar
 * @return          Ponteiro para o nó encontrado na árvore, NULL caso contrário
 */
tree_node_t *find(tree_node_t *link, int type, tree_node_t *node)
{
    if (link == NULL || compare_tree_nodes(node, link, type) == 0)
        return link;
    else if (compare_tree_nodes(node, link, type) < 0)
        return find(link->left[type], type, node); // ramo esquerdo
    else
        return find(link->right[type], type, node); // ramo direito
}

/**
 * @brief Determina a maior profundidade da árvore, de uma forma recursiva.
 *
 * @param link      Ponteiro para o nó atual
 * @param type      Árvore considerada
 * @return          Valor da maior profundidade da árvore
 */
int tree_depth(tree_node_t *link, int type)
{
    if (link == NULL)
        return 0;

    int left_depth = tree_depth(link->left[type], type);   // ramo esquerdo
    int right_depth = tree_depth(link->right[type], type); // ramo direito

    return (left_depth > right_depth) ? left_depth + 1 : right_depth + 1;
}

/**
 * @brief Lista dados dos nós da árvore que passam no filtro especificado, de uma forma recursiva.
 *
 * @param link      Ponteiro para o nó atual
 * @param type      Árvore considerada
 * @param count     Número de nós listados
 * @param filter    Filtro
 */
void list(tree_node_t *link, int type, int *count, char *filter)
{
    if (link == NULL)
        return;

    list(link->left[type], type, count, filter);
    if (filter_tree_node(link, type, filter))
        visit(link, count);
    list(link->right[type], type, count, filter);
}

/* -------------------------------- Fluxo de Execução Principal --------------------------------- */

int main(int argc, char **argv)
{
    double dt;

    /*
     * processar os argumentos da linha de comandos
     */
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s student_number number_of_persons [options ...]\n", argv[0]);
        fprintf(stderr, "Recognized options:\n");
        fprintf(stderr, "  -list[N] [filter]             # list the tree contents, sorted by key index N (the default is index 0)\n");
        // place a description of your own options here
        return 1;
    }

    int student_number = atoi(argv[1]);
    if (student_number < 1 || student_number >= 1000000)
    {
        fprintf(stderr, "Bad student number (%d) --- must be an integer belonging to [1,1000000{\n", student_number);
        return 1;
    }

    int n_persons = atoi(argv[2]);
    if (n_persons < 3 || n_persons > 10000000)
    {
        fprintf(stderr, "Bad number of persons (%d) --- must be an integer belonging to [3,10000000]\n", n_persons);
        return 1;
    }

    /*
     * gerar todos os dados
     */
    tree_node_t *persons = (tree_node_t *)calloc((size_t)n_persons, sizeof(tree_node_t));
    if (persons == NULL)
    {
        fprintf(stderr, "Output memory!\n");
        return 1;
    }

    aed_srandom(student_number);
    for (int i = 0; i < n_persons; i++)
    {
        random_name(&(persons[i].name[0]));
        random_zip_code(&(persons[i].zip_code[0]));
        random_telephone_number(&(persons[i].telephone_number[0]));
        random_social_security_number(&(persons[i].social_security_number[0]));
        for (int j = 0; j < N_DATA_TYPES; j++)
            persons[i].left[j] = persons[i].right[j] = NULL; // garantir que os ponteiros são NULL inicialmente
    }

    /*
     * criar as árvores binárias ordenadas
     */
    dt = cpu_time();

    tree_node_t *roots[N_DATA_TYPES]; // N índices, N raízes
    for (int main_index = 0; main_index < N_DATA_TYPES; main_index++)
        roots[main_index] = NULL;
    for (int i = 0; i < n_persons; i++)
        for (int main_index = 0; main_index < N_DATA_TYPES; main_index++)
            tree_insert(&(roots[main_index]), main_index, &(persons[i])); // place your code here to insert &(persons[i]) in the tree with number main_index

    dt = cpu_time() - dt;
    printf("Tree creation time (%d persons): %.3es\n", n_persons, dt);

    /*
     * procurar nó na árvore
     */
    for (int main_index = 0; main_index < N_DATA_TYPES; main_index++)
    {
        dt = cpu_time();

        for (int i = 0; i < n_persons; i++)
        {
            tree_node_t n = persons[i];                                   // copiar os dados do nó
            if (find(roots[main_index], main_index, &n) != &(persons[i])) // place your code here to find a given person, searching for it using the tree with number main_index
            {
                fprintf(stderr, "person %d not found using index %d\n", i, main_index);
                return 1;
            }
        }

        dt = cpu_time() - dt;
        printf("Tree search time (%d persons, index %d): %.3es\n", n_persons, main_index, dt);
    }

    /*
     * determinar a maior profundidade da árvore
     */
    for (int main_index = 0; main_index < N_DATA_TYPES; main_index++)
    {
        dt = cpu_time();

        int depth = tree_depth(roots[main_index], main_index); // place your code here to compute the depth of the tree with number main_index

        dt = cpu_time() - dt;
        printf("Tree depth for index %d: %d (done in %.3es)\n", main_index, depth, dt);
    }

    /*
     * processar os argumentos opcionais da linha de comandos
     */
    for (int i = 3; i < argc; i++)
    {
        if (strncmp(argv[i], "-list", 5) == 0)
        { // list all (optional)
            int main_index = atoi(&(argv[i][5]));
            if (main_index < 0)
                main_index = 0;
            if (main_index > 2)
                main_index = 2;

            char *filter = (argc == i + 1 || argv[i + 1][0] == '-') ? "" : argv[++i];

            printf("List of persons:\n");
            int count = 0;
            list(roots[main_index], main_index, &count, filter); // place your code here to traverse, in order, the tree with number main_index
        }
    }

    /*
     * limpeza - não esquecer de testar o programa com o valgrind para verificar que não existem memory leaks
     */
    free(persons);
    return 0;
}
