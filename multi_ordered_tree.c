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
#include <math.h>
#include "AED_2021_A02.h"

/* ------------------------------------------- Macros ------------------------------------------- */

/**
 * @brief Utilização correta do programa
 */
#define USAGE "Sintaxe: %s n_mec n_persons [options ...]\n"                        \
              "Opções válidas:\n"                                               \
              "    -f [regex], --filter [regex]    # filtra o conteúdo listado\n" \
              "    -l [N?], --list [N?]            # lista o conteúdo da árvore, ordenado pelos dados correspondentes ao índice N (por defeito, N=0)\n"

/**
 * @brief Flags da expressão regular (do filtro)
 */
#define REGEX_FLAGS (REG_NOSUB | REG_ICASE | REG_EXTENDED)

/**
 * @brief Configuração do programa
 */
#define CONFIG "Configuração do programa:\n" \
               "    n_mec ....... %d\n"        \
               "    n_persons ... %d\n"        \
               "    filter ...... \"%s\"\n"    \
               "    list_index .. %d\n"

/* ------------------------------------ Estruturas de Dados ------------------------------------- */

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

/**
 * @brief Opções disponíveis no programa.
 */
static struct option long_options[] = {
    {"filter", required_argument, NULL, 'f'},
    {"list", required_argument, NULL, 'l'},
    {NULL, 0, NULL, 0}};

/**
 * @brief Informações sobre a listagem
 */
typedef struct list_s
{
    int flag;
    int index;
} list_t;

/**
 * @brief Informações sobre o filtro
 */
typedef struct filter_s
{
    int flag;
    char *str;
    regex_t regex;
} filter_t;

/**
 * @brief Contagem de pessoas e de nós percorridos
 */
typedef struct pn_count_s
{
    int persons;
    int nodes;
} pn_count_t;

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
 * @brief Compara o dado no índice da árvore a uma string.
 *
 * @param node       Ponteiro para o nó
 * @param main_index Índice da árvore
 * @param str        Ponteiro para a string
 * @return           Resultado da comparação
 */
int compare_tree_data(tree_node_t *node, int main_index, char *str)
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

    return strncasecmp(str, data, strlen(str));
}

/**
 * @brief Filtra o nó da árvore.
 *
 * @param node       Ponteiro para o nó
 * @param main_index Índice da árvore
 * @param filter     Ponteiro para as informações sobre o filtro
 * @return           Resultado da filtragem
 */
int filter_tree_node(tree_node_t *node, int main_index, filter_t *filter)
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

    return regexec(&(filter->regex), data, 0, NULL, 0) == 0;
}

/**
 * @brief Imprime os dados do nó.
 *
 * @param node  Ponteiro para o nó
 * @param count Ponteiro para o número de pessoas listadas
 */
void visit(tree_node_t *node, int *count)
{
    printf("%-8d\t%-31s\t%-63s\t%-17s\t%-23s\n", ++(*count), node->name, node->zip_code, node->telephone_number, node->social_security_number);
}

/**
 * @brief Insere nó na árvore, de uma forma recursiva.
 *
 * @param link       Ponteiro para o ponteiro que aponta para o nó atual
 * @param main_index Índice da árvore
 * @param node       Ponteiro para o nó a inserir
 * @param n_mec      Número de estudante utilizado (utilizado apenas no caso de haver nós iguais)
 */
void tree_insert(tree_node_t **link, int main_index, tree_node_t *node, int n_mec)
{
    if (*link == NULL)
        *link = node;
    else if (compare_tree_nodes(node, *link, main_index) < 0)
        tree_insert(&((*link)->left[main_index]), main_index, node, n_mec); // ramo esquerdo
    else if (compare_tree_nodes(node, *link, main_index) > 0)
        tree_insert(&((*link)->right[main_index]), main_index, node, n_mec); // ramo direito
    else
    {
        fprintf(stderr, "Erro: foram gerados nós iguais - escolha um número de estudante diferente de %d\n", n_mec);
        exit(EXIT_FAILURE);
    }
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
        return -1;

    int left_depth = tree_depth(link->left[main_index], main_index);   // ramo esquerdo
    int right_depth = tree_depth(link->right[main_index], main_index); // ramo direito

    return (left_depth > right_depth) ? left_depth + 1 : right_depth + 1;
}

/**
 * @brief Determina o número de nós em cada nível da árvore, de uma forma recursiva.
 *
 * @param link       Ponteiro para o nó atual
 * @param main_index Índice da árvore
 * @param counts     Ponteiro para o vetor das contagens
 * @param level      Nível atual na árvore
 */
void count_nodes_in_levels(tree_node_t *link, int main_index, int *counts, int level)
{
    if (link == NULL)
        return;

    counts[level]++;
    count_nodes_in_levels(link->left[main_index], main_index, counts, level + 1);
    count_nodes_in_levels(link->right[main_index], main_index, counts, level + 1);
}

