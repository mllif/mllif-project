#pragma once

#include <memory>
#include <mllif/Frontend/MLIR/Type.h>

namespace mlir {
    class Type;
}

namespace mllif::mlir::builtin {

    class BuiltinType : public Type {
      public:
        static auto From(const ::mlir::Type &type, std::shared_ptr<::mlir::ModuleOp> module) -> std::shared_ptr<BuiltinType>;
    };

    class BuiltinIntType final : public BuiltinType {
        size_t _width;
        bool _signed;

      public:
        BuiltinIntType(size_t width, bool _signed) : _width(width), _signed(_signed) {}

        size_t &width() { return _width; }
        size_t width() const { return _width; }
        bool& signed_() { return _signed; }
        bool signed_() const { return _signed; }

        std::string store(Tree &symbols) const override;
    };

    class BuiltinFPType final : public BuiltinType {
        size_t _width;

      public:
        BuiltinFPType(size_t width) : _width(width) {}

        size_t &width() { return _width; }
        size_t width() const { return _width; }

        std::string store(Tree &symbols) const override;
    };

} // namespace mllif::mlir::builtin
