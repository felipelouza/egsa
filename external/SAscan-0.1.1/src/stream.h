#ifndef __STREAM_H_INCLUDED
#define __STREAM_H_INCLUDED

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>

#include "utils.h"


//==============================================================================
// Usage:
//
// stream_reader<int> *sr = new stream_reader<int>("input.txt", 1 << 22);
// while (!sr->empty()) {
//   int next = sr->read();
//   fprintf("%d\n", next);
// }
// delete sr;
//==============================================================================
template<typename T>
struct stream_reader {
  stream_reader(std::string fname, long buf_bytes = (4L << 20))
      : m_bufelems((buf_bytes + sizeof(T) - 1) / sizeof(T)) {
    m_file = utils::open_file(fname, "r");
    m_buffer = new T[m_bufelems];
    refill();
  }

  ~stream_reader() {
    delete[] m_buffer;
    std::fclose(m_file);
  }

  inline T read() {
    T ret = m_buffer[m_pos++];
    if (m_pos == m_filled)
      refill();

    return ret;
  }

  inline bool empty() {
    return (!m_filled && !refill());
  }
  
private:
  long refill() {
    m_filled = std::fread(m_buffer, sizeof(T), m_bufelems, m_file);
    m_pos = 0;

    return m_filled;
  }

  long m_bufelems, m_filled, m_pos;
  T *m_buffer;

  std::FILE *m_file;
};


template<typename T>
struct backward_stream_reader {
  backward_stream_reader(std::string fname, long buf_bytes = (4L << 20))
      : m_bufelems((buf_bytes + sizeof(T) - 1) / sizeof(T)), m_filled(0L) {
    m_buffer = new T[m_bufelems];
    m_file = utils::open_file(fname, "r");    
    std::fseek(m_file, 0L, SEEK_END);
    refill();
  }

  ~backward_stream_reader() {
    delete[] m_buffer;
    std::fclose(m_file);
  }

  inline T read() {
    T ret = m_buffer[m_pos--];
    if (m_pos < 0L) refill();
    
    return ret;
  }
  
private:
  void refill() {
    long curpos = std::ftell(m_file) / sizeof(T);
    long toread = std::min(m_bufelems, curpos - m_filled);

    std::fseek(m_file, -(m_filled + toread), SEEK_CUR);
    m_filled = std::fread(m_buffer, sizeof(T), toread, m_file);
    m_pos = m_filled - 1L;
  }

  long m_bufelems, m_filled, m_pos;
  T *m_buffer;

  std::FILE *m_file;
};


//==============================================================================
// Usage:
//
// stream_writer<int> *sw = new stream_writer<int>("output.txt", 1 << 22);
// for (int i = 0; i < n; ++i)
//   sw->write(SA[i]);
// delete sw;
//==============================================================================
template<typename T>
struct stream_writer {
  stream_writer(std::string fname, long bufsize)
      : m_bufelems((bufsize + sizeof(T) - 1) / sizeof(T)) {
    m_file = utils::open_file(fname.c_str(), "w");
    m_buffer = new T[m_bufelems];
    m_filled = 0;
  }

  inline void flush(long *iowrite) {
    utils::add_objects_to_file(m_buffer, m_filled, m_file, iowrite);
    m_filled = 0;
  }

  void write(T x, long *iowrite) {
    m_buffer[m_filled++] = x;

    if (m_filled == m_bufelems)
      flush(iowrite);
  }

  ~stream_writer() {
long iowrite=0;
    if (m_filled)
      flush(&iowrite);

    delete[] m_buffer;
    std::fclose(m_file);
  }

  int get_m_filled(){
    return m_filled;
  }


private:
  long m_bufelems, m_filled;
  T *m_buffer;

  std::FILE *m_file;
};


struct bit_stream_reader {
  bit_stream_reader(std::string filename) {
    m_file = utils::open_file(filename.c_str(), "r");
    m_buf = new unsigned char[k_bufsize];
    refill();
  }

  inline bool read() {
    bool ret = m_buf[m_pos_byte] & (1 << m_pos_bit);
    ++m_pos_bit;
    if (m_pos_bit == 8) {
      m_pos_bit = 0;
      ++m_pos_byte;
      if (m_pos_byte == m_filled)
        refill();
    }

    return ret;
  }

  ~bit_stream_reader() {
    delete[] m_buf;
    std::fclose(m_file);
  }

  int get_m_filled(){
    return m_filled;
  }

private:
  inline void refill() {
    m_filled = fread(m_buf, 1, k_bufsize, m_file);
    m_pos_byte = m_pos_bit = 0;
  }

  static const int k_bufsize = (2 << 20);

  std::FILE *m_file;

  unsigned char *m_buf;
  int m_filled, m_pos_byte, m_pos_bit;
};


struct bit_stream_writer {
  bit_stream_writer(std::string filename) {
    f = utils::open_file(filename, "w");
    buf = new unsigned char[bufsize];
    if (!buf) {
      fprintf(stderr, "Error: allocation error in bit_stream_writer\n");
      std::exit(EXIT_FAILURE);
    }
    std::fill(buf, buf + bufsize, 0);
    filled = pos_bit = 0;
  }

  inline void flush(long *iowrite) {
    if (pos_bit) ++filled; // final flush?
    utils::add_objects_to_file<unsigned char>(buf, filled, f, iowrite);
    filled = pos_bit = 0;
    std::fill(buf, buf + bufsize, 0);
  }

  void write(int bit, long *iowrite) {
    buf[filled] |= (bit << pos_bit);
    ++pos_bit;
    if (pos_bit == 8) {
      pos_bit = 0;
      ++filled;
      if (filled == bufsize)
        flush(iowrite);
    }
  }
  
  ~bit_stream_writer() {
long iowrite=0;
    flush(&iowrite);
    fclose(f);
    delete[] buf;
  }

  int get_filled(){
    return filled;
  }

private:
  static const int bufsize = (2 << 20);
  
  unsigned char *buf;
  int filled, pos_bit;

  std::FILE *f;
};


struct vbyte_stream_reader {
  vbyte_stream_reader(std::string fname, long bufsize)
      : m_bufsize(bufsize) {
    m_file = utils::open_file(fname, "r");
    m_buf = new unsigned char[m_bufsize];
    refill();
  }

  inline long read() {
    long ret = 0, offset = 0;
    while (m_buf[m_pos] & 0x80) {
      ret |= ((m_buf[m_pos++] & 0x7f) << offset);
      if (m_pos == m_filled)
        refill();
      offset += 7;
    }
    ret |= (m_buf[m_pos++] << offset);
    if (m_pos == m_filled)
      refill();

    return ret;
  }
  
  ~vbyte_stream_reader() {
    delete[] m_buf;
    std::fclose(m_file);
  }
  
private:
  inline void refill() {
    m_filled = std::fread(m_buf, 1, m_bufsize, m_file);
    m_pos = 0;
  }

  long m_bufsize, m_filled, m_pos;
  unsigned char *m_buf;
  
  std::FILE *m_file;
};

#endif // __STREAM_H_INCLUDED
