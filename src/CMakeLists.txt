set(LLVM_LINK_COMPONENTS support)

add_clang_executable(c2cpp
    matchers.cpp
    handlers.cpp
    main.cpp
)

target_link_libraries(c2cpp
    PRIVATE
    clangTooling
    clangBasic
    clangASTMatchers
)
