set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 10.0)
set(CMAKE_SYSTEM_PROCESSOR x64) #i686

set(triple x64-pc-win32)

# set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_C_COMPILER_TARGET ${triple})
# set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})
