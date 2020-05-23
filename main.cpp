#include <iostream>
#include <sstream>

#include <llvm/Support/CommandLine.h>

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Frontend/CompilerInstance.h>

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

class CastCallBack : public MatchFinder::MatchCallback {
    Rewriter& _rewriter;
public:
    CastCallBack(Rewriter& rewriter) : _rewriter(rewriter) {};

    virtual void run(const MatchFinder::MatchResult &Result) {
        const auto* CastExpr = Result.Nodes.getNodeAs<CStyleCastExpr>("cast");

        if(CastExpr->getExprLoc().isMacroID()) {
            return;
        }

        if(CastExpr->getCastKind() == CK_ToVoid) {
            return;
        }

        auto CharRange = CharSourceRange::getCharRange(
            CastExpr->getLParenLoc(), CastExpr->getSubExprAsWritten()->getBeginLoc()
        );

        StringRef DataType = Lexer::getSourceText(CharSourceRange::getTokenRange(
            CastExpr->getLParenLoc().getLocWithOffset(1),
            CastExpr->getRParenLoc().getLocWithOffset(-1)),
            *Result.SourceManager, Result.Context->getLangOpts()
        );

        std::string CastType(("static_cast<" + DataType + ">").str());

        const auto *SubExpr = CastExpr->getSubExprAsWritten()->IgnoreImpCasts();
        if (!isa<ParenExpr>(SubExpr)) {
            CastType.push_back('(');
            _rewriter.InsertText(Lexer::getLocForEndOfToken(SubExpr->getEndLoc(),
                0, *Result.SourceManager, Result.Context->getLangOpts()), ")");
        }
        _rewriter.ReplaceText(CharRange, CastType);
    }
};

class MyASTConsumer : public ASTConsumer {
public:
    MyASTConsumer(Rewriter &rewriter) : callback_(rewriter) {
        matcher_.addMatcher(
                cStyleCastExpr(unless(isExpansionInSystemHeader())).bind("cast"), &callback_);
    }

    void HandleTranslationUnit(ASTContext &Context) override {
        matcher_.matchAST(Context);
    }

private:
    CastCallBack callback_;
    MatchFinder matcher_;
};

class CStyleCheckerFrontendAction : public ASTFrontendAction {
public:
    CStyleCheckerFrontendAction() = default;
    void EndSourceFileAction() override {
        rewriter_.getEditBuffer(rewriter_.getSourceMgr().getMainFileID())
            .write(llvm::outs());
    }

    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef /* file */) override {
        rewriter_.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
        return std::make_unique<MyASTConsumer>(rewriter_);
    }

private:
    Rewriter rewriter_;
};

static llvm::cl::OptionCategory CastMatcherCategory("cast-matcher options");

int main(int argc, const char **argv) {
    CommonOptionsParser OptionsParser(argc, argv, CastMatcherCategory);
    ClangTool Tool(OptionsParser.getCompilations(),
            OptionsParser.getSourcePathList());

    return Tool.run(newFrontendActionFactory<CStyleCheckerFrontendAction>().get());
}
