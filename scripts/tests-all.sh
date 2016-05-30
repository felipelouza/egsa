#! /bin/sh

##dna
##uniprot
##gutengerg
##wikipedia

A=(     24 49 68 92 153 1 1 1 1 1
        3123223 5446446 10892892 24285785 38178677 50825784 1 1 1 1
        22495772 41291544 81783088 158966176 231949264 299532352 471915440 1 1 1
        7240144 17880288 38760576 92521152 151781728 217672592 282802889 351363467 416868852 1)

E=(     1024 2048 4096 8192 10752 1 1 1 1 1
        1160 2153 4286 8273 12328 16147 1 1 1 1
        1115 2210 4472 8945 12288 16384 20480 1 1 1
        1024 2048 4096 8192 12288 16384 20480 24576 29158 1)


D=("DNA.all.fasta" "uniprot_trembl.fasta" "gutenberg-201209-cleaned.txt" "enwiki-20160501.txt")

#host="bug"
#dir="/mnt/disk1/louza/dataset/sac-corpus/"
#workspace="/home/louza/"

host="jau"
dir="/mnt/data/bio/projects/gpt/"
workspace="/home/gpt/"

c=0
###########################################################################
#cd ${workspace}egsa/
#mkdir -p tests
#
#for j in 8 #7 6 5 4 3 2 1 ##settings
#do
#       cp lib/settings/defines_${j}.h lib/defines.h
#       make clean
#       make DEBUG=1
#       echo "settings '${j}':"
#
#       for i in {0..8}
#       do
#
#               for d in 3 #{1..3}
#               do
#                       cd ${workspace}egsa/
#
#                       a=$(($d*10+$i))
#                       test=${D[$d]}
#
#                       echo ${test}:${A[$a]}
#                       mkdir -p tests/${test}/${j}
#                       date >> tests/${test}/${j}/${host}.db.$i.txt
#
#                       echo " " >> tests/${test}/${j}/${host}.db.$i.txt
#                       make run DIR=${dir} INPUT=${test} K=${A[$a]} CHECK=$c >> tests/${test}/${j}/${host}.db.$i.txt
#                       
#                       rm -f ${dir}*.gesa
#                       rm -f ${dir}*.bin
#                       rm -Rf ${dir}*.esa
#                       rm -Rf ${dir}partition/*
#                       rm -Rf ${dir}tmp/*
#               done
#               
#       done
#done
#
############################################################################
export HOME=${dir}

cd ${workspace}egsa/
gcc external/convert_fasta.c -o convert_fasta

for i in {0..5}
do

        for d in 1 #{1..3}
        do

                a=$(($d*10+$i))
                test=${D[$d]}
                echo "./esactest ${test}"

####
                cd ${workspace}egsa/
                ./convert_fasta ${dir} uniprot_trembl.fasta ${A[$a]} > ${dir}sac-corpus/uniprot_trembl.${A[$a]}.fasta
                cd ${workspace}eSAIS-DC3-LCP-0.5.4/build_2g/src/
####
                mkdir -p ${workspace}egsa/tests/${test}/esais
                date >> ${workspace}egsa/tests/${test}/esais/${host}.esais.db.$i.txt
                ./esactest uniprot_trembl.${A[$a]}.fasta  > ${workspace}egsa/tests/${test}/esais/${host}.esais.db.$i.txt

                rm ${dir}sac-corpus/uniprot_trembl.${A[$a]}.fasta
        done

done
        
