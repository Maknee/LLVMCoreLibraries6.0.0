#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/Error.h"

using namespace llvm;

//Require a bitcode file input
static cl::opt<std::string> file_name(cl::Positional, cl::desc("Bitcode file"), cl::Required);

int main(int argc, char* argv[])
{
  //Parse the command line input
  cl::ParseCommandLineOptions(argc, argv, "LLVM Hello");
  LLVMContext llvm_context;

  //Read the file that was input
  ErrorOr<std::unique_ptr<MemoryBuffer>> memory_buffer = MemoryBuffer::getFile(file_name);
  if(auto ec = memory_buffer.getError()) 
  {
    llvm::errs() << "[MemoryBuffer Error] " << ec.message() << "\n";
    return -1;
  }

  //Parse the input file as a bitcode file
  Expected<std::unique_ptr<Module>> module = parseBitcodeFile(memory_buffer->get()->getMemBufferRef(), llvm_context);
  if(auto error = module.takeError()) {
    llvm::errs() << errorToErrorCode(std::move(error)).message() << "\n";
    return -1;
  }

  //Iterate through all basic blocks and print out the basic block size
  const Module::FunctionListType& basic_blocks = module->get()->getFunctionList();
  for (const Function& basic_block : basic_blocks) {
    if(!basic_block.isDeclaration()) {
      llvm::outs() << basic_block.getName() << " has " << basic_block.size() << " basic blocks\n";

      for (const Argument& arg : basic_block.args()) {
        //print argument number, argument name, argument type
        llvm::outs() << "  " << arg.getArgNo() << ": " << arg.getName() << " - Type: ";
        arg.getType()->print(llvm::outs()); 
        llvm::outs() << "\n";
      }
    }
  }

  return 0;
}