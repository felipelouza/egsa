#egsa tool

This software is the implementation of egsa \[1\] (http://link.springer.com/chapter/10.1007%2F978-3-642-38905-4_20), an external memory algorithm to construct generalized enhanced suffix arrays.

Given a collection of K strings, egsa outputs its:

* Generalized suffix array 
* LCP-array 
* Burrows-Wheeler transform.

--
**Settings:**

We have implemented two settings to egsa:

* Mode 1: indexing a small collection of large strings (DNA sequences);
* Mode 2: indexing a large collections of small strings:
  - DNA sequencing reads; or
  - protein sequences;

--
**Examples:**

One could find examples in _example_ folder, at least one for each setting mode.

##install:


```sh
git clone https://github.com/felipelouza/egsa.git
cd egsa
```

**Compile:**


```sh
make clean
make compile MODE=1  
```

or 

```sh
make clean
make compile MODE=2
```

\*In mode 2, one can add _PROTEIN=1_ if the input is a collection of protein sequences.

##run:

**Mode 1:**

Given a collection of K strings (DNA sequences), each one stored in a different (fasta) file, indicated by the file INPUT in directory DIR.

_fasta_ (genomes):

```sh
make run DIR=example/fasta/ INPUT=all.in K=5 
```

--

**Mode 2:** 

Given a collection of K strings (reads or proteins), all strings stored (concatenated) in the same (fastq or fasta) file INPUT in directory DIR.

_fastq_ (reads):

```sh
make run DIR=example/fastq/ INPUT=reads-10000.fastq K=1000
```

_fasta_ (proteins):

```sh
make run DIR=example/proteins/ INPUT=proteins-10000.fasta K=1000
```


##output:

The generalized enhanced suffix array is stored in the same directory DIR in a file called:

```sh
$(INPUT).K.gesa
```

Each input file is converted and stored in a binary file called:

```sh
$(INPUT).K.bin
```

##options:

One can choose the following options:

Output the burrows wheeler transform:

```sh

make compile BWT=1

```

In mode 2, one can inform to egsa the maximum available internal memory to be used (in MB):

```sh

make run MEMLIMIT=10

```

**debug:**

To see a more detailed execution output use:

```sh

make compile DEBUG=1

```


One can check if the output produced by egsa is correct:

```sh

make run CHECK=1

```

##external resources:

We have included the source codes of the following algorithms: 

* sais-lite-LCP: suffix array and LCP-array construction algorithm \[2\]
* sa-is: suffix array construction algorithm \[3\]
* KLAAP: LCP-array construction algorithm given the suffix array \[4\]

##citation:

Please, if you use egsa tool in an academic setting cite the following paper:

	@inproceedings{DBLP:conf/cpm/LouzaTC13,
	  author = {Louza, Felipe A. and Telles, Guilherme P. and Ciferri, Cristina D. A.},
	  title = {External Memory Generalized Suffix and {LCP} Arrays Construction},
   	  year = {2013},
	  isbn = {978-3-642-38904-7},
	  booktitle = {Combinatorial Pattern Matching, 24th Annual Symposium, {CPM} 2013,
               Bad Herrenalb, Germany, June 17-19, 2013. Proceedings},
	  pages = {201--210},
	  year = {2013},
	  series = {Lecture Notes in Computer Science},
	  volume = {7922},
	  publisher = {Springer}
	}

##references:

\[1\] Louza, F. A., Telles, G. P., & Ciferri, C. D. A. (2013). External Memory Generalized Suffix and LCP Arrays Construction. In Proc. CPM  (pp. 201–210).

\[2\] Fischer, J. (2011). Inducing the LCP-Array. In F. Dehne, J. Iacono, & J.-R. Sack (Eds.), Proc. WADS (pp. 374–385).

\[3\] Nong, G., Zhang, S., & Chan, W. H. (2011). Two Efficient Algorithms for Linear Time Suffix Array Construction. IEEE Transactions on Computers, 60(10), 1471–1484. doi:10.1109/TC.2010.188

\[4\] Kasai, T., Lee, G., Arimura, H., Arikawa, S., & Park, K. (2001). Linear-Time Longest-Common-Prefix Computation in Suffix Arrays and Its Applications. In Proc. CPM (pp. 181–192).

