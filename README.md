#egsa tool

This software is the implementation of egsa \[1\], an external memory algorithm to construct generalized enhanced suffix arrays.


\[1\] 
Louza, F.A., Telles, G.P., Ciferri, C.D.A.: External memory generalized 
suffix and lcp arrays construction. In: Proc. 24th Annual Symposium on Combinatorial Pattern Matching (2013)

article: http://link.springer.com/chapter/10.1007%2F978-3-642-38905-4_20

talk: http://www.ic.unicamp.br/~ra144643/downloads/talks/cpm2013.pdf


#run:

To run a test type:


Mode 1 (DNA sequences, each one in a different fasta file):
```sh

make clean

make MODE=1 BWT=1 DEBUG=0
make run DIR=example/fasta/ INPUT=all.in K=5 CHECK=1
```

Mode 2 (DNA reads, all in the same fastq file):
```sh
make clean

make MODE=2 BWT=1 DEBUG=0
make run DIR=example/fastq/ INPUT=reads-10000.fastq K=1000 CHECK=1
```

