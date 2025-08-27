
A сommand-line utility for quality control (QC) of genomic alignment data. It is optimised for rapid, multi-threaded processing of multi-gigabyte BAM files.

The system performs a single-pass analysis of BAM files, calculating key quality metrics with minimal memory consumption. Performance is achieved through native compilation, OpenMP-based parallelism, and the use of the low-level C library `htslib`—the industry standard for handling SAM/BAM/CRAM formats.

---

### Architecture \ Technology

The application is a single, statically-linked executable, which ensures maximum portability and straightforward integration into any bioinformatics pipeline.

*   **Core:** C++17, compiled with high optimisation flags and stringent compiler checks (`-Wall`, `-Wextra`). All logic is encapsulated within the `BamAnalyzer` class.
*   **Build System:** CMake. The `CMakeLists.txt` is configured for building in a Linux environment (WSL2 is recommended on Windows).
*   **BAM Parsing:** `htslib`. Direct use of the original C library ensures maximum parsing performance by eliminating any overhead.
*   **Parallelism:** OpenMP. A classic and efficient "single-producer, multiple-consumer" model allows all available CPU cores to be engaged for data processing.
*   **Command-Line Interface (CLI):** `CLI11`. A lightweight, header-only library for creating an intuitive and powerful command-line interface.
*   **Reporting:** `nlohmann/json`. Used for generating a structured JSON report with the analysis results.

---

### Getting Started

**Prerequisites:**
*   A C++ compiler (GCC/Clang)
*   CMake (version 3.16+)
*   Git
*   Build dependencies for `htslib`: `build-essential`, `autoconf`, `automake`, `libtool`, `zlib1g-dev`, `libbz2-dev`, `liblzma-dev`, `libcurl4-gnutls-dev`, `libssl-dev`.

**Dependency Installation (Debian/Ubuntu/WSL):**
```bash
sudo apt-get update && sudo apt-get install -y build-essential cmake git autoconf automake libtool zlib1g-dev libbz2-dev liblzma-dev libcurl4-gnutls-dev libssl-dev
```

**Build Procedure:**

1.  **Clone the Repository**
    ```bash
    git clone https://github.com/YourUsername/BAMalyzer.git # Replace with your URL
    cd BAMalyzer
    ```

2.  **Build `htslib` (One-Time Step)**
    The external dependency `htslib` is built manually to ensure stability.
    ```bash
    git clone --recursive https://github.com/samtools/htslib.git
    cd htslib
    autoreconf -i
    ./configure --prefix=$(pwd)/install
    make
    make install
    cd .. 
    ```
    *This step creates a `./htslib/install` directory containing the compiled library.*

3.  **Build `BAMalyzer`**
    ```bash
    mkdir build && cd build
    cmake ..
    make -j$(nproc)
    ```
    *An executable named `bamalyzer` will be created in the `./build` directory.*

---

### Utility Specification

The executable is located in the `./build` directory.

#### `bamalyzer [OPTIONS]`
> Performs a QC analysis on an input BAM file and generates a JSON report.

*   **Options:**
    *   `-i, --input TEXT` (Required): Path to the input BAM file.
    *   `-o, --output TEXT` (Optional): Path to the output JSON file. Defaults to `report.json`.
    *   `-t, --threads INT` (Optional): Number of processing threads. Defaults to 1.

*   **Example `bash` Usage:**
    ```bash
    # Executed from the project root directory (./BAMalyzer)
    ./build/bamalyzer \
      -i ./test_data/HG00096.mapped.ILLUMINA.bwa.GBR.low_coverage.20120522.bam \
      -o qc_report.json \
      -t 4
    ```

#### Output Report Format (`JSON`)
> A structured report containing key quality control metrics.

*   **Example Structure:**
    ```json
    {
      "inputFile": "./test_data/my_alignments.bam",
      "totalReads": 145063589,
      "mappedReads": 144534109,
      "qcFailedReads": 0,
      "mapqDistribution": {
        "0": 7950899,
        "60": 123811466,
        "...": "..."
      },
      "insertSizeDistribution": {
        "150": 563618,
        "160": 1180020,
        "...": "..."
      }
    }
    ```

---

### Performance & Production Use

> BAMalyzer is engineered as a high-performance tool ready for integration. Its performance is directly dependent on the I/O subsystem speed and the number of available CPU cores.
For use in pipelines:
1.   Ensure `htslib` and other system dependencies are available on the target machine, or consider a fully static build of `bamalyzer` for maximum portability.
2.  Adjust the thread count (`--threads`) according to the server's resources and the number of concurrent tasks to avoid system overload.
3.  The utility will exit with a non-zero status code upon failure (e.g., input file not found), allowing it to be used in scripts and workflow managers such as Snakemake or Nextflow.
```
