#
# GNU GENERAL PUBLIC LICENSE
# Author Pierre Lindenbaum
#
.PHONY:all clean

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


load	./illumina.so

clean:
	rm -f illumina.so
	
