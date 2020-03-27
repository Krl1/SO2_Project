#! /bin/bash

g++ main.cpp -pthread -std=c++17 -lncurses

if [ $# -eq 0 ]
then
    ./a.out 5
elif [ $1 -ge 5 ]
then
    ./a.out $1
else
    echo "Podano złą liczbę filozofów. Podaj 5 lub więcej."
fi

exit 1
