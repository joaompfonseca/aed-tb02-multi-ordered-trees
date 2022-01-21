#!/bin/bash

[[ $# -ne 2 ]] && echo "Sintaxe: $0 [nMec] [nExp]" && exit 1
[[ $1 -lt 1 || $1 -ge 1000000 ]] && echo "Número de estudante inválido!" && exit 1
[[ $2 -le 0 ]] && echo "Número de experiências inválido!" && exit 1

# Para cada número de pessoas
i=4
while [[ $i -le 10000000 ]]
do
    # Realizar e experiências
    for e in $(seq 1 $2)
    do
        ./multi_ordered_tree $1 $i > "./results/data$1-$i-$e.txt" # data[n_mec]-[n_persons]-[n_exp].txt
    done

    i=$((i<<1))
done