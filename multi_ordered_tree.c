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
#include <getopt.h>
#include <regex.h>
#include "AED_2021_A02.h"

/* ------------------------------------------- Macros ------------------------------------------- */

/**
 * @brief Utilização correta do programa
 */
#define USAGE "Sintaxe: %s n_estudante n_pessoas [opções ...]\n"                                                                                         \
              "Opções válidas:\n"                                                                                                                       \
              "    -l [N], --list [N]              # lista o conteúdo da árvore, ordenado pelos dados correspondentes ao índice N (por defeito, N=0)\n" \
              "    -f [regex], --filter [regex]    # filtra o conteúdo listado\n"

/**
 * @brief Flags da expressão regular (do filtro)
 */
#define REGEX_FLAGS (REG_EXTENDED | REG_NOSUB)

/* ------------------------------------ Estruturas de Dados ------------------------------------- */

/**
 * @brief Opções disponíveis no programa.
 */
static struct option long_options[] = {
    {"filter", required_argument, NULL, 'f'},
    {"list", required_argument, NULL, 'l'},
    {NULL, 0, NULL, 0}};

/**
 * @brief Estrutura do nó personalizado da árvore
 *
 * Queremos manter N_MAIN_INDEXES árvores ordenadas (utilizando os mesmos nós!), logo necessitamos de N_MAIN_INDEXES ponteiros para a esquerda e N_MAIN_INDEXES ponteiros para a direita.
 * Ao inserir um novo nó, fazê-mo-lo N_MAIN_INDEXES vezes (uma vez por cada índice), precisando assim de N_MAIN_INDEXES raízes.
 */
typedef struct tree_node_s
{
    char name[MAX_NAME_SIZE + 1];                                     // índice 0
    char zip_code[MAX_ZIP_CODE_SIZE + 1];                             // índice 1
    char telephone_number[MAX_TELEPHONE_NUMBER_SIZE + 1];             // índice 2
    char social_security_number[MAX_SOCIAL_SECURITY_NUMBER_SIZE + 1]; // índice 3
    struct tree_node_s *left[N_MAIN_INDEXES];                         // ponteiros para a esquerda (um para cada índice) - esquerda significa menor
    struct tree_node_s *right[N_MAIN_INDEXES];                        // ponteiros para a direita (um para cada índice) - direita significa maior
} tree_node_t;

/* ------------------------------------------ Funções ------------------------------------------- */

/**
 * @brief Compara dois nós da árvore.
 *
 * @param node1      Ponteiro para o nó 1
 * @param node2      Ponteiro para o nó 2
 * @param main_index Índice da árvore
 * @return           Resultado da comparação
 */
int compare_tree_nodes(tree_node_t *node1, tree_node_t *node2, int main_index)
{
    int i, c;

    for (i = 0; i < N_MAIN_INDEXES; i++)
    {
        switch (main_index)
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
            return c;                                                         // nós diferem nesta árvore
        main_index = (main_index == N_MAIN_INDEXES - 1) ? 0 : main_index + 1; // próxima árvore
    }
    return 0;
}

/**
 * @brief Filtra o nó da árvore.
 *
 * @param node       Ponteiro para o nó
 * @param main_index Índice da árvore
 * @param filter     Ponteiro para o filtro
 * @return           Resultado da filtragem
 */
int filter_tree_node(tree_node_t *node, int main_index, regex_t *filter)
{
    char *data = NULL;

    switch (main_index)
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
        break;
    }

    return (regexec(filter, data, 0, NULL, 0)) ? 0 : 1;
}

/**
 * @brief Imprime os dados do nó.
 *
 * @param node  Ponteiro para o nó
 * @param count Ponteiro para o número de nós listados
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
 * @param link       Ponteiro para o ponteiro que aponta para o nó atual
 * @param main_index Índice da árvore
 * @param node       Ponteiro para o nó a inserir
 */
void tree_insert(tree_node_t **link, int main_index, tree_node_t *node)
{
    if (*link == NULL)
        *link = node;
    else if (compare_tree_nodes(node, *link, main_index) <= 0)
        tree_insert(&((*link)->left[main_index]), main_index, node); // ramo esquerdo
    else
        tree_insert(&((*link)->right[main_index]), main_index, node); // ramo direito
}

/**
 * @brief Procura nó na árvore, de uma forma recursiva.
 *
 * @param link       Ponteiro para o nó atual
 * @param main_index Índice da árvore
 * @param node       Ponteiro para o nó a procurar
 * @return           Ponteiro para o nó encontrado na árvore, NULL caso contrário
 */
tree_node_t *find(tree_node_t *link, int main_index, tree_node_t *node)
{
    if (link == NULL || compare_tree_nodes(node, link, main_index) == 0)
        return link;
    else if (compare_tree_nodes(node, link, main_index) < 0)
        return find(link->left[main_index], main_index, node); // ramo esquerdo
    else
        return find(link->right[main_index], main_index, node); // ramo direito
}

/**
 * @brief Determina a maior profundidade da árvore, de uma forma recursiva.
 *
 * @param link       Ponteiro para o nó atual
 * @param main_index Índice da árvore
 * @return           Valor da maior profundidade da árvore
 */
int tree_depth(tree_node_t *link, int main_index)
{
    if (link == NULL)
        return 0;

    int left_depth = tree_depth(link->left[main_index], main_index);   // ramo esquerdo
    int right_depth = tree_depth(link->right[main_index], main_index); // ramo direito

    return (left_depth > right_depth) ? left_depth + 1 : right_depth + 1;
}

