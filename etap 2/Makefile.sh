#! /bin/bash

# g++ main.cpp worker.cpp client.cpp basket.cpp bread.cpp butter.cpp jam.cpp shelf.cpp till.cpp trolley.cpp global.cpp -pthread -std=c++17 -lncurses
g++ main.cpp -pthread -std=c++17 -lncurses

if [ $# -eq 0 ]
then
    ./a.out 8 5
elif [ $# -eq 1 ]
then
	if [ $1 -ge 5 ]
	then
	    ./a.out $1 5
	else
    	echo "Podano złą liczbę Pracowników. Podaj 5 lub więcej."
	fi
elif [ $# -eq 2 ]
then
	if [ $1 -ge 5 -a $2 -ge 5 ]
	then
	    ./a.out $1 $2
	else
    	echo "Podano złą liczbę Pracowników lub Klientów. Prawidłowa liczba dla Pracowników to >= 5, dla Klientów to >= 8."
	fi
fi

exit 1
