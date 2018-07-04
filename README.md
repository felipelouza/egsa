# egsa tool

This software is the implementation of eGSA \[1,2\] [ALMOB](https://doi.org/10.1186/s13015-017-0117-9), an external memory algorithm to construct generalized enhanced suffix arrays.

Given a collection of K strings, eGSA outputs the:

* Generalized suffix array 
* LCP-array 
* Burrows-Wheeler transform.

## install

```sh
git clone https://github.com/felipelouza/egsa.git
cd egsa
make
```

## run

Given a collection of K strings concatenated in a single file INPUT in the directory DIR.

```sh
./egsa [options] FILE K

Available options:
	-h    this help message
  -b    output BWT (ext: .bwt)
	-c    check SA and LCP
	-v    verbose output (more v's for more verbose)
```

_notes_

* K=0 gives all strings as input.
* Supported extensions are: _.txt_, _.fasta_ and _.fastq_.
* Strings are separated per '\0' (new line) in _.txt_.

## output

The generalized enhanced suffix array is stored in the same directory of FILE:

```sh
FILE.K.gesa
```

The generalized BWT (_option -b_) is stored in the same directory of FILE:

```sh
FILE.K.bwt
```

Temporary files are stored in subfolders:

```sh
partition/
tmp/
```

## options

Output the BWT inside GESA structure, that is GESA[i] = (text, suffix, lcp, bwt):

```sh
make clean
make compile BWT=1
```

One can inform the maximum available internal memory to be used (in MB):

```sh

make clean
make compile MEMLIMIT=10

```

## debug

To see a more detailed execution output use:

```sh
make clean
make compile DEBUG=1
```


One can check if the output produced by egsa is correct:

```sh
make clean
make run CHECK=1
```

## external resources

We have included the source codes of the following algorithms: 

* gsaca-k: generalized suffix array construction algorithm \[3\]
* lcp-PHI: LCP-array construction algorithm given the suffix array \[4\]

## citation

Please, if you use egsa tool in an academic setting cite the following paper \[1\]:

	@article{Louza2017d,
		author = {Louza, Felipe A. and Telles, Guilherme P. and Hoffmann, Steve and Ciferri, Cristina D. A.},
		title={Generalized enhanced suffix array construction in external memory},
		journal={Algorithms for Molecular Biology},
		year = {2017},
		pages = {26:1--26:16},
		doi = {10.1186/s13015-017-0117-9}
	}

## references

\[1\] Louza, F. A., Telles, G. P., Hoffmann, S., Ciferri, C. D. A. (2017). Generalized enhanced suffix array construction in external memory. Algorithms for Molecular Biology 12(1): 26:1-26:16. [link](https://almob.biomedcentral.com/articles/10.1186/s13015-017-0117-9)  

\[2\] Louza, F. A., Telles, G. P., Ciferri, C. D. A. (2013). External Memory Generalized Suffix and LCP Arrays Construction. In Proc. CPM  (pp. 201-210).

\[3\] Louza, F. A., Gog, S., Telles, G. P. (2016). Inducing enhanced suffix arrays for string collections. Theor. Comput. Sci. 678: 22-39, [github](https://github.com/felipelouza/gsa-is)

\[4\] Kärkkäinen, J., Manzini, G., & Puglisi, S. J. (2009). Permuted Longest-Common-Prefix Array. In G. Kucherov & E. Ukkonen (Eds.), Proc. CPM (Vol. 5577, pp. 181–192).

## thanks

Thanks to Fabio Garofalo, [Giovanna Rosone](https://github.com/giovannarosone), [Giovanni Manzini](https://github.com/giovmanz) and Guilherme P. Telles by helpful suggestions and debugging.

