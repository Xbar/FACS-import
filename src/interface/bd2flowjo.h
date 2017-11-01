#ifndef BD2FLOWJO_INTERFACE_H
#define BD2FLOWJO_INTERFACE_H

extern "C"
{
  int bd2flowjo(const char* bd_file, const char* flowjo_file,
      const char* output_file);
}
#endif
