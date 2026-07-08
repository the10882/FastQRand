# FastQRand
The program is designed to generate synthetic DNA sequencing data in FASTQ format. Such data is used for testing bioinformatics pipelines, debugging genome assembly algorithms, read alignment, and quality analysis

To configure the program, modify the constant values at the top of the code before compiling.
```
const char BASES[4] = {'G', 'T', 'A', 'C'}; -- nucleotide bases used
const int SEQ_LENGTH = 102; -- read length
const int DEPTH = 137000000; -- number of reads
const std::string FLENAME = "random.fastq"; -- output file name
const int BUFFER_SIZE = 1000000; -- buffer size (between file writes)
const int THREADS = 30; -- number of threads
```
