#include "library.h"

#include <cstdlib>
#include <coreclr_delegates.h>
#include <hostfxr.h>
#include <nethost.h>

#if _WIN32
#  include <libloaderapi.h>
#  define c_dlopen LoadLibraryW
#  define c_dlclose FreeLibrary
#  define c_dlsym GetProcAddress
#else
#  include <dlfcn.h>
#  define c_dlopen(f) dlopen(f, RTLD_LAZY | RTLD_LOCAL)
#  define c_dlclose dlclose
#  define c_dlsym dlsym
#endif

static get_function_pointer_fn s_get_function_pointer;
static void *s_dl;
static hostfxr_handle s_hostfxr;

int mllif::init(const char_t *runtimeConfigPath, const char_t *assemblyPath) {
  static char_t buffer[8192];
  int r = 0;

  size_t size = sizeof buffer;
  if (r = get_hostfxr_path(buffer, &size, nullptr)) {
    return r;
  }

  s_dl = c_dlopen(buffer);

  auto initr = reinterpret_cast<hostfxr_initialize_for_runtime_config_fn>(c_dlsym(s_dl, "hostfxr_initialize_for_runtime_config"));
  if (!initr) {
    return -1;
  }

  auto get_delegate = reinterpret_cast<hostfxr_get_runtime_delegate_fn>(c_dlsym(s_dl, "hostfxr_get_runtime_delegate"));
  if (!get_delegate) {
    return -1;
  }

  if (r = initr(runtimeConfigPath, nullptr, &s_hostfxr)) {
    return r;
  }

  load_assembly_fn load_assembly = nullptr;
  if (r = get_delegate(s_hostfxr, hdt_load_assembly, (void **)&load_assembly)) {
    return r;
  }
  if (r = load_assembly(assemblyPath, nullptr, nullptr)) {
    return r;
  }
  if (r = get_delegate(s_hostfxr, hdt_get_function_pointer, (void **)&s_get_function_pointer)) {
    return r;
  }

  return 0;
}

void mllif::close() {
  //hostfxr_close(s_hostfxr);
  c_dlclose(s_dl);
}
