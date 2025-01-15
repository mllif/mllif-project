#pragma once
#include "Tree.h"

namespace mllif::mlir {

    class Type {
      public:
        virtual ~Type() = default;

        /**
         * @brief Stores this type into symbol tree if necessary and Returns reference string for this type
         * @param symbols A symbol tree
         * @return Reference string for this type
         */
        virtual std::string store(Tree &symbols) const = 0;
    };

} // namespace mllif::mlir
