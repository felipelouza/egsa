# egsa tool

This software is the implementation of egsa \[1,4\] (http://link.springer.com/chapter/10.1007%2F978-3-642-38905-4_20), an external memory algorithm to construct generalized enhanced suffix arrays.

Given a collection of K strings, egsa outputs the:

* Generalized suffix array 
* LCP-array 
* Burrows-Wheeler transform.

## install

```sh
git clone https://github.com/felipelouza/egsa.git
cd egsa
```

**Compile**

```sh
make compile 
```

## run

	Given a collection of K strings concatenated in a single file INPUT in the directory DIR (K=0 gives all strings in INPUT).

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


**Examples**

One could find examples in _dataset_ folder, at least one for each setting mode.


## output

The generalized enhanced suffix array is stored in the same directory DIR in a file called:

```sh
$(INPUT).K.gesa
```

Temporary files are stored in folders 

```sh
$(DIR)/partition/
$(DIR)/tmp/
```

## options

Output the Burrows-Wheeler transform:

```sh

make compile BWT=1

```

One can inform the maximum available internal memory to be used (in MB):

```sh

make clean
make compile MEMLIMIT=10

```

**debug**

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

* gsaca-k: generalized suffix array construction algorithm \[2\]
* lcp-PHI: LCP-array construction algorithm given the suffix array \[3\]

## citation

Please, if you use egsa tool in an academic setting cite the following paper:

	@article{Louza2017d,
		author = {Louza, Felipe A. and Telles, Guilherme P. and Hoffmann, Steve and Ciferri, Cristina D. A.},
		title={Generalized enhanced suffix array construction in external memory},
		journal={Algorithms for Molecular Biology},
		year = {2017},
		pages = {26:1--26:16},
		doi = {10.1186/s13015-017-0117-9}
	}

## references

\[1\] Louza, F. A., Telles, G. P., Ciferri, C. D. A. (2013). External Memory Generalized Suffix and LCP Arrays Construction. In Proc. CPM  (pp. 201-210).

\[2\] Louza, F. A., Gog, S., Telles, G. P. (2016). Induced Suffix Sorting for String Collections. In Proc. DCC (pp. 43-52), https://github.com/felipelouza/gsa-is 

\[3\] Kärkkäinen, J., Manzini, G., & Puglisi, S. J. (2009). Permuted Longest-Common-Prefix Array. In G. Kucherov & E. Ukkonen (Eds.), Proc. CPM (Vol. 5577, pp. 181–192).

\[4\] Louza, F. A., Telles, G. P., Hoffman, S., Ciferri, C. D. A. (2017). Generalized enhanced suffix array construction in external memory, Algorithms for Molecular Biology.

##thanks:

Thanks to Fabio Garofalo, Giovanna Rosone and Guilherme P. Telles by helpful suggestions and debugging.

