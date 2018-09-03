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
  clangAST
  clangBasic
  clangFrontend
  clangLex
  clangTooling
  )

