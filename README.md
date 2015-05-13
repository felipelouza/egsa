#egsa tool

This software is the implementation of egsa \[1\], an external memory algorithm to construct generalized enhanced suffix arrays.


\[1\] 
Louza, F. A., Telles, G. P., & Ciferri, C. D. A. (2013). 
External Memory Generalized Suffix and LCP Arrays Construction. 
In J. Fischer & P. Sanders (Eds.), 
Proc. 24th Annual Symposium on Combinatorial Pattern Matching (2013) 
(Vol. 7922, pp. 201–210). Bad Herrenalb: Springer. doi:10.1007/978-3-642-38905-4_20

article: http://link.springer.com/chapter/10.1007%2F978-3-642-38905-4_20


#run:

We have implemented two different settings to egsa index:

* Collection of large strings;
* Collections of small strings;

To run a test type:

Mode 1 (DNA sequences, each one in a different fasta file):
```sh

make clean

make MODE=1 
make run DIR=example/fasta/ INPUT=all.in K=5 CHECK=1
```

Mode 2 (DNA reads, all in the same fastq file):
```sh
make clean

make MODE=2 
make run DIR=example/fastq/ INPUT=reads-10000.fastq K=1000 CHECK=1
```


#options:

One can choose if egsa outputs the burrows wheeler transform:

```sh

make BWT=1

```

To see a more detailed execution of egsa use:

```sh

make DEBUG=1

```

#references:

egsa uses the algorithm implementation (in folder external) from the following articles:

\[2\] Nong, G., Zhang, S., & Chan, W. H. (2011). Two Efficient Algorithms for Linear Time Suffix Array Construction. IEEE Transactions on Computers, 60(10), 1471–1484. doi:10.1109/TC.2010.188

\[3\] Kasai, T., Lee, G., Arimura, H., Arikawa, S., & Park, K. (2001). Linear-Time Longest-Common-Prefix Computation in Suffix Arrays and Its Applications. In Proc. CPM (pp. 181–192).

\[4\] Fischer, J. (2011). Inducing the LCP-Array. In F. Dehne, J. Iacono, & J.-R. Sack (Eds.), Proc. WADS (pp. 374–385). Springer Berlin Heidelberg.
