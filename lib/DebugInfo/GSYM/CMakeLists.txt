add_llvm_library(LLVMDebugInfoGSYM
  FileWriter.cpp
  FunctionInfo.cpp
  InlineInfo.cpp
  LineTable.cpp
  Range.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/DebugInfo/GSYM
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/DebugInfo
  )
