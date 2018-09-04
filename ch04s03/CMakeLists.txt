set(LLVM_LINK_COMPONENTS
  Support
  Core
  BitReader
  )

add_clang_executable(learning
  main.cpp
  )

target_link_libraries(learning
  PRIVATE
  
  libclang
  clangTooling
  clangToolingCore
  clangIndex
  clangMove
  clangFrontend
  clangFrontendTool
  clangDriver
  clangSerialization
  clangParse
  clangSema
  clangAnalysis
  clangEdit
  clangAST
  clangASTMatchers
  clangLex
  clangBasic
  )

