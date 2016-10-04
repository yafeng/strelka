// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Strelka - Small Variant Caller
// Copyright (c) 2009-2016 Illumina, Inc.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//

/// \file

/// \author Chris Saunders
///

#include "starling_common/starling_ref_seq.hh"

#include "htsapi/samtools_fasta_util.hh"
#include "htsapi/bam_header_util.hh"



void
get_starling_ref_seq(const starling_base_options& opt,
                     reference_contig_segment& ref)
{
    assert(not opt.referenceFilename.empty());

    // make a temp copy of report_range here to determine how much we
    // pull from ref_seq:
    pos_range ref_range = opt.user_report_range;
    static const pos_t region_read_size_pad(512);
    const pos_t pad_size(opt.max_indel_size+region_read_size_pad);
    if (ref_range.is_begin_pos)
    {
        ref_range.begin_pos -= 1;
        ref_range.begin_pos = std::max(0,ref_range.begin_pos-pad_size);
    }
    else
    {
        ref_range.set_begin_pos(0);
    }

    if (ref_range.is_end_pos)
    {
        ref_range.end_pos += pad_size;
    }

    ref.set_offset(ref_range.begin_pos);

    assert(! opt.bam_seq_name.empty());

    // note: the ref function below takes closed-closed endpoints, so we subtract one from endPos
    get_standardized_region_seq(opt.referenceFilename, opt.bam_seq_name, ref_range.begin_pos, ref_range.end_pos-1, ref.seq());
}



void
setRefSegment(
    const starling_base_options& opt,
    const std::string& chrom,
    const known_pos_range2& range,
    reference_contig_segment& ref)
{
    assert(! chrom.empty());

    ref.set_offset(range.begin_pos());
    // note: the ref function below takes closed-closed endpoints, so we subtract one from endPos
    get_standardized_region_seq(opt.referenceFilename, chrom, range.begin_pos(), range.end_pos()-1, ref.seq());
}



static
std::string
getSamtoolsRegionString(
    const std::string& chromName,
    const known_pos_range2& range)
{
    std::ostringstream oss;
    oss << chromName << ':' << range.begin_pos()+1 << '-' << range.end_pos();
    return oss.str();
}



void
getStrelkaAnalysisRegions(
    const std::string& region,
    const unsigned maxIndelSize,
    AnalysisRegionInfo& rinfo)
{
    int32_t regionBeginPos(0), regionEndPos(0);
    parse_bam_region(region.c_str(), rinfo.regionChrom, regionBeginPos, regionEndPos);

    // translate from samtools to strelka range:
    rinfo.regionRange.set_range(regionBeginPos, regionEndPos+1);

    rinfo.streamerRegionRange = rinfo.regionRange;
    rinfo.streamerRegionRange.expandBy(maxIndelSize);
    rinfo.streamerRegionRange.makeNonNegative();

    rinfo.streamerRegion = getSamtoolsRegionString(rinfo.regionChrom, rinfo.streamerRegionRange);

    static const pos_t region_read_size_pad(512);
    rinfo.refRegionRange = rinfo.streamerRegionRange;
    rinfo.refRegionRange.expandBy(region_read_size_pad);
    rinfo.refRegionRange.makeNonNegative();
}
