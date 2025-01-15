#pragma once

#include <mllif/Frontend/MLIR/Adapter.h>

namespace mllif::mlir::cir {

    class CIRAdapter final : public Adapter {
      public:
        void handle(Tree &symbols, std::shared_ptr<::mlir::ModuleOp> module, ::mlir::Operation *op) override;
    };

}
