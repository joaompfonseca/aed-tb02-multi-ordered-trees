#!/bin/bash

[[ $# -ne 1 ]] && echo "Sintaxe: $0 [nMec]" && exit 1

# Para cada número de pessoas
i=4
while [[ $i -le 10000000 ]]
do
    # Realizar 10 experiências
    for e in {1..10}
    do
    ./multi_ordered_tree $1 $i > "./results/data$1-$i-$e.txt" # data[n_mec]-[n_persons]-[n_exp].txt
    done

    ((i<<1))
done