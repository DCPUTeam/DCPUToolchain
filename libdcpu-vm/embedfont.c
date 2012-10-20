/* This is a small tool to convert binary files to C files.
 * Sourced from http://stackoverflow.com/questions/11813271/embed-resources-eg-shader-code-images-into-executable-library-with-cmake */

#include <stdlib.h>
#include <stdio.h>
#include <bfile.h>
#ifdef WIN32
#define snprintf _snprintf
#endif

BFILE* bopen_or_exit(const char* fname, const char* mode)
{
  BFILE* f = bfopen(fname, mode);
  if (f == NULL) {
    perror(fname);
    exit(EXIT_FAILURE);
  }
  return f;
}

FILE* fopen_or_exit(const char* fname, const char* mode)
{
  FILE* f = fopen(fname, mode);
  if (f == NULL) {
    perror(fname);
    exit(EXIT_FAILURE);
  }
  return f;
}

int main(int argc, char** argv)
{
  BFILE* in;
  FILE* out;
  unsigned char buf[256];
  size_t nread = 0;
  size_t linecount = 0;
  size_t i;

  if (argc < 3) {
    fprintf(stderr, "USAGE: %s {sym} {rsrc}\n\n"
	"  Creates {sym}.c from the contents of {rsrc}\n",
	argv[0]);
    return EXIT_FAILURE;
  }

  out = fopen_or_exit(argv[1],"w");
  in = bopen_or_exit(argv[2], "rb");

  fprintf(out, "#include <stdlib.h>\n");
  fprintf(out, "const char vm_hw_lem1802_font[] = {\n");

  do {
    nread = bfread(buf, 1, sizeof(buf), in);
    for (i=0; i < nread; i++) {
      fprintf(out, "0x%02x, ", buf[i]);
      if (++linecount == 10) { fprintf(out, "\n"); linecount = 0; }
    }
  } while (!bfeof(in));
  if (linecount > 0) fprintf(out, "\n");
  fprintf(out, "};\n");
  fprintf(out, "const size_t vm_hw_lem1802_font_len = sizeof(vm_hw_lem1802_font);\n\n");

  bfclose(in);
  fclose(out);

  return EXIT_SUCCESS;
}
