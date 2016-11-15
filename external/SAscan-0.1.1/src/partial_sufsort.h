#ifndef __PARTIAL_SUFSORT_H_INCLUDED
#define __PARTIAL_SUFSORT_H_INCLUDED

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>

#include "divsufsort.h"
#include "divsufsort64.h"
#include "utils.h"
#include "rank.h"
#include "srank.h"
#include "gap_array.h"
#include "merge.h"
#include "bitvector.h"
#include "stream.h"
#include "sascan.h"
#include "settings.h"
    

template<typename output_type>
void SAscan(std::string input_filename, long ram_use, long size, unsigned char **BWT,
    bool in_recursion, bool compute_bwt, std::string text_filename, long text_offset);

//==============================================================================
// Compute partial suffix array of B[0..block_size) and store on disk.
// Optionally (of computed_bwt == true) also compute the BWT.
//==============================================================================
void compute_partial_sa_and_bwt(unsigned char *B, long block_size,
    long max_block_size, long ram_use, std::string text_fname,
    std::string sa_fname, bool compute_bwt, long n_block, bitvector *gt_eof_bv,
    unsigned char **BWT, long block_offset) {

  fprintf(stderr, "  compute-bwt = %s\n", compute_bwt ? "TRUE" : "FALSE");
  if (block_size <= MAX_32BIT_DIVSUFSORT_LENGTH) {

    // Just use use 32-bit divsufsort.
    fprintf(stderr, "  Computing partial SA (divsufsort): ");
    long double sa_start = utils::wallclock();
    int *SA = new int[block_size];
    divsufsort(B, SA, (int)block_size);
    fprintf(stderr, "%.2Lf\n", utils::wallclock() - sa_start);

    fprintf(stderr, "  Writing partial SA to disk ");
    long double writing_sa_start = utils::wallclock();
    if (n_block > 1 && max_block_size <= MAX_32BIT_DIVSUFSORT_LENGTH) {
      fprintf(stderr, "(using 32-bit ints): ");
      utils::write_objects_to_file<int>(SA, block_size, sa_fname);
    } else {
      fprintf(stderr, "(using 40-bit ints): ");
      stream_writer<uint40> *sa_writer = new stream_writer<uint40>(sa_fname, 2L << 20);
      for (long j = 0; j < block_size; ++j) sa_writer->write(uint40((unsigned long)SA[j]));
      delete sa_writer;
    }
    fprintf(stderr, "%.2Lf\n", utils::wallclock() - writing_sa_start);

    if (compute_bwt) {
      // Remap symbols of B back to original.
      fprintf(stderr, "  Re-remapping B: ");
      long double reremap_start = utils::wallclock();
      for (long j = 0; j < block_size; ++j) B[j] -= gt_eof_bv->get(j);
      fprintf(stderr, "%.2Lf\n", utils::wallclock() - reremap_start);

      // Compute BWT
      fprintf(stderr, "  Compute BWT: ");
      long double bwt_start = utils::wallclock();
      unsigned char *tmp = (unsigned char *)SA;
      for (long j = 0, jj = 0; j < block_size; ++j)
        if (SA[j]) tmp[jj++] = B[SA[j] - 1];
      std::copy(tmp, tmp + block_size - 1, B);
      *BWT = B;
      fprintf(stderr, "%.2Lf\n", utils::wallclock() - bwt_start);
    } else delete[] B;
    
    delete[] SA;
    delete gt_eof_bv;
    
  } else if (9L * block_size <= ram_use) {
    // Easy case: block_size > 2147483647 but enough RAM to use divsufsort64.
    fprintf(stderr, "  Computing partial SA (divsufsort64): ");
    long double sa_start = utils::wallclock();
    long *SA = new long[block_size];
    divsufsort64(B, SA, block_size);
    fprintf(stderr, "%.2Lf\n", utils::wallclock() - sa_start);
    
    fprintf(stderr, "  Writing partial SA to disk ");
    long double writing_sa_start = utils::wallclock();

    // (max_block_size >= block_size > 2147483647) => use uint40.
    // Currently for testing purposes we leave the checks below. In the final
    // version always the 'else' branch will execute.
    if (n_block > 1 && max_block_size <= MAX_32BIT_DIVSUFSORT_LENGTH) {
      fprintf(stderr, " (using 32-bit bits): ");
      stream_writer<int> *sa_writer = new stream_writer<int>(sa_fname, 2L << 20);
      for (long j = 0; j < block_size; ++j) sa_writer->write((int)SA[j]);
      delete sa_writer;
    } else {
      fprintf(stderr, "(using 40-bit ints): ");
      stream_writer<uint40> *sa_writer = new stream_writer<uint40>(sa_fname, 2L << 20);
      for (long j = 0; j < block_size; ++j) sa_writer->write(uint40((unsigned long)SA[j]));
      delete sa_writer;
    }
    fprintf(stderr, "%.2Lf\n", utils::wallclock() - writing_sa_start);

    if (compute_bwt) {
      // Remap symbols of B back to original.
      fprintf(stderr, "  Re-remapping B: ");
      long double reremap_start = utils::wallclock();
      for (long j = 0; j < block_size; ++j) B[j] -= gt_eof_bv->get(j);
      fprintf(stderr, "%.2Lf\n", utils::wallclock() - reremap_start);

      // Compute BWT
      fprintf(stderr, "  Compute BWT: ");
      long double bwt_start = utils::wallclock();
      unsigned char *tmp = (unsigned char *)SA;
      for (long j = 0, jj = 0; j < block_size; ++j)
        if (SA[j]) tmp[jj++] = B[SA[j] - 1];
      std::copy(tmp, tmp + block_size - 1, B);
      *BWT = B;
      fprintf(stderr, "%.2Lf\n", utils::wallclock() - bwt_start);
    } else  delete[] B;
    
    delete gt_eof_bv;
    delete[] SA;
    
  } else {
    // (block_size > 2147483647 and 8 * block_size > ram_use) => use recursion.
    // To save I/O from recursion we also get the BWT, which is obtained during
    // the storage of the resulting partial SA to disk.
    fprintf(stderr, "  Recursively computing partial SA:\n");
    long double rec_partial_sa_start = utils::wallclock();

    // Save the remapped block to temp file.
    std::string B_fname = text_fname + ".current_block";
    utils::write_objects_to_file<unsigned char>(B, block_size, B_fname);

    // Free all memory.
    delete gt_eof_bv;
    delete[] B;

    if (max_block_size <=  MAX_32BIT_DIVSUFSORT_LENGTH)
      SAscan<int>(B_fname, ram_use, 0, BWT, true, compute_bwt, text_fname, block_offset);
    else
      SAscan<uint40>(B_fname, ram_use, 0, BWT, true, compute_bwt, text_fname, block_offset);
 
    // Delete the temp file.
    utils::file_delete(B_fname);
    utils::execute("mv " + B_fname + ".sa5 " + sa_fname);

    fprintf(stderr, "  Recursively computing partial SA: %.2Lf\n",
        utils::wallclock() - rec_partial_sa_start);
  }
}


