#! /bin/sh

#uniprot
#Ap=(0 100000 200000 400000 800000 1600000 3200000 6400000 12800000 25600000 50825784)
Ap=(0 100 200 4000 8000 16000 3200 6400 12800 25600 50825)
#29266939

host="bug"
dir="/mnt/disk1/louza/dataset/"

#########################################################################

mkdir -p tests

##proteins

make clean
make

test="protein"

for j in {1..8} ##settings
	do
		echo "settings '${j}':"
	
		for i in {1..10}
		do

			echo "('${test}')dataset:  '${Ap[$i]}' strings"

			mkdir -p tests/protein/
			date >> tests/protein/uniprot.${host}."${Ap[$i]}".txt

			cp lib/settings/defines_${j}.h lib/defines.h
			
			echo " " >> tests/protein/uniprot.${host}."${Ap[$i]}".txt
			make run DIR=${dir} INPUT=uniprot_trembl.fasta K=${Ap[$i]} CHECK=1 >> tests/protein/uniprot.${host}."${Ap[$i]}".txt
		done	
done