/**
 * @brief Lista dados dos nós da árvore que passam no filtro especificado, de uma forma recursiva.
 *
 * @param link       Ponteiro para o nó atual
 * @param main_index Índice da árvore
 * @param count      Ponteiro para as contagens do número de pessoas listadas e número de nós percorridos
 * @param filter     Ponteiro para as informações sobre o filtro
 */
void list_nodes(tree_node_t *link, int main_index, pn_count_t *count, filter_t *filter)
{
    if (link == NULL)
        return;

    (count->nodes)++;

    if (filter->str[0] != '^' || compare_tree_data(link, main_index, filter->str + 1) <= 0)
        list_nodes(link->left[main_index], main_index, count, filter); // ramo esquerdo

    if (filter_tree_node(link, main_index, filter))
        visit(link, &(count->persons)); // imprimir os dados do nó

    if (filter->str[0] != '^' || compare_tree_data(link, main_index, filter->str + 1) >= 0)
        list_nodes(link->right[main_index], main_index, count, filter); // ramo direito
}

/* -------------------------------- Fluxo de Execução Principal --------------------------------- */

int main(int argc, char **argv)
{
    double ti, tf; // intervalo de tempo

    /*
     * processar os argumentos da linha de comandos
     */
    if (argc < 3)
    {
        fprintf(stderr, USAGE, argv[0]);
        return EXIT_FAILURE;
    }

    /* número de estudante */
    int n_mec = atoi(argv[1]);
    if (n_mec < 1 || n_mec >= 1000000)
    {
        fprintf(stderr, "Erro: número de estudante inválido (%d) - deve ser um inteiro no intervalo [1,1000000{\n", n_mec);
        return EXIT_FAILURE;
    }

    /* número de pessoas */
    int n_persons = atoi(argv[2]);
    if (n_persons < 3 || n_persons > 10000000)
    {
        fprintf(stderr, "Erro: número de pessoas inválido (%d) - deve ser um inteiro no intervalo [3,10000000]\n", n_persons);
        return EXIT_FAILURE;
    }

    /* filtro (por defeito) */
    filter_t *filter = (filter_t *)malloc(1 * sizeof(filter_t));
    filter->flag = 0;
    filter->str = (char *)&(".*");
    regcomp(&(filter->regex), filter->str, REGEX_FLAGS);

    /* listar (por defeito) */
    list_t *list = (list_t *)malloc(1 * sizeof(list_t));
    list->flag = 0;
    list->index = 0;

    /* opções */
    int c;
    while ((c = getopt_long(argc, argv, ":f:l:", long_options, NULL)) != -1)
    {
        switch (c)
        {
        case 'f': // --filter [str]
            filter->str = optarg;
            if (regcomp(&(filter->regex), filter->str, REGEX_FLAGS) != 0)
            {
                fprintf(stderr, "Erro (opção f/filter): filtro inválido (%s)\n", filter->str);
                return EXIT_FAILURE;
            }
            filter->flag = 1;
            break;
        case 'l': // --list [index]
            list->index = atoi(optarg);
            if (list->index < 0 || list->index > N_MAIN_INDEXES - 1)
            {
                fprintf(stderr, "Erro (opção l/list): índice inválido (%d) - deve ser um inteiro no intervalo [0,%d]\n", list->index, N_MAIN_INDEXES - 1);
                return EXIT_FAILURE;
            }
            if (strncmp(optarg, "-l", 2) == 0 ||
                strncmp(optarg, "--list", 6) == 0 ||
                strncmp(optarg, "-f", 2) == 0 ||
                strncmp(optarg, "--filter", 8) == 0)
                optind--;
            list->flag = 1;
            break;
        case ':':
            switch (optopt)
            {
            case 'l':
                list->index = 0;
                list->flag = 1;
                break;
            default:
                fprintf(stderr, "Erro: uma opção fornecida necessita de argumento\n");
                return EXIT_FAILURE;
            }
            break;
        case '?':
            fprintf(stderr, "Erro: foi fornecida uma opção desconhecida\n");
            return EXIT_FAILURE;
        }
    }
    /* foi fornecido um filtro sem ter sido pedida uma listagem */
    if (filter->flag && !list->flag)
    {
        fprintf(stderr, "Erro: opção f/filter foi fornecida sem a opção l/list\n");
        return EXIT_FAILURE;
    }

    /* configuração do programa */
    fprintf(stderr, CONFIG, n_mec, n_persons, filter->str, list->index);

    /*
     * gerar todos os dados
     */
    tree_node_t *persons = (tree_node_t *)calloc((size_t)n_persons, sizeof(tree_node_t));
    if (persons == NULL)
    {
        fprintf(stderr, "Erro: memória insuficiente para gerar todos os dados\n");
        return EXIT_FAILURE;
    }

    aed_srandom(n_mec);
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
     * criar as árvores ordenadas
     */
    tree_node_t *roots[N_MAIN_INDEXES]; // N índices, N raízes
    for (int main_index = 0; main_index < N_MAIN_INDEXES; main_index++)
        roots[main_index] = NULL;

    /* imprimir cabeçalho */
    printf("Criação das árvores ordenadas:\n");
    printf("%-9s\t%-5s\t%-12s\n", "n_persons", "index", "time");

    for (int main_index = 0; main_index < N_MAIN_INDEXES; main_index++)
    {
        /* inserir todos os nós na árvore */
        ti = cpu_time();
        for (int i = 0; i < n_persons; i++)
            tree_insert(&(roots[main_index]), main_index, &(persons[i]), n_mec);
        tf = cpu_time();

        /* imprimir número de pessoas, índice da árvore e tempo de execução*/
        printf("%-9d\t%-5d\t%-12e\n", n_persons, main_index, tf - ti);
    }
    printf("\n");

    /*
     * procurar todos os nós nas árvores
     */

    /* imprimir cabeçalho */
    printf("Procura de nós nas árvores:\n");
    printf("%-9s\t%-5s\t%-12s\n", "n_persons", "index", "time");

    for (int main_index = 0; main_index < N_MAIN_INDEXES; main_index++)
    {
        /* procurar todos os nós na árvore */
        ti = cpu_time();
        for (int i = 0; i < n_persons; i++)
        {
            tree_node_t n = persons[i]; // copiar os dados do nó
            if (find(roots[main_index], main_index, &n) != &(persons[i]))
            {
                fprintf(stderr, "Erro: pessoa %d não foi encontrada na árvore com o índice %d\n", i, main_index);
                return EXIT_FAILURE;
            }
        }
        tf = cpu_time();

        /* imprimir número de pessoas, índice da árvore e tempo de execução */
        printf("%-9d\t%-5d\t%-12e\n", n_persons, main_index, tf - ti);
    }
    printf("\n");

    /*
     * determinar a maior profundidade das árvores
     */

    /* imprimir cabeçalho */
    printf("Maior profundidade das árvores:\n");
    printf("%-9s\t%-5s\t%-12s\t%-5s\n", "n_persons", "index", "time", "depth");

    int depths[N_MAIN_INDEXES];
    for (int main_index = 0; main_index < N_MAIN_INDEXES; main_index++)
    {
        /* determinar a maior profundidade da árvore */
        ti = cpu_time();
        depths[main_index] = tree_depth(roots[main_index], main_index);
        tf = cpu_time();

        /* imprimir número de pessoas, índice da árvore, tempo de execução e maior profundidade da árvore*/
        printf("%-9d\t%-5d\t%-12e\t%-5d\n", n_persons, main_index, tf - ti, depths[main_index]);
    }
    printf("\n");

    /*
     * determinar o número de nós em cada nível das árvores
     */

    /* profundidade máxima de entre todas as árvores */
    int max_depth = 0;
    for (int main_index = 0; main_index < N_MAIN_INDEXES; main_index++)
        max_depth = (depths[main_index] > max_depth) ? depths[main_index] : max_depth;

    /* imprimir cabeçalho */
    printf("Número de nós em cada nível:\n");
    printf("%-9s\t%-5s\t%-12s", "n_persons", "index", "time");
    for (int i = 0; i <= max_depth; i++)
        printf("\t%-3s%-3d", "lvl", i);
    printf("\n");

    for (int main_index = 0; main_index < N_MAIN_INDEXES; main_index++)
    {
        int n_levels = depths[main_index] + 1;
        int *counts = (int *)calloc(n_levels, sizeof(int));

        /* determinar o número de nós em cada nível da árvore */
        ti = cpu_time();
        count_nodes_in_levels(roots[main_index], main_index, counts, 0);
        tf = cpu_time();

        /* imprimir número de pessoas, índice da árvore, tempo de execução e número de nós por nível*/
        printf("%-9d\t%-5d\t%-12e", n_persons, main_index, tf - ti);
        int lvl;
        for (lvl = 0; lvl < n_levels; lvl++)
            printf("\t%-6d", counts[lvl]);
        for (; lvl <= max_depth; lvl++)
            printf("\t%-6f", NAN);
        printf("\n");

        free(counts);
    }
    printf("\n");

    /*
     * listar os nós da árvore, conforme o índice e o filtro forncidos
     */
    if (list->flag)
    {
        pn_count_t count = {.persons = 0, .nodes = 0};

        /* imprimir cabeçalho */
        printf("Lista de pessoas (índice %d, filtro \"%s\"):\n", list->index, filter->str);
        printf("%-8s\t%-31s\t%-63s\t%-20s\t%-26s\n", "#", "name (0)", "zip_code (1)", "telephone number (2)", "social security number (3)");

        list_nodes(roots[list->index], list->index, &count, filter);

        printf("Foram listadas %d/%d pessoas\n", count.persons, n_persons);
        printf("Foram percorridos %d/%d nós da árvore\n", count.nodes, n_persons);
        printf("\n");
    }

    /*
     * limpeza - não esquecer de testar o programa com o valgrind para verificar que não existem memory leaks
     */
    free(persons);
    free(list);
    regfree(&(filter->regex));
    free(filter);

    return 0;
}
