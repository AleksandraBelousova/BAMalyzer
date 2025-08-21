#include "BamAnalyzer.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <numeric>
#include <cmath>
#include <omp.h>
#include <nlohmann/json.hpp>

BamAnalyzer::BamAnalyzer(std::string bam_path, int threads)
    : bam_path_(std::move(bam_path)), threads_(threads) {
    bam_file_ = sam_open(bam_path_.c_str(), "r");
    if (!bam_file_) throw std::runtime_error("Error: Could not open BAM file " + bam_path_);
    header_ = sam_hdr_read(bam_file_);
    if (!header_) {
        sam_close(bam_file_);
        throw std::runtime_error("Error: Could not read header from " + bam_path_);
    }
    if (threads_ > 1) hts_set_threads(bam_file_, threads_);
}

BamAnalyzer::~BamAnalyzer() {
    if (header_) bam_hdr_destroy(header_);
    if (bam_file_) sam_close(bam_file_);
}

void BamAnalyzer::process() {
    std::vector<PerThreadMetrics> thread_metrics(threads_);
    #pragma omp parallel num_threads(threads_)
    {
        PerThreadMetrics& local_metrics = thread_metrics[omp_get_thread_num()];
        bam1_t* record = bam_init1();
        while (true) {
            int read_result;
            #pragma omp critical(bam_read)
            {
                read_result = sam_read1(bam_file_, header_, record);
            }
            if (read_result < 0) break;

            local_metrics.total_reads++;
            if (!(record->core.flag & BAM_FQCFAIL)) local_metrics.qc_passed_reads++;
            if (record->core.flag & BAM_FUNMAP) continue;
            
            local_metrics.mapped_reads++;
            local_metrics.mapq_dist[record->core.qual]++;
            if ((record->core.flag & BAM_FPROPER_PAIR) && record->core.isize != 0) {
                local_metrics.insert_size_dist[std::abs(record->core.isize)]++;
            }
        }
        bam_destroy1(record);
    }

    for (const auto& local : thread_metrics) {
        total_reads_ += local.total_reads;
        mapped_reads_ += local.mapped_reads;
        qc_failed_reads_ += (local.total_reads - local.qc_passed_reads);
        for (const auto& p : local.mapq_dist) mapq_dist_[p.first] += p.second;
        for (const auto& p : local.insert_size_dist) insert_size_dist_[p.first] += p.second;
    }
}

void BamAnalyzer::write_json_report(const std::string& output_path) const {
    nlohmann::json report;
    report["inputFile"] = bam_path_;
    report["totalReads"] = total_reads_;
    report["mappedReads"] = mapped_reads_;
    report["qcFailedReads"] = qc_failed_reads_;
    for (const auto& p : mapq_dist_) report["mapqDistribution"][std::to_string(p.first)] = p.second;
    for (const auto& p : insert_size_dist_) report["insertSizeDistribution"][std::to_string(p.first)] = p.second;
    
    std::ofstream out(output_path);
    if (!out) throw std::runtime_error("Error: Could not open output file " + output_path);
    out << report.dump(4);
}