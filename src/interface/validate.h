#ifndef VALIDATE_XML_INTERFACE_H
#define VALIDATE_XML_INTERFACE_H

extern "C"
{
  int validate_bd(const char* bd_file);

  int validate_flowjo(const char* flowjo_file);
}
#endif
