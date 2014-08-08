#
# The MIT License (MIT)
#
# Copyright (c) 2014 Pierre Lindenbaum
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
#

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
	@echo "READ: " $(illumina_read  ${SAMPLES} )
	@echo "SET: " $(illumina_set  ${SAMPLES} )



illumina.so: test.c
	$(CC) -shared -fPIC -o $ $<

load	./illumina.so


