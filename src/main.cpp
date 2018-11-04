#include "handlers.h"
#include "matchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Rewrite/Core/Rewriter.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace llvm;

int main(int argc, const char **argv) {
    auto category = cl::OptionCategory{ "c2cpp" };
    auto op = CommonOptionsParser{ argc, argv, category };
    auto tool = RefactoringTool{ op.getCompilations(), op.getSourcePathList() };

    // declare handlers
    auto impVoidPtrCastHandler = c2cpp::ImpVoidPtrCastHandler{ tool.getReplacements() };
    auto enumDeclHandler = c2cpp::EnumDeclHandler{ tool.getReplacements() };
    auto charLiteralHandler = c2cpp::CharLiteralHandler{ tool.getReplacements() };
//    auto invalidNameHandler = c2cpp::InvalidNameHandler{ tool.getReplacements() };

    // add matchers/handlers to finder
    auto finder = MatchFinder{};
    finder.addMatcher(c2cpp::impVoidPtrCastMatcher, &impVoidPtrCastHandler);
    finder.addMatcher(c2cpp::enumDeclMatcher, &enumDeclHandler);
    finder.addMatcher(c2cpp::charLiteralMatcher, &charLiteralHandler);
//    finder.addMatcher(c2cpp::invalidNameMatcher, &invalidNameHandler);

    if (int err = tool.run(newFrontendActionFactory(&finder).get())){
        return err;
    }

    // print out replacements
    for (const auto& e : tool.getReplacements()) {
        for (const auto& r : e.second) {
            llvm::outs() << r.toString() << '\n';
        }
    }
    llvm::outs() << '\n';

    auto diagOp = IntrusiveRefCntPtr<DiagnosticOptions>{ new DiagnosticOptions{} };
    DiagnosticsEngine diagEng{
        IntrusiveRefCntPtr<DiagnosticIDs>{ new DiagnosticIDs{} }, diagOp.get(),
        new TextDiagnosticPrinter{ llvm::errs(), diagOp.get() }, true
    };
    SourceManager srcManager{ diagEng, tool.getFiles() };
    
    auto rewriter = Rewriter{ srcManager, LangOptions{} };
    tool.applyAllReplacements(rewriter);

    // print out rewritten buffers
    for (auto begin = rewriter.buffer_begin(),
              end   = rewriter.buffer_end();
              begin != end; ++begin) {
        const auto entry = srcManager.getFileEntryForID(begin->first);
        llvm::outs() << "Rewrite buffer for file: " << entry->getName() << '\n';
        begin->second.write(llvm::outs());
    }

    return 0;
}

