#include "matchers.h"
#include "clang/AST/OperationKinds.h"
#include <string>
#include <vector>

using namespace clang::ast_matchers;

namespace c2cpp {

const StatementMatcher impVoidPtrCastMatcher =
    expr(anyOf(
        declRefExpr(
            hasParent(
                implicitCastExpr(
                    hasCastKind(clang::CK_LValueToRValue),
                    hasParent(implicitCastExpr(
                            hasCastKind(clang::CK_BitCast),
                            hasType(qualType().bind("bitCastType"))
                    )),
                    hasType(pointerType(pointee(voidType())))
                )
            )
        ),
        callExpr(
            hasParent(implicitCastExpr(
                    hasCastKind(clang::CK_BitCast),
                    hasType(qualType().bind("bitCastType"))
            )),
            hasType(pointerType(pointee(voidType())))
        )
    )).bind("expr");

const DeclarationMatcher enumDeclMatcher =
    enumDecl().bind("decl");

const StatementMatcher charLiteralMatcher =
    stmt(anyOf(
        sizeOfExpr(
            hasDescendant(parenExpr(
                hasDescendant(characterLiteral()),
                hasType(qualType().bind("qualType"))
            ).bind("body"))
        ),
        sizeOfExpr(hasDescendant(
            characterLiteral(
                hasType(qualType().bind("qualType"))
            ).bind("body")
        ))
    ));

const DeclarationMatcher invalidNameMatcher =
    namedDecl().bind("decl");

} // c2cpp

