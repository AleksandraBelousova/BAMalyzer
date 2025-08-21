#include <iostream>
#include "BamAnalyzer.h"
#include <CLI/CLI.hpp>

int main(int argc, char** argv) {
    CLI::App app{"BAMalyzer: High-Performance Genomic Alignment Analysis Toolkit"};
    std::string input_bam;
    app.add_option("-i,--input", input_bam, "Input BAM file")->required()->check(CLI::ExistingFile);
    std::string output_json = "report.json";
    app.add_option("-o,--output", output_json, "Output JSON report file");
    int threads = 1;
    app.add_option("-t,--threads", threads, "Number of processing threads")->check(CLI::PositiveNumber);
    CLI11_PARSE(app, argc, argv);

    try {
        BamAnalyzer analyzer(input_bam, threads);
        analyzer.process();
        analyzer.write_json_report(output_json);
        std::cout << "Analysis finished successfully. Report generated at " << output_json << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}