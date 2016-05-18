#egsa tool

This software is the implementation of egsa \[1\] (http://link.springer.com/chapter/10.1007%2F978-3-642-38905-4_20), an external memory algorithm to construct generalized enhanced suffix arrays.

Given a collection of K strings, egsa outputs the:

* Generalized suffix array 
* LCP-array 
* Burrows-Wheeler transform.

##install:

```sh
git clone https://github.com/felipelouza/egsa.git
cd egsa
```

**Compile:**

```sh
make clean
make compile 
```

##run:

Given a collection of K strings concatenated in a single file INPUT in the directory DIR.

_fasta_
```sh
make run DIR=dataset/fasta/ INPUT=proteins-100.fasta K=100
```

_fastq_
```sh
make run DIR=dataset/fastq/ INPUT=reads-100.fastq K=100
```

_txt_
```sh
make run DIR=dataset/txt/	INPUT=input-100.txt K=100
```
* strings are separated per '\0' (new line)

--
**Examples:**

One could find examples in _dataset_ folder, at least one for each setting mode.


##output:

The generalized enhanced suffix array is stored in the same directory DIR in a file called:

```sh
$(INPUT).K.gesa
```

Temporary files are stored in folders 

```sh
$(DIR)/partition/
$(DIR)/tmp/
```

##options:

Output the burrows wheeler transform:

```sh

make compile BWT=1

```

One can inform the maximum available internal memory to be used (in MB):

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
* gsaca-k: generalized suffix array construction algorithm \[3\]
* lcp-PHI: LCP-array construction algorithm given the suffix array \[4\]

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

\[1\] Louza, F. A., Telles, G. P., Ciferri, C. D. A. (2013). External Memory Generalized Suffix and LCP Arrays Construction. In Proc. CPM  (pp. 201-210).

\[2\] Fischer, J. (2011). Inducing the LCP-Array. In F. Dehne, J. Iacono, & J.-R. Sack (Eds.), Proc. WADS (pp. 374-385).

\[3\] Louza, F. A., Gog, S., Telles, G. P. (2016). Induced Suffix Sorting for String Collections. In Proc. DCC (pp. 43-52), https://github.com/felipelouza/gsa-is 

\[4\] Kärkkäinen, J., Manzini, G., & Puglisi, S. J. (2009). Permuted Longest-Common-Prefix Array. In G. Kucherov & E. Ukkonen (Eds.), Proc. CPM (Vol. 5577, pp. 181–192).

##thanks:

Thanks to Guilherme P. Telles, Steve Hoffmann, Fabio Garofalo and Giovanna Rosone by helpful suggestions and debugging.

