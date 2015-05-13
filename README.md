#egsa tool

This software is the implementation of egsa \[1\] (http://link.springer.com/chapter/10.1007%2F978-3-642-38905-4_20), an external memory algorithm to construct generalized enhanced suffix arrays.

Given a collection of K strings, egsa outputs its:

* Generalized suffix array 
* LCP-array 
* Burrows-Wheeler transform.


#run:

We have implemented two different settings to egsa:

* Mode 1: indexing a small collection of large strings (such as DNA sequences);
* Mode 2: indexing a large collections of small strings (such as reads);

--

**Mode 1:**

```sh

make clean

make compile MODE=1 
make run DIR=example/fasta/ INPUT=all.in K=5 BWT=1 CHECK=1
```

Each string is stored in a different (fasta) file, indicated by all.in file.

--

**Mode 2:**

```sh
make clean

make compile MODE=2 
make run DIR=example/fastq/ INPUT=reads-10000.fastq K=1000 BWT=1 MEMLIMIT=10 CHECK=1
```

All strings are stored (concatenated) in the same file, e.g. fastq files.


#options:

One can choose if egsa outputs the burrows wheeler transform:

```sh

make compile BWT=1

```

In mode 2, one can inform to egsa the maximum available internal memory to be used (in MB):

```sh

make run MEMLIMIT=10

```


One can check if the output produced by egsa is correct:

```sh

make run CHECK=1

```

To see a more detailed execution output use:

```sh

make DEBUG=1

```

#external resources:

Currently egsa uses algorithms (located in folder external) from \[2, 3, 4\] to construct the
enhanced suffix arrays in phase 1.


#references:

\[1\] Louza, F. A., Telles, G. P., & Ciferri, C. D. A. (2013). External Memory Generalized Suffix and LCP Arrays Construction. In Proc. CPM  (pp. 201–210).

\[2\] Nong, G., Zhang, S., & Chan, W. H. (2011). Two Efficient Algorithms for Linear Time Suffix Array Construction. IEEE Transactions on Computers, 60(10), 1471–1484. doi:10.1109/TC.2010.188

\[3\] Kasai, T., Lee, G., Arimura, H., Arikawa, S., & Park, K. (2001). Linear-Time Longest-Common-Prefix Computation in Suffix Arrays and Its Applications. In Proc. CPM (pp. 181–192).

\[4\] Fischer, J. (2011). Inducing the LCP-Array. In F. Dehne, J. Iacono, & J.-R. Sack (Eds.), Proc. WADS (pp. 374–385). Springer Berlin Heidelberg.
