#pragma once
                
#include <cstdint>
                
namespace mllif {
  [[mllif::export]]
#if _WIN32
  int init(const wchar_t *runtimeConfigPath, const wchar_t *assemblyPath);
#else
  int init(const char *runtimeConfigPath, const char *assemblyPath);
#endif
  [[mllif::export]]
  void close();
}
