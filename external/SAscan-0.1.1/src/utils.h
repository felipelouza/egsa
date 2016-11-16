#ifndef __UTILS_H_INCLUDED
#define __UTILS_H_INCLUDED

#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>


namespace utils {

#define STRX(x) #x
#define STR(x) STRX(x)


//================================ FILE I/O ==================================//

std::string absolute_path(std::string fname);
std::FILE *open_file(std::string filename, std::string mode);
long file_size(std::string filename, long size);
bool file_exists(std::string filename);
void file_delete(std::string filename);
void read_block(std::string filename, long beg, long length, unsigned char *b, long *ioread);

template<typename T>
void write_objects_to_file(T *tab, long length, std::string filename, long *iowrite) {
  std::FILE *f = open_file(filename, "w");
  size_t fwrite_ret = std::fwrite(tab, sizeof(T), length, f);
(*iowrite)+=sizeof(T)*length;
  if ((long)fwrite_ret != length) {
    fprintf(stderr, "Error: fwrite in line %s of %s returned %ld\n",
        STR(__LINE__), STR(__FILE__), fwrite_ret);
    std::exit(EXIT_FAILURE);
  }

  std::fclose(f);
}

template<typename T>
void add_objects_to_file(T *tab, long length, std::FILE *f, long *iowrite) {
  size_t fwrite_ret = std::fwrite(tab, sizeof(T), length, f);
(*iowrite)+=sizeof(T)*length;
  if ((long)fwrite_ret != length) {
    fprintf(stderr, "Error: fwrite in line %s of %s returned %lu\n",
        STR(__LINE__), STR(__FILE__), fwrite_ret);
    std::exit(EXIT_FAILURE);
  }
}

template<typename T>
void read_objects_from_file(T* &tab, long length, std::FILE *f, long *ioread) {
  size_t fread_ret = fread(tab, sizeof(T), length, f);
(*ioread)+=sizeof(T)*length;
  if ((long)fread_ret != length) {
    fprintf(stderr, "Error: fread in line %s of %s returned %ld\n",
        STR(__LINE__), STR(__FILE__), fread_ret);
    std::exit(EXIT_FAILURE);
  }
}

template<typename T>
void read_objects_from_file(T* &tab, long &length, std::string filename, long *ioread) {
  std::FILE *f = open_file(filename, "r");  

  std::fseek(f, 0L, SEEK_END);
  length = (long)(std::ftell(f) / sizeof(T));

  tab = new T[length];
  std::rewind(f);
  read_objects_from_file<T>(tab, length, f, ioread);

  std::fclose(f);
}

template<typename T>
void read_n_objects_from_file(T* &tab, long length, std::string filename, long *ioread) {
  tab = new T[length];

  std::FILE *f = open_file(filename, "r");
  read_objects_from_file<T>(tab, length, f, ioread);
  std::fclose(f);
}


//================================= MISC =====================================//

long log2ceil(long x);
void execute(std::string cmd);
long double wallclock();

template<typename int_type>
std::string intToStr(int_type x) {
  std::stringstream ss;
  ss << x;
  return ss.str();
}


} // namespace utils

#endif // __UTILS_H_INCLUDED
