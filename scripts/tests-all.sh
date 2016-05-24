#! /bin/sh

##dna
##uniprot
##gutengerg
##wikipedia

A=(	2 6 8 10 12 24 49 68 92 153
	100000 200000 400000 800000 1600000 3200000 6400000 12800000 25600000 50825784
	921710 1843420 3686839 7373679 14747358 29494715 58989430 117978860 235957720 471915440
 	814197 1628394 3256788 6513576 13027152 26054303 52108607 104217213 208434426 416868852)

#A=(	1 2 3 4 5 6 5 4 3 2
#	2 4 6 8 10 12 14 16 18 20
#	20 40 60 80 100 120 140 160 180 200
#	200 400 600 800 1000 1200 1400 1600 1800 2000)

D=("DNA.all.fasta" "uniprot_trembl.fasta" "gutenberg-201209-cleaned.txt" "enwiki-20160501.txt")
#D=("DNA.all.fasta" "input-10000.txt" "proteins-10000.fasta" "reads-10000.fastq")  

#host="bug"
#dir="/mnt/disk1/louza/dataset/"
#workspace="/home/louza/"

host="jau"
dir="/mnt/data/bio/projects/gpt/"
workspace="/home/gpt/"

#host="louza"
#dir="/home/louza/dataset/egsa/"
#workspace="/home/louza/"

c=0
##########################################################################
#cd ${workspace}"egsa/"
#make clean
#make esais_compile
#
#rm ${dir}/partition/*
#mkdir ${dir}/sac-corpus/
#
#for i in {0..9}
#do
#	for d in {0..3}
#	do
#		a=$(($d*10+$i))
#		test=${D[$d]}
#
#		echo ${test}:${A[$a]}
#
#	        make esais_input DIR=${dir} INPUT=${test} K=${A[$a]} 
#	done
#	
#done
#mv ${dir}/partition/* ${dir}/sac-corpus/
##########################################################################

cd ${workspace}egsa/
mkdir -p tests
export HOME=/mnt/data/bio/projects/gpt/

for j in 8 #7 6 5 4 3 2 1 ##settings
do

	cd ${workspace}egsa/
	cp lib/settings/defines_${j}.h lib/defines.h
	make clean
	make
	echo "settings '${j}':"


	for i in {0..9}
	do

		for d in {1..3}
		do
			cd ${workspace}egsa/

			a=$(($d*10+$i))
			test=${D[$d]}

			echo ${test}:${A[$a]}

		        mkdir -p tests/${test}/${j}
		        date >> tests/${test}/${j}/${host}.db.$i.txt

		        echo " " >> tests/${test}/${j}/${host}.db.$i.txt
		        make run DIR=${dir} INPUT=${test} K=${A[$a]} CHECK=$c >> tests/${test}/${j}/${host}.db.$i.txt
		        
			rm -f ${dir}*.gesa
			rm -f ${dir}*.bin
			rm -Rf ${dir}*.esa
			rm -Rf ${dir}partition/*
			rm -Rf ${dir}tmp/*
#####
			echo "./esactest ${test}.${A[$a]}.0.bin"
	       		cd ${workspace}eSAIS-DC3-LCP-0.5.4/build_2g/src/

		        mkdir -p tests/${test}/esais
       	        	date >> ${workspace}egsa/tests/${test}/esais/${host}.esais.db.$i.txt
			./esactest ${test}.${A[$a]}.0.bin > ${workspace}egsa/tests/${test}/esais/${host}.esais.db.$i.txt

#####
		done
		
	done
done
