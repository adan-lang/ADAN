include(FetchContent)

FetchContent_Declare(
    fast_float
    GIT_REPOSITORY https://github.com/fastfloat/fast_float.git
    GIT_TAG tags/v8.3.0
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(fast_float)

find_package(LLVM REQUIRED CONFIG)
if(LLVM_PACKAGE_VERSION VERSION_LESS "23.0")
    message(FATAL_ERROR
        "ADAN requires LLVM 23.0 or newer; found LLVM ${LLVM_PACKAGE_VERSION}. "
        "Set LLVM_DIR to an LLVM 23+ installation.")
endif()