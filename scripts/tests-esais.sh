#! /bin/sh

####
#
#D=("DNA.all.fasta" "uniprot_trembl.fasta" "gutenberg-201209-cleaned.txt" "enwiki-20160501.txt")
#
#E=(     1048 2185 4133 8274 10397 1 1 1 1 1
#        1084 2041 4135 8272 12242 15849 19127 1 1 1
#        1115 2210 4472 8945 12288 16384 20480 1 1 1
#        1024 2048 4096 8192 12288 16384 20480 24576 29158 1)
#
####

D=("dna-100.fasta" "input-10000.txt" "proteins-10000.fasta" "reads-10000.fastq")  

E=(	1 2 3 4 5 6 5 4 3 2
	2 4 6 8 10 12 14 16 18 20
	2 4 6 8 10 12 14 16 18 20
	2 4 6 8 10 12 14 16 18 20)

####

host="louza"
dir="/home/louza/dataset/"
workspace="/home/louza/Dropbox/workspace/"
esais="/home/louza/"

#host="bug"
#dir="/mnt/disk1/louza/dataset/"
#workspace="/home/louza/"
#esais="/home/louza/"

#host="jau"
#dir="/mnt/data/bio/projects/gpt/"
#workspace="/home/gpt/"
#esais="/home/gpt/"

###########################################################################
echo "export HOME=${dir}"
export HOME=${dir}

echo "cd ${esais}eSAIS-DC3-LCP-0.5.4/build_2g/src/"
cd ${esais}eSAIS-DC3-LCP-0.5.4/build_2g/src/

for d in 1 #{1..3}
do
	for i in {0..9}
	do
		a=$(($d*10+$i))
		test=${D[$d]}
		echo "./esactest ${test} -s ${E[$a]}mb"
		
		mkdir -p ${workspace}egsa/tests/${test}/esais
		date >> ${workspace}egsa/tests/${test}/esais/${host}.esais.db.$i.txt
		./esactest ${test} -s ${E[$a]}mb #> ${workspace}egsa/tests/${test}/esais/${host}.esais.db.$i.txt
	done

done
#############################################################################
