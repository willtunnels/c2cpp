#include "handlers.h"
#include "clang/AST/Expr.h"
#include "clang/Lex/Lexer.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceLocation.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>

using clang::tooling::Replacement;

namespace c2cpp {

const std::vector<std::string> cxxOnlyKeywords = {
    "alignas",
    "alignof",
    "and",
    "and_eq",
    "asm",
    "atomic_cancel",
    "atomic_commit",
    "atomic_noexcept",
    "bitand",
    "bitor",
    "bool",
    "catch",
    "char16_t ",
    "char32_t",
    "class",
    "compl",
    "concept",
    "constexpr",
    "const_cast",
    "co_await",
    "co_return ",
    "co_yield ",
    "decltype",
    "delete",
    "dynamic_cast",
    "explicit",
    "export",
    "false",
    "friend",
    "import",
    "module ",
    "mutable",
    "namespace",
    "new",
    "noexcept",
    "not",
    "not_eq",
    "nullptr",
    "operator",
    "or",
    "or_eq",
    "private",
    "protected",
    "public",
    "reflexpr",
    "reinterpret_cast",
    "requires",
    "static_assert",
    "static_cast",
    "synchronized",
    "template",
    "this",
    "thread_local",
    "throw",
    "true",
    "try",
    "typeid",
    "typename",
    "using",
    "virtual",
    "wchar_t",
    "xor",
    "xor_eq"
};

const std::vector<std::string> cOnlyKeywords {
    "restrict"
};

void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

std::string getSourceText(const clang::SourceManager& srcManager, clang::SourceLocation start,
        clang::SourceLocation end_, const clang::LangOptions& langOps = clang::LangOptions{}) { 
    auto end = clang::Lexer::getLocForEndOfToken(end_, 0, srcManager, langOps);
    return std::string{
        srcManager.getCharacterData(start),
        static_cast<std::size_t>(
                srcManager.getCharacterData(end) -
                srcManager.getCharacterData(start)
        ) };
}

void ImpVoidPtrCastHandler::run(const MatchFinder::MatchResult& result) {
    auto expr        = result.Nodes.getNodeAs<clang::Expr>("expr");
    auto bitCastType = result.Nodes.getNodeAs<clang::QualType>("bitCastType");
    
    if (expr && bitCastType) {
        auto srcManager = result.SourceManager;
        auto start      = expr->getBeginLoc();
        auto oldText    = getSourceText(*srcManager, start, expr->getEndLoc());
        auto castType   = bitCastType->getAsString();

        auto sstream = std::ostringstream{};
        sstream << "static_cast<" << castType << ">(" << oldText << ")";
        auto newText = sstream.str();

        auto rep = Replacement{ *srcManager, start, static_cast<unsigned>(oldText.length()), newText };
        if (replacements_[srcManager->getFilename(start)].add(rep)) {
            llvm::outs() << "ImpVoidPtrCastHandler.run: failed to add replacement\n";
        }
    }
}

void EnumDeclHandler::run(const MatchFinder::MatchResult& result) {
    if (auto decl = result.Nodes.getNodeAs<clang::EnumDecl>("decl")) {
        auto srcManager = result.SourceManager;
        auto start      = decl->getBeginLoc();
        auto oldText    = getSourceText(*srcManager, start, decl->getEndLoc());

        auto brace   = oldText.find('{');
        auto prefix  = oldText.substr(0, brace);
        auto suffix  = oldText.substr(brace, std::string::npos);

        auto sstream = std::ostringstream{};
        rtrim(prefix);
        sstream << prefix << " : int " << suffix;
        auto newText = sstream.str();

        auto rep = Replacement{ *srcManager, start, static_cast<unsigned>(oldText.length()), newText };
        if (replacements_[srcManager->getFilename(start)].add(rep)) {
            llvm::outs() << "EnumDeclHandler.run: failed to add replacement\n";
        }
    }
}

void CharLiteralHandler::run(const MatchFinder::MatchResult& result) {
    auto body     = result.Nodes.getNodeAs<clang::Stmt>("body");
    auto typeNode = result.Nodes.getNodeAs<clang::QualType>("qualType");

    if (body && typeNode) {
        auto srcManager = result.SourceManager;
        auto start      = body->getBeginLoc();
        auto oldText    = getSourceText(*srcManager, start, body->getEndLoc());
        auto type       = typeNode->getAsString();

        auto sstream = std::ostringstream{};
        sstream << '(' << type << ')';
        auto newText = sstream.str();
        
        auto rep = Replacement{ *srcManager, start, static_cast<unsigned>(oldText.length()), newText };
        if (replacements_[srcManager->getFilename(start)].add(rep)) {
            llvm::outs() << "CharLiteralHandler.run: failed to add replacement\n";
        }
    }
}

void InvalidNameHandler::run(const MatchFinder::MatchResult& result) {
    if (auto decl = result.Nodes.getNodeAs<clang::NamedDecl>("decl")) {
        auto srcManager = result.SourceManager;
        auto nameStr = decl->getNameAsString();
        for (const auto& keyword : cxxOnlyKeywords) {
            if (keyword.length() <= nameStr.length()) {
                auto diff = std::mismatch(keyword.begin(), keyword.end(), nameStr.begin());
                auto hasUnderscoreSuffix = std::accumulate(diff.second, nameStr.end(), true,
                        [](bool init, char c){ return init && c == '_'; });
                if (diff.first == keyword.end() || hasUnderscoreSuffix) {
//                    auto start = decl->getBeginLoc();
//                    auto rep   = Replacement{ *srcManager, decl->getEndLoc(), 0, "_" };
//                    llvm::outs() << getSourceText(*srcManager, start, decl->getEndLoc()) << '\n';
//                    if (replacements_[srcManager->getFilename(start)].add(rep)) {
//                        llvm::outs() << "InvalidNameHandler.run: failed to add replacement\n";
//                    }
                }
            }
        }
    }
}

} // c2cpp

