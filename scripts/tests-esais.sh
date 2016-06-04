#! /bin/sh


D=("dna-100.fasta" "input-10000.txt" "proteins-10000.fasta" "reads-10000.fastq")  

E=(	1 2 3 4 5 6 7 8 9 10
	10 20 30 40 50 60 70 80 90 100
	100 200 300 400 500 600 700 800 900 1000
	1000 2000 3000 4000 5000 6000 7000 8000 9000 10000)
####

host="louza"
dir="/home/louza/dataset/"
workspace="/home/louza/Dropbox/workspace/"
esais="/home/louza/"

###########################################################################
echo "export HOME=${dir}"
export HOME=${dir}

echo "cd ${esais}eSAIS-DC3-LCP-0.5.4/build_2g/src/"
cd ${esais}eSAIS-DC3-LCP-0.5.4/build_2g/src/

for d in 1 #{1..3}
do
	for i in 0 # {0..9}
	do
		a=$(($d*10+$i))
		test=${D[$d]}
		echo "./esactest ${test} -s ${E[$a]}"
		
		mkdir -p ${workspace}egsa/tests/${test}/esais
		date >> ${workspace}egsa/tests/${test}/esais/${host}.esais.db.$i.txt
		./esactest ${test} -s ${E[$a]} > ${workspace}egsa/tests/${test}/esais/${host}.esais.db.$i.txt
	done

done
#############################################################################
