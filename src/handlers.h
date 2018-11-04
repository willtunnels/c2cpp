#ifndef C2CPP_HANDLERS_H
#define C2CPP_HANDLERS_H
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Core/Replacement.h"
#include <map>
#include <set>
#include <string>

namespace c2cpp {

using MatchFinder     = clang::ast_matchers::MatchFinder;
using ReplacementsMap = std::map<std::string, clang::tooling::Replacements>;

class ImpVoidPtrCastHandler : public MatchFinder::MatchCallback {
public:
    ImpVoidPtrCastHandler(ReplacementsMap& replacements)
        : replacements_(replacements) {}
    virtual void run(const MatchFinder::MatchResult& result);
private:
    ReplacementsMap& replacements_;
};

class EnumDeclHandler : public MatchFinder::MatchCallback {
public:
    EnumDeclHandler(ReplacementsMap& replacements)
        : replacements_(replacements) {}
    virtual void run(const MatchFinder::MatchResult& result);
private:
    ReplacementsMap& replacements_;
};

class CharLiteralHandler : public MatchFinder::MatchCallback {
public:
    CharLiteralHandler(ReplacementsMap& replacements)
        : replacements_(replacements) {}
    virtual void run(const MatchFinder::MatchResult& result);
private:
    ReplacementsMap& replacements_;
};

class InvalidNameHandler : public MatchFinder::MatchCallback {
public:
    virtual void run(const MatchFinder::MatchResult& result);
};

} // c2cpp

#endif // C2CPP_HANDLERS_H
