#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Host.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/Parser.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Lex/PreprocessorOptions.h"
#include <iostream>

using namespace llvm;
using namespace clang;

static cl::opt<std::string>
filename(cl::Positional, cl::desc("Input file"), cl::Required);

int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv, "Cool");
  CompilerInstance ci;

  ci.createDiagnostics();

  //Set clang to target our machine (In my case, x86_64)
  std::shared_ptr<clang::TargetOptions> target_options = std::make_shared<clang
    ::TargetOptions>();
  target_options->Triple = sys::getDefaultTargetTriple();

  TargetInfo *target_info = TargetInfo::CreateTargetInfo(ci.getDiagnostics(),
                                                         target_options);
  ci.setTarget(target_info);

  //Create file manager
  ci.createFileManager();
  ci.createSourceManager(ci.getFileManager());

  //Create preprocessor
  ci.createPreprocessor(TU_Complete);
  ci.getPreprocessorOpts().UsePredefines = false;

  //Create AST consumer
  std::unique_ptr<ASTConsumer> ast_consumer = CreateASTPrinter(nullptr, "");
  ci.setASTConsumer(std::move(ast_consumer));
  ci.createASTContext();

  //Create Semantics manager
  ci.createSema(TU_Complete, nullptr);
  const FileEntry *file_entry = ci.getFileManager().getFile(filename);
  if (!file_entry) {
    llvm::errs() << "Could not load file\n";
    return -1;
  }

  //Load out file
  ci.getSourceManager().createFileID(file_entry, SourceLocation(),
                                     SrcMgr::CharacteristicKind::C_System);
  ci.getDiagnosticClient().BeginSourceFile(ci.getLangOpts(), nullptr);

  //Parse our ast
  ParseAST(ci.getSema());
  
  //Print AST statistics
  ci.getASTContext().PrintStats();
  ci.getASTContext().Idents.PrintStats();

  return 0;
}
