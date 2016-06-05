#! /bin/sh

D=("dna-100.fasta" "input-100.txt" "proteins-10000.fasta" "reads-10000.fastq")  

A=(	1 2 3 4 5 6 7 8 9 10
	10 20 30 40 50 60 70 80 90 100
	100 200 300 400 500 600 700 800 900 1000
	1000 2000 3000 4000 5000 6000 7000 8000 9000 10000)
####

host="louza"
dir="dataset/"

c=0
##########################################################################
cd ../
mkdir -p tests

for j in 8 #7 6 5 4 3 2 1 ##settings
do
	cp lib/settings/defines_${j}.h lib/defines.h
	make clean
	make DEBUG=0 BWT=1
	
	echo "settings '${j}':"

	for d in {1..3}
	do
	
		for i in {0..9}
		do
			a=$(($d*10+$i))
			test=${D[$d]}
			
			echo ${test}:${A[$a]}
			mkdir -p tests/${test}/${j}
			date >> tests/${test}/${j}/${host}.db.$i.txt
			
			echo " " >> tests/${test}/${j}/${host}.db.$i.txt
			make run DIR=${dir} INPUT=${test} K=${A[$a]} CHECK=$c >> tests/${test}/${j}/${host}.db.$i.txt
			
			rm -Rf ${dir}*.gesa ${dir}*.bwt ${dir}*.bin ${dir}*.esa ${dir}partition/* ${dir}tmp/*
		done
		
	done
done
############################################################################
