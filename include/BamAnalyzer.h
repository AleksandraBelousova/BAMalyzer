#pragma once
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <htslib/sam.h>

class BamAnalyzer {
public:
    BamAnalyzer(std::string bam_path, int threads);
    ~BamAnalyzer();
    void process();
    void write_json_report(const std::string& output_path) const;
    BamAnalyzer(const BamAnalyzer&) = delete;
    BamAnalyzer& operator=(const BamAnalyzer&) = delete;

private:
    struct PerThreadMetrics {
        long total_reads = 0;
        long mapped_reads = 0;
        long qc_passed_reads = 0;
        std::map<int, long> mapq_dist;
        std::map<long, long> insert_size_dist;
    };
    std::string bam_path_;
    int threads_;
    samFile* bam_file_ = nullptr;
    bam_hdr_t* header_ = nullptr;
    long total_reads_ = 0;
    long mapped_reads_ = 0;
    long qc_failed_reads_ = 0;
    std::map<int, long> mapq_dist_;
    std::map<long, long> insert_size_dist_;
};