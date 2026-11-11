include(FetchContent)

FetchContent_Declare(
    fast_float
    GIT_REPOSITORY https://github.com/fastfloat/fast_float.git
    GIT_TAG tags/v8.3.0
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(fast_float)

find_package(LLVM REQUIRED CONFIG)