#include "polyglat-ir/diagnostics.h"

void polyglat::ir::PolyglatDiag::print(llvm::DiagnosticPrinter &DP) const {
    DP << llvm::StringRef(Message) << '\n';
}
