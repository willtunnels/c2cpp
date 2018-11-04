#ifndef C2CPP_MATCHERS_H
#define C2CPP_MATCHERS_H
#include "clang/ASTMatchers/ASTMatchers.h"

namespace c2cpp {

extern const clang::ast_matchers::StatementMatcher   impVoidPtrCastMatcher;
extern const clang::ast_matchers::DeclarationMatcher enumDeclMatcher;
extern const clang::ast_matchers::StatementMatcher   charLiteralMatcher;
extern const clang::ast_matchers::DeclarationMatcher invalidNameMatcher;

} // c2cpp

#endif // C2CPP_MATCHERS_H
