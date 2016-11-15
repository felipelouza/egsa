#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>

#include <string>
#include <fstream>
#include <algorithm>

#include "utils.h"

namespace utils {


//================================== I/O =====================================//

std::string absolute_path(std::string fname) {
  char path[1 << 18];
  bool created = false;

  if (!file_exists(fname)) {
    // We need to create the file, since realpath fails on non-existing files.
    std::fclose(open_file(fname, "w"));
    created = true;
  }
  if (!realpath(fname.c_str(), path)) {
    fprintf(stderr, "Error: realpath failed for %s\n", fname.c_str());
    std::exit(EXIT_FAILURE);
  }

  if (created)
    file_delete(fname);

  return std::string(path);
}

std::FILE *open_file(std::string filename, std::string mode) {
  std::FILE *f = std::fopen(filename.c_str(), mode.c_str());
  if (f == NULL) {
    std::perror(filename.c_str());
    std::exit(EXIT_FAILURE);
  }

  return f;
}

bool file_exists(std::string filename) {
  std::FILE *f = std::fopen(filename.c_str(), "r");
  bool ret = (f != NULL);
  if (f != NULL)
    std::fclose(f);

  return ret;
}

long file_size(std::string filename, long size) {
  std::FILE *f = open_file(filename, "rt");
  std::fseek(f, 0L, SEEK_END);

  long size_file = std::ftell(f);

  if(size==0 || size_file < size) size = size_file;


  std::fclose(f);

  return size;
}

void file_delete(std::string filename) {
  int res = std::remove(filename.c_str());
  if (res) {
    fprintf(stderr, "Failed to delete %s: %s\n",
        filename.c_str(), strerror(errno));
    std::exit(EXIT_FAILURE);
  }
}

void read_block(std::string filename, long beg, long length, unsigned char *b) {
  std::FILE *f = open_file(filename, "r");
  std::fseek(f, beg, SEEK_SET);
  read_objects_from_file<unsigned char>(b, length, f);
  std::fclose(f);
}


//================================= MISC =====================================//

void execute(std::string cmd) {
  int system_ret = system(cmd.c_str());
  if (system_ret) {
    fprintf(stderr, "Error: executing command [%s] returned %d.\n",
        cmd.c_str(), system_ret);
    std::exit(EXIT_FAILURE);
  }
}


long log2ceil(long x) {
  long pow2 = 1;
  long w = 0;

  while (pow2 < x) {
    pow2 <<= 1;
    ++w;
  }

  return w;
}

long double wallclock() {
  timeval tim;
  gettimeofday(&tim, NULL);

  return tim.tv_sec + (tim.tv_usec / 1000000.0L);
}


} // namespace utils