//==============================================================================
// Compute partial SAs and gap arrays and write to disk.
//==============================================================================
void partial_sufsort(std::string filename, long length, long max_block_size,
    long ram_use) {
  long n_block = (length + max_block_size - 1) / max_block_size;
  long block_id = n_block - 1, prev_end = length;

  while (block_id >= 0) {
    long beg = max_block_size * block_id;
    long end = std::min(length, beg + max_block_size);
    long block_size = end - beg; // B = text[beg..end), current block
    bool need_streaming = (block_id + 1 != n_block);

    fprintf(stderr, "Processing block %ld/%ld [%ld..%ld):\n",
      n_block - block_id, n_block, beg, end);
    fprintf(stderr, "  need_streaming = %s\n", need_streaming ? "TRUE" : "FALSE");
    fprintf(stderr, "  block_size = %ld (%.2LfMiB)\n", block_size,
        (long double)block_size / (1 << 20));

    // Read current block.
    fprintf(stderr, "  Reading block: ");
    long double read_start = utils::wallclock();
    unsigned char *B = new unsigned char[block_size];
    utils::read_block(filename, beg, block_size, B);
    unsigned char last = B[block_size - 1];
    fprintf(stderr, "%.2Lf\n", utils::wallclock() - read_start);

    bitvector *gt_eof_bv = NULL;
    if (need_streaming) {
      // Read previous block.
      fprintf(stderr, "  Read previous block: ");
      long double prev_block_reading_start = utils::wallclock();
      unsigned char *extprevB = new unsigned char[max_block_size + 1];
      long ext_prev_block_size = prev_end - end + 1;
      utils::read_block(filename, end - 1, ext_prev_block_size, extprevB);
      fprintf(stderr, "%.2Lf\n", utils::wallclock() - prev_block_reading_start);

      // Compute gt_eof.
      fprintf(stderr, "  Compute gt_eof_bv: ");
      long double gt_eof_start = utils::wallclock();
      gt_eof_bv = new bitvector(block_size);
      bitvector *gt_head_bv = new bitvector(filename + ".gt_head");
      compute_gt_eof_bv(extprevB, ext_prev_block_size, B, block_size, gt_head_bv, gt_eof_bv);
      delete gt_head_bv;
      delete[] extprevB;
      fprintf(stderr, "%.2Lf\n", utils::wallclock() - gt_eof_start);

      // FIXME
      bool used = false;
      for (long j = 0; j < block_size; ++j)
        if (B[j] == 255) used = true;
      if (used) {
        printf("\nError: Input text cannot contain symbol 255.\n");
        std::exit(EXIT_FAILURE);
      }

      // Remap symbols of B.
      fprintf(stderr, "  Remapping B: ");
      long double remap_start = utils::wallclock();
      for (long j = 0; j < block_size; ++j) B[j] += gt_eof_bv->get(j);
      fprintf(stderr, "%.2Lf\n", utils::wallclock() - remap_start);
    }

    // Compute the head of the new gt bitvector.
    long whole_suffix_rank = 0;
    if (n_block > 1) {
      fprintf(stderr, "  Compute new_gt_head_bv: ");
      long double new_gt_head_bv_start = utils::wallclock();
      bitvector *new_gt_head_bv = new bitvector(block_size);
      whole_suffix_rank = compute_new_gt_head_bv(B, block_size, new_gt_head_bv);
      new_gt_head_bv->save(filename + ".new_gt_head");
      delete new_gt_head_bv;
      fprintf(stderr, "%.2Lf\n", utils::wallclock() - new_gt_head_bv_start);
    }

    // Compute/save partial SA. Compute BWT if it's not the last block.
    unsigned char *BWT = NULL;
    std::string sa_fname = filename + ".partial_sa." + utils::intToStr(block_id);
    compute_partial_sa_and_bwt(B, block_size, max_block_size, ram_use,
        filename, sa_fname, need_streaming, n_block, gt_eof_bv, &BWT, beg);

    if (need_streaming) {
      // Build the rank support for BWT.
      fprintf(stderr, "  Building the rank data structure: ");
      long double building_rank_start = utils::wallclock();
      rank4n<> *rank = new rank4n<>(BWT, block_size - 1);
      delete[] BWT;
      fprintf(stderr, "%.2Lf\n", utils::wallclock() - building_rank_start);

      // Get symbol counts of a block and turn into exclusive partial sum.
      long *count = new long[256];
      std::copy(rank->m_count, rank->m_count + 256, count);
      ++count[last];
      for (long jj = 0, s = 0, t; jj < 256; ++jj)
        { t = count[jj]; count[jj] = s; s += t; }

      // Allocate the gap array, do the streaming and store gap to disk.
      fprintf(stderr, "  Stream:\r");
      long double stream_start = utils::wallclock();
      buffered_gap_array *gap = new buffered_gap_array(block_size + 1);
      bit_stream_writer *new_gt_tail = new bit_stream_writer(filename + ".new_gt_tail");
      bit_stream_reader *gt_tail = prev_end < length ? new bit_stream_reader(filename + ".gt_tail") : NULL;
      long i = 0;
      unsigned char next_gt = 0;

      backward_stream_reader<unsigned char> *text_streamer =
        new backward_stream_reader<unsigned char>(filename);
      for (long j = length - 1, dbg = 0; j >= prev_end; --j, ++dbg) { // stream the tail
        if (dbg == (1 << 24)) {
          long double elapsed = utils::wallclock() - stream_start;
          long double streamed_mib = (1.L * (length - j)) / (1 << 20);
          fprintf(stderr,"  Stream: %.1Lf%%. Time: %.2Lf. Speed: %.2LfMiB/s\r",
            (100.L * (length - j)) / (length - end),
            elapsed,
            streamed_mib / elapsed);
          dbg = 0;
        }

        unsigned char c = text_streamer->read();     // c = text[j]
        i = count[c] + rank->rank(i - (i > whole_suffix_rank), c);
        if (c == last && next_gt) ++i;               // next_gt = gt[j + 1]
        new_gt_tail->write(i > whole_suffix_rank);
        gap->increment(i);
        next_gt = gt_tail->read();
      }

      delete gt_tail;

      bit_stream_reader *gt_head = new bit_stream_reader(filename + ".gt_head");

      for (long j = prev_end - 1, dbg = 0; j >= end; --j, ++dbg) { // stream the head
        if (dbg == (1 << 24)) {
          long double elapsed = utils::wallclock() - stream_start;
          long double streamed_mib = (1.L * (length - j)) / (1 << 20);
          fprintf(stderr,"  Stream: %.1Lf%%. Time: %.2Lf. Speed: %.2LfMiB/s\r",
            (100.L * (length - j)) / (length - end),
            elapsed,
            streamed_mib / elapsed);
          dbg = 0;
        }

        unsigned char c = text_streamer->read();  // c = text^R[j]
        i = count[c] + rank->rank(i - (i > whole_suffix_rank), c);
        if (c == last && next_gt) ++i;            // next_gt = gt[j + 1]
        new_gt_tail->write(i > whole_suffix_rank);
        gap->increment(i);
        next_gt = gt_head->read();
      }

      long double stream_time = utils::wallclock() - stream_start;
      long double streamed_mib = (1.L * (length - end)) / (1 << 20);
      fprintf(stderr,"  Stream: 100.0%%. Time: %.2Lf. Speed: %.2LfMiB/s\n",
        stream_time,
        streamed_mib / stream_time);

      delete text_streamer;
      delete gt_head;
      delete new_gt_tail;
      delete rank;
      delete[] count;

      utils::execute("mv " + filename + ".new_gt_tail " + filename + ".gt_tail");

      // Save gap to file.
      gap->save_to_file(filename + ".gap." + utils::intToStr(block_id));
      delete gap;
    }

    prev_end = end;
    end = beg;
    --block_id;
    if (n_block > 1)
      utils::execute("mv " + filename + ".new_gt_head " + filename + ".gt_head");
  }

  if (utils::file_exists(filename + ".gt_head")) utils::file_delete(filename + ".gt_head");
  if (utils::file_exists(filename + ".gt_tail")) utils::file_delete(filename + ".gt_tail");
}

#endif // __PARTIAL_SUFSORT_H_INCLUDED
