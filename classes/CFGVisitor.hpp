#include <iostream>
#include <fstream>
#include <string>
#include <llvm/Support/CommandLine.h>
#include <clang/AST/AST.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Analysis/CFG.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <llvm/Support/raw_ostream.h>
#include <vector>

using namespace std;
using namespace clang;
using namespace clang::tooling;
using namespace llvm;

/**
 * This class visits CFGs, grabs the relevant data per CFG block, and prints out the CFG information in DOT format.
 */

class CFGVisitor {
public:
    CFGVisitor(ofstream& DotFile, ASTContext &Context, const string &FunctionName, const string &FunctionType, const string &FunctionParameters)
        : DotFile(DotFile), Context(Context), FunctionName(FunctionName), FunctionType(FunctionType), FunctionParameters(FunctionParameters) {}

    void VisitCFG(const CFG& cfg) {
        /**
         * Responsible for the main structure of the DOT graph
         */


        //Starting off the DOT graph
        DotFile << "digraph \"" << (FunctionName + ";" + FunctionType + ";" + FunctionParameters) << "\" {\n";
        DotFile << "  node [shape=rectangle];\n";

        // Visiting each block
        for (const CFGBlock *Block : cfg) {
            VisitBlock(Block, Context, DotFile, cfg); 
        }

        // Printing the edges
        for (const CFGBlock *Block : cfg) {
            for (CFGBlock::const_succ_iterator I = Block->succ_begin(); I != Block->succ_end(); ++I) {
                if (const CFGBlock *Succ = *I) {
                    DotFile << "  Block" << Block->getBlockID() << " -> Block" << Succ->getBlockID() << ";\n";

                    // for testing
                    cout << "  Block" << Block->getBlockID() << " -> Block" << Succ->getBlockID() << ";\n";
                }
            }
        }

        DotFile << "}\n";
    }

    void PrintStatement(const Stmt *S, ASTContext &Context, raw_string_ostream &Stream) {
    /**
     * Responsible for printing out the finer details of blocks that are not entry or exit blocks.
     */
    if (!S) return;

    SourceManager &SM = Context.getSourceManager();
    SourceLocation BeginLoc = S->getBeginLoc();
    StringRef lineString = "";
    if (BeginLoc.isValid()) {
        PresumedLoc PLoc = SM.getPresumedLoc(BeginLoc);
        if (PLoc.isValid()) {
            unsigned Line = PLoc.getLine();

            SourceLocation LineStart = SM.translateLineCol(SM.getFileID(BeginLoc), Line, 1);
            SourceLocation LineEnd = SM.translateLineCol(SM.getFileID(BeginLoc), Line, UINT_MAX);
            SourceRange LineRange(LineStart, LineEnd);

            Rewriter Rewriter;
            Rewriter.setSourceMgr(SM, Context.getLangOpts());
            lineString = Lexer::getSourceText(CharSourceRange::getTokenRange(LineRange), SM, Context.getLangOpts());
        }
    }

    if (isa<CallExpr>(S)) {
        Stream << "FUNCTION CALLED: ";
        S->printPretty(Stream, nullptr, PrintingPolicy(Context.getLangOpts()));
        Stream << "\\l";
    } else if (isa<DeclStmt>(S)) {
        Stream << "DECLARATION: ";
        S->printPretty(Stream, nullptr, PrintingPolicy(Context.getLangOpts()));
        Stream << "\\l";
    } else if (const ReturnStmt *RS = dyn_cast<ReturnStmt>(S)) {
        string ReturnStr;
        raw_string_ostream ReturnStream(ReturnStr);
        RS->printPretty(ReturnStream, nullptr, PrintingPolicy(Context.getLangOpts()));
        ReturnStream.flush();
        Stream << "  RETURNED.";
    } else {
        if (isa<BinaryOperator>(S)) {
            if (lineString.find("while") != llvm::StringRef::npos) {
                Stream << "WHILE LOOP CONDITION: ";
            } else if (lineString.find("for") != llvm::StringRef::npos) {
                Stream << "FOR LOOP CONDITION: ";
            } else if (lineString.find("if") != llvm::StringRef::npos) {
                Stream << "IF CONDITION: ";
            }
        }
        S->printPretty(Stream, nullptr, PrintingPolicy(Context.getLangOpts()));
        if (lineString.find("return") == llvm::StringRef::npos) {
                Stream << "\\l";
            }
    }

}

string escapeDoubleQuotes(const string& input) {
    /**
     * Escapes double quotes so front-end does not break
     */
    string output;

    for (char ch : input) {
        if (ch == '\"') {
            output += "\\\"";
        } else {
            output += ch;
        }
    }
    return output;
}

// Visiting blocks of the CFG built by clang
void VisitBlock(const CFGBlock *Block, ASTContext &Context, ofstream& DotFile, const CFG &cfg) {
    /**
     * Responsible for the text inside of the visualized blocks, specifically for entry, exit, and loop blocks.
     */
    string BlockLabel;
    raw_string_ostream BlockStream(BlockLabel);

    if (Block == &cfg.getEntry()) {
        // Labeling entry block
        BlockStream << "ENTRY POINT";
    } else if (Block == &cfg.getExit()) {
        // Labeling exit block
        BlockStream << "END";
    } else if (Block->empty()) {
        // While loops make empty blocks
        BlockStream << "LOOP";
    } else {
        for (const CFGElement &Element : *Block) {
            if (Element.getKind() == CFGElement::Statement) {
                const Stmt *S = Element.castAs<CFGStmt>().getStmt();
                PrintStatement(S, Context, BlockStream);
            }
        }
    }

    // Printing everything out in DOT format
    DotFile << "  Block" << Block->getBlockID() << " [label=\"" << escapeDoubleQuotes(BlockStream.str()) << "\\l\"];\n";
}

private:
    ofstream& DotFile;
    ASTContext &Context;
    string FunctionName;
    string FunctionType;
    string FunctionParameters;

};