#!/bin/bash

[[ $# -ne 9 ]] && echo "Sintaxe: $0 nMecMin nMecMax nPersonsMin nPersonsMax nExp list_flag list_index clever_filter_flag filter" && exit 1

# Para cada número de estudante
for nmec in $(seq $1 $2)
do
    
    # Para cada número de pessoas
    nper=$3
    while [[ nper -le $4 ]]
    do

        # Realizar e experiências
        for exp in $(seq 1 $5)
        do
            [[ $6 -eq 1 ]] && ./multi_ordered_tree $nmec $nper --list $7 --filter "$9" > "./results_list/data$nmec-$nper-$exp-$8.txt"
            [[ $6 -eq 0 ]] && ./multi_ordered_tree $nmec $nper > "./results/data$nmec-$nper-$exp.txt"
        done

        nper=$((nper<<1))

    done

done