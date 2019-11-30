function(CheckTypeTraitsCompiles)
    set(CMAKE_REQUIRED_FLAGS     "-std=c++17")
    include(CheckIncludeFileCXX)
    check_include_file_cxx(experimental/type_traits    has_type_traits  )
    if(NOT has_type_traits)
        message(FATAL_ERROR "\n\
                Missing one or more C++17 headers.\n\
                Consider using a newer compiler (GCC 8 or above, Clang 7 or above),\n\
                or checking the compiler flags. If using Clang, pass the variable \n\
                GCC_TOOLCHAIN=<path> \n\
                where path is the install directory of a recent GCC installation (version > 8).
                Also, don't forget to compile with flags:  [-lstdc++fs -std=c++17].
        ")
    endif()

    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles("
        #include<experimental/type_traits>
        int main(){
            return 0;
        }
        " TYPETRAITS_COMPILES)
    if(NOT TYPETRAITS_COMPILES)
        message(FATAL_ERROR "Unable to compile with experimental/type_traits header")
    endif()
endfunction()