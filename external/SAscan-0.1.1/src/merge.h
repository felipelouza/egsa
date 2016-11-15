#ifndef __MERGE_H_INCLUDED
#define __MERGE_H_INCLUDED

#include <string>
#include <algorithm>

#include "utils.h"
#include "stream.h"
#include "uint40.h"


//==============================================================================
// Merge partial suffix arrays and (optionally) compute BWT.
//==============================================================================
template<typename offset_type, typename output_type>
void merge(std::string input_filename, long length, long max_block_size,
    long n_block, long ram_use, std::string out_filename, unsigned char **BWT,
    bool compute_bwt, std::string text_filename, long text_offset) {

  // Invariant: 5 * length <= ram_use.
  if (n_block <= 1) {
    fprintf(stderr, "Error: trying to merge %ld blocks.\n", n_block);
    std::exit(EXIT_FAILURE);
  }

  unsigned char *text = NULL;
  long buffer_size = 0;
  long pieces = (1 + sizeof(offset_type)) * n_block - 1 + sizeof(output_type);
  if (compute_bwt) {
    // Read the original block of text
    text = new unsigned char[length];
    utils::read_block(text_filename, text_offset, length, text);

    *BWT = new unsigned char[length - 1];
    long merge_ram_use = ram_use - 2 * length; // is positive
    buffer_size = (merge_ram_use + pieces - 1) / pieces;
  } else buffer_size = (ram_use + pieces - 1) / pieces;

  fprintf(stderr, "Buffer size for merging: %ld\n", buffer_size);
  fprintf(stderr, "Compute BWT = %s\n", compute_bwt ? "TRUE" : "FALSE");
  fprintf(stderr, "sizeof(offset_type) = %ld\n", sizeof(offset_type));
  fprintf(stderr, "sizeof(output_type) = %ld\n", sizeof(output_type));

  stream_writer<output_type> *output = new stream_writer<output_type>(out_filename, sizeof(output_type) * buffer_size);
  stream_reader<offset_type> **sparseSA = new stream_reader<offset_type>*[n_block];
  vbyte_stream_reader **gap = new vbyte_stream_reader*[n_block - 1];
  for (long i = 0; i < n_block; ++i) {
    sparseSA[i] = new stream_reader<offset_type>(input_filename + ".partial_sa." +
        utils::intToStr(i), sizeof(offset_type) * buffer_size);
    if (i + 1 != n_block)
      gap[i] = new vbyte_stream_reader(input_filename + ".gap." + utils::intToStr(i), buffer_size);
  }

  // Merge.
  long *suffix_rank = new long[n_block];
  for (long i = 0; i + 1 < n_block; ++i)
    suffix_rank[i] = gap[i]->read();
  suffix_rank[n_block - 1] = 0;

  long *block_rank = new long[n_block];
  std::fill(block_rank, block_rank + n_block, 0);

  long *suf_ptr = new long[n_block]; // First non-extracted suffix in the block.
  std::fill(suf_ptr, suf_ptr + n_block, 0);

  fprintf(stderr, "Merging:\r");
  long double merge_start = utils::wallclock();
  for (long i = 0, bwt_ptr = 0, dbg = 0; i < length; ++i, ++dbg) {
    if (dbg == (1 << 23)) {
      long double elapsed = utils::wallclock() - merge_start;
      fprintf(stderr, "Merging: %.1Lf%%. Time: %.2Lfs\r",
          (100.L * i) / length, elapsed);
      dbg = 0;
    }

    // Find the leftmost block j with block_rank[j] == suffix_rank[j].
    long j = 0;
    while (j < n_block && block_rank[j] != suffix_rank[j]) ++j;

    // Extract the suffix.
    unsigned long SAi = (unsigned long)sparseSA[j]->read() +
      (unsigned long)max_block_size * j; // SA[i]
    output->write((output_type)SAi);
    
    // Compute the BWT entry, if it was requested.
    if (compute_bwt && SAi > 0)
      (*BWT)[bwt_ptr++] = text[SAi - 1];

    // Update suffix_rank[j].    
    ++suffix_rank[j];
    if (j + 1 != n_block)
      suffix_rank[j] += gap[j]->read();
    
    // Update block_rank[0..j].
    for (long k = 0; k <= j; ++k)
      ++block_rank[k];
  }

  long double merge_time = utils::wallclock() - merge_start;
  fprintf(stderr, "Merging: 100.0%%. Time: %.2Lfs\n", merge_time);

  // Clean up.
  delete output;
  if (text)
    delete[] text;

  for (long i = 0; i < n_block; ++i) {
    delete sparseSA[i];
    if (i + 1 != n_block)
      delete gap[i];
  }

  delete[] sparseSA;
  delete[] gap;

  delete[] block_rank;
  delete[] suffix_rank;
  delete[] suf_ptr;
  
  for (int i = 0; i < n_block; ++i) {
    utils::file_delete(input_filename + ".partial_sa." + utils::intToStr(i));
    if (i + 1 != n_block)
      utils::file_delete(input_filename + ".gap." + utils::intToStr(i));
  }
}

#endif // __MERGE_H_INCLUDED

