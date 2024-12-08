#pragma once

namespace polyglat::ir {

    enum DiagnosticCode {
        DC_NameRequired,
        DC_NamespaceRequired,
        DC_TypeRequired,
        DC_ValueRequired,
        DC_CallConvRequired,
        DC_ReturnTypeRequired,
        DC_NoSuchAttribute,
        DC_NoSuchFunction,
        DC_NoSuchParameter,
        DC_NoSuchGlobalVariable,
        DC_InvalidInitializer,
        DC_UnrecognizedAttribute,
    };

    class PolyglatDiag : public llvm::DiagnosticInfo {
        DiagnosticCode Code;
        std::string Message;

      public:
        PolyglatDiag(
            const llvm::DiagnosticKind kind,
            const llvm::DiagnosticSeverity severity,
            const DiagnosticCode code,
            const std::string &message)
            : DiagnosticInfo(kind, severity), Code(code), Message(message) {}

        void print(llvm::DiagnosticPrinter &DP) const override;
    };

#define PG_DIAG_INIT(kind, severity, code, fmt, ...) PolyglatDiag(llvm::kind, llvm::severity, code, std::format(fmt __VA_OPT__(, ) __VA_ARGS__))
#define PG_ID(arg) arg
#define PG_PARENS ()
#define PG_EXPAND(...) PG_EXPAND_(__VA_ARGS__)
#define PG_EXPAND_(...) PG_EXPAND__(__VA_ARGS__)
#define PG_EXPAND__(...) PG_EXPAND___(__VA_ARGS__)
#define PG_EXPAND___(...) __VA_ARGS__
#define PG_ARGS(...) __VA_OPT__(PG_EXPAND(PG_ARGS_(__VA_ARGS__)))
#define PG_ARGS_(a, ...) const std::string& a __VA_OPT__(, PG_ARGS__ PG_PARENS(__VA_ARGS__))
#define PG_ARGS__() PG_ARGS_

#define PG_DIAG(name, kind, severity, code, fmt, ...)                 \
    class name final : public PolyglatDiag {                          \
      public:                                                         \
        name(PG_ARGS(__VA_ARGS__))                                    \
            : PG_DIAG_INIT(kind, severity, code, fmt, __VA_ARGS__) {} \
    }

    // Errors
    PG_DIAG(NameRequiredError, DK_MisExpect, DS_Error, DC_NameRequired, "name is required for {} '{}'", _for, name);
    PG_DIAG(NamespaceRequiredError, DK_MisExpect, DS_Error, DC_NamespaceRequired, "namespace is required for {} '{}'", _for, name);
    PG_DIAG(TypeRequiredError, DK_MisExpect, DS_Error, DC_TypeRequired, "type is required for {} '{}'", _for, name);
    PG_DIAG(ValueRequiredError, DK_MisExpect, DS_Error, DC_ValueRequired, "value is required for {} '{}'", _for, name);
    PG_DIAG(CallConvRequiredError, DK_MisExpect, DS_Error, DC_CallConvRequired, "calling convention attribute is required for {} '{}'", _for, name);
    PG_DIAG(ReturnTypeRequiredError, DK_MisExpect, DS_Error, DC_ReturnTypeRequired, "return type attribute is required for {} '{}'", _for, name);
    PG_DIAG(NoSuchAttributeError, DK_MisExpect, DS_Error, DC_NoSuchAttribute, "no such attribute '{}' for {} '{}'", attribute, _for, name);
    PG_DIAG(NoSuchFunctionError, DK_MisExpect, DS_Error, DC_NoSuchFunction, "no such function '{}'", fn);
    PG_DIAG(NoSuchParameterError, DK_MisExpect, DS_Error, DC_NoSuchParameter, "no such parameter '{}' for {} '{}'", var, _for, name);
    PG_DIAG(NoSuchGlobalVariableError, DK_MisExpect, DS_Error, DC_NoSuchGlobalVariable, "no such global variable '{}'", var);
    PG_DIAG(InvalidInitializerError, DK_MisExpect, DS_Error, DC_InvalidInitializer, "invalid initializer for {} '{}'", _for, target);
    // Warnings
    PG_DIAG(UnrecognizedAttributeError, DK_MisExpect, DS_Warning, DC_UnrecognizedAttribute, "unrecognized attribute '{}' of {} '{}'", key, _of, name);

} // namespace polyglat::ir
