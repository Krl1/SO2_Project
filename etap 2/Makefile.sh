#! /bin/bash

g++ main.cpp worker.cpp client.cpp basket.cpp bread.cpp butter.cpp jam.cpp shelf.cpp till.cpp trolley.cpp -pthread -std=c++17 -lncurses

if [ $# -eq 0 ]
then
    ./a.out 15 5
elif [ $# -eq 1 ]
then
	if [ $1 -ge 10 ]
	then
	    ./a.out $1 5
	else
    	echo "Podano złą liczbę Pracowników. Podaj 10 lub więcej."
	fi
elif [ $# -eq 2 ]
then
	if [ $1 -ge 10 -a $2 -ge 5 ]
	then
	    ./a.out $1 $2
	else
    	echo "Podano złą liczbę Pracowników lub Klientów. Prawidłowa liczba dla Pracowników to >= 10, dla Klientów to >= 5."
	fi
fi

exit 1
