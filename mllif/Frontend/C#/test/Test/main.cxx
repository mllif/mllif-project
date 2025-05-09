
#include <iostream>
#include <filesystem>
#include <library.h>

int main() {
  auto runtimeConfig = std::filesystem::canonical("../bin/Debug/net8.0/MLLIFCSharpFrontTest.runtimeconfig.json");
  auto assembly = std::filesystem::canonical("../bin/Debug/net8.0/MLLIFCSharpFrontTest.dll");

  auto v = mllif::init(runtimeConfig.c_str(), assembly.c_str());
  if (v) {
    std::cout << "0x" << std::hex << v << std::dec << '\n';
    return -1;
  }

  auto program = MLLIFCSharpFrontTest::Program::New();
  auto value = program.Hello(1, 2);
  std::cout << "returns: " << value << '\n';

  std::cout << ".NET initialized";

  mllif::close();
}
