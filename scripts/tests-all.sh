#! /bin/sh

####
#
#D=("DNA.all.fasta" "uniprot_trembl.fasta" "gutenberg-201209-cleaned.txt" "enwiki-20160501.txt")
#
#A=(     32 51 68 126 153 1 1 1 1 1
#        3123223 5446446 10892892 24285785 38178677 50825784 1 1 1 1
#        22495772 41291544 81783088 158966176 231949264 299532352 471915440 1 1 1
#        7240144 17880288 38760576 92521152 151781728 217672592 282802889 351363467 416868852 1)
####

D=("DNA.all.fasta" "input-10000.txt" "proteins-10000.fasta" "reads-10000.fastq")  

A=(	1 2 3 4 5 6 5 4 3 2
	2 4 6 8 10 12 14 16 18 20
	20 40 60 80 100 120 140 160 180 200
	200 400 600 800 1000 1200 1400 1600 1800 2000)
####

host="louza"
dir="/home/louza/dataset/"
workspace="/home/louza/Dropbox/workspace/"

#host="bug"
#dir="/mnt/disk1/louza/dataset/"
#workspace="/home/louza/"

#host="jau"
#dir="/mnt/data/bio/projects/gpt/"
#workspace="/home/gpt/"

c=0
##########################################################################
cd ${workspace}egsa/
mkdir -p tests

for j in 8 #7 6 5 4 3 2 1 ##settings
do
	cp lib/settings/defines_${j}.h lib/defines.h
	make clean
	make DEBUG=0
	
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
			
			rm -Rf ${dir}*.gesa ${dir}*.bin ${dir}*.esa ${dir}partition/* ${dir}tmp/*
		done
		
	done
done
############################################################################