/**
 * @brief Lista dados dos nós da árvore que passam no filtro especificado, de uma forma recursiva.
 *
 * @param link       Ponteiro para o nó atual
 * @param main_index Índice da árvore
 * @param count      Ponteiro para o número de nós listados
 * @param filter     Ponteiro para o filtro
 */
void list(tree_node_t *link, int main_index, int *count, regex_t *filter)
{
    if (link == NULL)
        return;

    list(link->left[main_index], main_index, count, filter);
    if (filter_tree_node(link, main_index, filter))
        visit(link, count);
    list(link->right[main_index], main_index, count, filter);
}

/* -------------------------------- Fluxo de Execução Principal --------------------------------- */

int main(int argc, char **argv)
{
    double dt; // intervalo de tempo

    /*
     * processar os argumentos da linha de comandos
     */
    if (argc < 3)
    {
        fprintf(stderr, USAGE, argv[0]);
        return EXIT_FAILURE;
    }

    /* número de estudante */
    int student_number = atoi(argv[1]);
    if (student_number < 1 || student_number >= 1000000)
    {
        fprintf(stderr, "Erro: número de estudante inválido (%d) - deve ser um inteiro no intervalo [1,1000000{\n", student_number);
        return 1;
    }

    /* número de pessoas */
    int n_persons = atoi(argv[2]);
    if (n_persons < 3 || n_persons > 10000000)
    {
        fprintf(stderr, "Erro: número de pessoas inválido (%d) - deve ser um inteiro no intervalo [3,10000000]\n", n_persons);
        return 1;
    }

    /* opções */
    regex_t filter;
    regcomp(&filter, ".*", REGEX_FLAGS); // filtro por defeito
    int list_index = 0;                  // índice por defeito
    int c;
    while ((c = getopt_long(argc, argv, ":f:l", long_options, NULL)) != -1)
    {
        switch (c)
        {
        case 'f': // --filter [str]
            if (regcomp(&filter, optarg, REGEX_FLAGS) != 0)
            {
                fprintf(stderr, "Erro (opção f/filter): filtro inválido\n");
                return EXIT_FAILURE;
            }
            break;
        case 'l': // --list [index]
            if (optarg != NULL && (list_index = atoi(optarg)) < 0)
            {
                fprintf(stderr, "Erro (opção l/list): índice inválido (%d) - deve ser um número inteiro não negativo\n", list_index);
                return EXIT_FAILURE;
            }
            list_index = (list_index >= N_MAIN_INDEXES) ? N_MAIN_INDEXES - 1 : list_index;
            break;
        case ':':
            fprintf(stderr, "Erro: uma opção fornecida necessita de argumento\n");
            return EXIT_FAILURE;
        case '?':
            fprintf(stderr, "Erro: foi fornecida uma opção desconhecida\n");
            return EXIT_FAILURE;
        }
    }

    /*
     * gerar todos os dados
     */
    tree_node_t *persons = (tree_node_t *)calloc((size_t)n_persons, sizeof(tree_node_t));
    if (persons == NULL)
    {
        fprintf(stderr, "Erro: memória insuficiente para gerar todos os dados\n");
        return EXIT_FAILURE;
    }

    aed_srandom(student_number);
    for (int i = 0; i < n_persons; i++)
    {
        random_name(&(persons[i].name[0]));
        random_zip_code(&(persons[i].zip_code[0]));
        random_telephone_number(&(persons[i].telephone_number[0]));
        random_social_security_number(&(persons[i].social_security_number[0]));
        for (int j = 0; j < N_MAIN_INDEXES; j++)
            persons[i].left[j] = persons[i].right[j] = NULL; // garantir que os ponteiros são NULL inicialmente
    }

    /*
     * criar as árvores binárias ordenadas
     */
    dt = cpu_time();

    tree_node_t *roots[N_MAIN_INDEXES]; // N índices, N raízes
    for (int main_index = 0; main_index < N_MAIN_INDEXES; main_index++)
        roots[main_index] = NULL;
    for (int i = 0; i < n_persons; i++)
        for (int main_index = 0; main_index < N_MAIN_INDEXES; main_index++)
            tree_insert(&(roots[main_index]), main_index, &(persons[i])); // place your code here to insert &(persons[i]) in the tree with number main_index

    dt = cpu_time() - dt;
    printf("Tree creation time (%d persons): %.3es\n", n_persons, dt);

    /*
     * procurar nó na árvore
     */
    for (int main_index = 0; main_index < N_MAIN_INDEXES; main_index++)
    {
        dt = cpu_time();

        for (int i = 0; i < n_persons; i++)
        {
            tree_node_t n = persons[i]; // copiar os dados do nó
            if (find(roots[main_index], main_index, &n) != &(persons[i]))
            {
                fprintf(stderr, "Erro: pessoa %d não foi encontrada com o índice %d\n", i, main_index);
                return 1;
            }
        }

        dt = cpu_time() - dt;
        printf("Tree search time (%d persons, index %d): %.3es\n", n_persons, main_index, dt);
    }

    /*
     * determinar a maior profundidade da árvore
     */
    for (int main_index = 0; main_index < N_MAIN_INDEXES; main_index++)
    {
        dt = cpu_time();

        int depth = tree_depth(roots[main_index], main_index);

        dt = cpu_time() - dt;
        printf("Tree depth for index %d: %d (done in %.3es)\n", main_index, depth, dt);
    }

    /*
     * listar os nós da árvore, conforme o índice e o filtro forncidos
     */
    int count = 0;
    printf("List of persons:\n");
    list(roots[list_index], list_index, &count, &filter);

    /*
     * limpeza - não esquecer de testar o programa com o valgrind para verificar que não existem memory leaks
     */
    free(persons);
    regfree(&filter);
    return 0;
}
