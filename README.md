plugins4make
============

Plugin for GNU Make 4 ( see http://www.gnu.org/software/make/manual/make.html#Loading-Objects )

# Introduction

> The latest version of GNU-Make http://www.gnu.org/software/make/  provides many advanced capabilities, including many useful functions.
> However, it does not contain a complete programming language and so it has limitations.
> Sometimes these limitations can be overcome through use of the shell function to invoke a separate program, although this can be inefficient. 
> On systems which support dynamically loadable objects, you can write your own extension in any language (which can be compiled into such an object) and load it to provide extended capabilities 


# A GNU-make plug-in for the Illumina FASTQs.

from http://support.illumina.com/help/SequencingAnalysisWorkflow/Content/Vault/Informatics/Sequencing_Analysis/CASAVA/swSEQ_mCA_FASTQFiles.htm

Illumina FASTQ files use the following naming scheme:
```
<sample name>_<barcode sequence>_L<lane (0-padded to 3 digits)>_R<read number>_<set number (0-padded to 3 digits>.fastq.gz
```

For example, the following is a valid FASTQ file name:

```
NA10831_ATCACG_L002_R1_001.fastq.gz
```

Here I'm writing a set of new functions for makefile to extract the different parts of a fastq file-name:

First a `struct` holding the parts of the file is created:

```C
enum E_IlluminaComponent
	{
	E_sampleName,
	E_barcodeSequence,
	E_lane,
	E_readNumber,
	E_setNumber
	};


typedef struct illumina_scheme_t
	{
	char* filename;
	char* components[NUM_ILLUMINA_COMPONENTS];
	} IlluminaScheme,*IlluminaSchemePtr ;
```	

and a function parsing the filenames is created:

```C
IlluminaSchemePtr IlluminaSchemeNew(const char* filename)
	{
	...
	}
```

when the plugin `llumina` is loaded as a dynamic C library, the method `llumina_gmk_setup` is called,
and we tell **make** about the new functions with `gmk_add_function(name,callback,min_args,max_args,no_expand_content)` :

```C
int illumina_gmk_setup ()
  {
   gmk_add_function ("illumina_sample",illumina_sample, 1, 1, 0);
   gmk_add_function ("illumina_lane",illumina_lane, 1, 1, 0);
   (...)
  }
```

> A function registered with make must match the `gmk_func_ptr` type.
> It will be invoked with three parameters: **name** (the name of the function), **argc** (the number of arguments to the function), and **argv** (an array of pointers to arguments to the function). The last pointer (that is, argv[argc]) will be null (0).
> The return value of the function is the result of expanding the function.

```C
char* illumina_sample(const char *function_name, unsigned int argc, char **argv)
	{
	/ùextract the filename(s), build and return a string containing the samples */
	}
```

## Compiling

the plugin must be compiled as a dynamic C library.

Note: The manual says this step can also be generated in the final 'Makefile' (via `load ./illumina.so`) but I was not able to compile a missing library (`illumina.so  cannot open shared object file: No such file or directory`)

so I compiled it by hand:
```
gcc -Wall -I/path/to/sources/make-4.0 -shared -fPIC -o illumina.so illumina.c
```
## Test

here is the makefile:

```Makefile
SAMPLES=  NA10831_ATCACG_L002_R1_001.fastq.gz \
	  hello \
	  NA10832_ATCACG_L002_R1_001.fastq.gz \
	  NA10831_ATCACG_L002_R2_001.fastq.gz \
	  NA10832_ATCACG_L002_R2_001.fastq.gz \
	  NA10833_ATCAGG_L003_R1_003.fastq.gz \
	  NA10833_ATCAGG_L003_R1_003.fastq.gz \
	  ERROR_ATCAGG_x003_R3_0z3.fastq.gz \
	  false

all:
	@echo "SAMPLES: " $(illumina_sample  ${SAMPLES} )
	@echo "BARCODES: " $(illumina_barcode  ${SAMPLES} )
	@echo "LANE: " $(illumina_lane  ${SAMPLES} )
	@echo "READ: " $(illumina_read  ${SAMPLES} )
	@echo "SET: " $(illumina_set  ${SAMPLES} )
```

output

```
$ make
SAMPLES:  NA10831 NA10832 NA10833
BARCODES:  ATCACG ATCAGG
LANE:  L002 L003
READ:  R1 R2
SET:  001 003
```

# Author

Pierre Lindenbaum PhD

@yokofakun : https://twitter.com/yokofakun


