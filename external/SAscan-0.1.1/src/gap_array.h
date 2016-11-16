#ifndef __GAP_ARRAY_H_INCLUDED
#define __GAP_ARRAY_H_INCLUDED

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>

#include "utils.h"


struct buffered_gap_array {
  buffered_gap_array(long n) {
    if (n <= 0) {
      fprintf(stderr, "Error: constructing a gap array of length 0.\n");
      std::exit(EXIT_FAILURE);
    }

    m_length = n;
    m_count = new unsigned char[m_length];
    std::fill(m_count, m_count + m_length, 0);
    m_buf = new long[k_bufsize];
    m_filled = 0;
  }

  inline void flush() {
    for (int i = 0; i < m_filled; ++i) {
      long pos = m_buf[i];
      ++m_count[pos];
      if (!m_count[pos])
        m_excess.push_back(pos);
    }
    
    m_filled = 0;
  }

  inline void increment(long i) {
    m_buf[m_filled++] = i;
    
    if (m_filled == k_bufsize)
      flush();
  }

  ~buffered_gap_array() {
    delete[] m_count;
    delete[] m_buf;
  }
  
  // Store to file using v-byte encoding.  
  void save_to_file(std::string fname, long *iowrite) {
    flush();
    
    fprintf(stderr, "  gap->excess.size() = %lu\n", m_excess.size());
    fprintf(stderr, "  Saving gap to file: ");
    long double gap_save_start = utils::wallclock();
    std::sort(m_excess.begin(), m_excess.end());
    std::FILE *f = utils::open_file(fname.c_str(), "w");
    unsigned char *buffer = (unsigned char *)m_buf;
    m_filled = 0;
    
    long max_pos = m_excess.size();
    for (long j = 0, pos = 0; j < m_length; ++j) {
      long c = 0;
      while (pos < max_pos && m_excess[pos] == j) ++pos, ++c;
      long gap_j = m_count[j] + (c << 8);
      while (gap_j > 127) {
        buffer[m_filled++] = ((gap_j & 0x7f) | 0x80);
        gap_j >>= 7;
        if (m_filled == k_bufsize) {
          utils::add_objects_to_file<unsigned char>(buffer, m_filled, f, iowrite);
          m_filled = 0;
        }
      }
      buffer[m_filled++] = gap_j;
      if (m_filled == k_bufsize) {
        utils::add_objects_to_file<unsigned char>(buffer, m_filled, f, iowrite);
        m_filled = 0;
      }
    }
    if (m_filled)
      utils::add_objects_to_file<unsigned char>(buffer, m_filled, f, iowrite);

    std::fclose(f);
    fprintf(stderr, "%.2Lf\n", utils::wallclock() - gap_save_start);
  }

  static const int k_bufsize = (1 << 19);

  unsigned char *m_count;
  std::vector<long> m_excess;
  
  int m_filled;
  long m_length, *m_buf;
};

#endif // __GAP_ARRAY_H_INCLUDED

