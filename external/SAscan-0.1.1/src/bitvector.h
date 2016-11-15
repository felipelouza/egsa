#ifndef __BITVECTOR_H_INCLUDED
#define __BITVECTOR_H_INCLUDED

#include <cstdio>
#include <cstdlib>

#include "utils.h"


struct bitvector {
  bitvector(std::string filename) {
    utils::read_objects_from_file<unsigned char>(m_data, m_alloc_bytes, filename);
  }

  bitvector(long length) : m_alloc_bytes((length + 7) / 8) {
    if (length <= 0) {
      fprintf(stderr, "Error: constructing a bitvector of length 0.\n");
      std::exit(EXIT_FAILURE);
    }

    m_data = new unsigned char[m_alloc_bytes];
    std::fill(m_data, m_data + m_alloc_bytes, 0);
  }

  inline bool get(long i) const {
    return m_data[i >> 3] & (1 << (i & 7));
  }

  inline void set(long i) {
    m_data[i >> 3] |= (1 << (i & 7));
  }

  inline void reset(long i) {
    m_data[i >> 3] &= (~(1 << (i & 7)));
  }

  void save(std::string filename) const {
    utils::write_objects_to_file<unsigned char>(m_data, m_alloc_bytes, filename);
  }

  ~bitvector() {
    delete[] m_data;
  }

  long m_alloc_bytes;
  unsigned char *m_data;
};

#endif // __BITVECTOR_H_INCLUDED

