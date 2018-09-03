// Options:
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/Error.h"
#include <iostream>

//Expected<T> help from https://weliveindetail.github.io/blog/post/2017/09/06/llvm-expected-basics.html

using namespace llvm;

static cl::opt<std::string> FileName(cl::Positional, cl::desc("Bitcode file"), cl::Required);

int main(int argc, char** argv) {
  cl::ParseCommandLineOptions(argc, argv, "LLVM hello world\n");
  LLVMContext context;
  ErrorOr<std::unique_ptr<MemoryBuffer>> mb = MemoryBuffer::getFile(FileName);
  Expected<std::unique_ptr<Module>> expected_module = parseBitcodeFile(mb.get()->getMemBufferRef(), context);
  if (auto error = expected_module.takeError()) {
    logAllUnhandledErrors(std::move(error), llvm::errs(), "Error reading bitcode:");
    return -1;
  }
  std::unique_ptr<Module> m = std::move(expected_module.get());
  for (Module::const_iterator i = m->getFunctionList().begin(), 
    e = m->getFunctionList().end(); i != e; ++i) {
    if (!i->isDeclaration()) {
      llvm::outs() << i->getName() << " has " << i->size() << " basic block(s).\n";
    }
  }
  return 0;
}