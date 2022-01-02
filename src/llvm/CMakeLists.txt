set(LLVM_DIR "/usr" CACHE PATH "Path to LLVM installation")
set(LLVM_INCLUDE_DIR "${LLVM_DIR}/include" CACHE PATH "Path to LLVM includes")
set(LLVM_LIBRARY_DIR "${LLVM_DIR}/lib" CACHE PATH "Path to LLVM libraries")

find_library(CLANG clang HINTS ${LLVM_LIBRARY_DIR} REQUIRED)
find_library(CLANGPP clang-cpp HINTS ${LLVM_LIBRARY_DIR} REQUIRED)
find_library(LLVM LLVM HINTS ${LLVM_LIBRARY_DIR} REQUIRED)

add_executable(llvm-stepX
  main.cpp)
target_include_directories(llvm-stepX PRIVATE ${LLVM_INCLUDE_DIR})
target_link_libraries(llvm-stepX PRIVATE mal_defs ${CLANG} ${CLANGPP} ${LLVM})

mal_step_test(llvm-step2 step2_eval llvm-stepX)
#mal_perf(llvm llvm-stepX)

