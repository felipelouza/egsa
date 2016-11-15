#include <cstdio>
#include <cstdlib>
#include <getopt.h>

#include "sascan.h"
#include "utils.h"


char *program_name;

void usage(int status) {
  printf(
"Usage: %s [OPTION]... FILE\n"
"Construct the suffix array for text stored in FILE.\n"
"\n"
"Mandatory arguments to long options are mandatory for short options too.\n"
"  -m, --mem=LIMIT         limit RAM usage to LIMIT MiB (default: 3072)\n",
    program_name);

  std::exit(status);
}

int main(int argc, char **argv) {
  program_name = argv[0];

  static struct option long_options[] = {
    {"mem",    required_argument, NULL, 'm'},
    {"size",    required_argument, NULL, 's'},
    {NULL, 0, NULL, 0}
  };

  long ram_use = 3072L << 20;
  long size = 0; // 1024L << 20;

  // Parse command-line options.
  int c;
  while ((c = getopt_long(argc, argv, "m:", long_options, NULL)) != -1) {
    switch(c) {
      case 'm':
        ram_use = std::atol(optarg) << 20;
        if (ram_use <= 0L) {
          fprintf(stderr, "Error: invalid RAM limit (%ld)\n\n", ram_use);
          usage(EXIT_FAILURE);
        }
      case 's':
        size = std::atol(optarg) << 20;
        break;
      default:
        usage(EXIT_FAILURE);
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Error: FILE not provided\n\n");
    usage(EXIT_FAILURE);
  }

  // Parse the text filename.
  std::string text_fname = std::string(argv[optind++]);
  if (optind < argc) {
    fprintf(stderr, "Warning: multiple input files provided. "
    "Only the first will be processed.\n");
  }

  // Check if input exists.
  if (!utils::file_exists(text_fname)) {
    fprintf(stderr, "Error: input file (%s) does not exist\n\n",
        text_fname.c_str());
    usage(EXIT_FAILURE);
  }

  text_fname = utils::absolute_path(text_fname);
printf("size = %lu\n", size);
  SAscan(text_fname, ram_use, size);
}
