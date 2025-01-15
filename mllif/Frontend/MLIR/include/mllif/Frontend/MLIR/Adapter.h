#pragma once

#include <mllif/Frontend/MLIR/Tree.h>
#include <mlir/IR/DialectRegistry.h>
#include <mlir/IR/Operation.h>

namespace mllif::mlir {

    class Adapter {
      public:
        virtual ~Adapter() = default;

        virtual void handle(Tree& symbols, ::mlir::Operation* op) = 0;
    };

    extern std::vector<std::shared_ptr<Adapter>> Adapters;

    template <typename T>
    class AdapterRegistry {
      public:
        AdapterRegistry() {
            Adapters.push_back(std::make_shared<T>());
        }
    };

} // namespace mllif::mlir
