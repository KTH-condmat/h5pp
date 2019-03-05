
find_package(Eigen3 3.3.4 HINTS ${INSTALL_DIRECTORY_THIRD_PARTY}/Eigen3)

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    if("${_variableName}" MATCHES "EIGEN" OR "${_variableName}" MATCHES "eigen" OR "${_variableName}" MATCHES "Eigen")
#        message(STATUS "${_variableName}=${${_variableName}}")
#    endif()
#endforeach()

#if(NOT EIGEN3_FOUND)
    # Try finding arpack as module library
#    message(STATUS "SEARCHING FOR Eigen3 IN LOADED MODULES")
#    find_package(Eigen3 3.3.4 PATHS "$ENV{EIGEN3_CMAKE_DIR}" NO_DEFAULT_PATH)
#    if (NOT EIGEN3_FOUND)
#    find_path(EIGEN3_INCLUDE_DIR
#            NAMES Core
#            PATHS "$ENV{EIGEN3_INCLUDE_DIR}/Eigen"
#            )
#    endif()
#endif()


if(EIGEN3_FOUND)
    message(STATUS "EIGEN FOUND IN SYSTEM: ${EIGEN3_INCLUDE_DIR}")
    add_library(Eigen3 INTERFACE)
    target_link_libraries(Eigen3 INTERFACE Eigen3::Eigen)
    get_target_property(EIGEN3_INCLUDE_DIR Eigen3::Eigen INTERFACE_INCLUDE_DIRECTORIES)
elseif (DOWNLOAD_EIGEN3 OR DOWNLOAD_ALL)
    message(STATUS "Eigen3 will be installed into ${INSTALL_DIRECTORY_THIRD_PARTY}/Eigen3 on first build.")

    include(ExternalProject)
    ExternalProject_Add(external_EIGEN3
            GIT_REPOSITORY https://github.com/eigenteam/eigen-git-mirror.git
            GIT_TAG 3.3.7
            GIT_PROGRESS 1
            PREFIX      ${BUILD_DIRECTORY_THIRD_PARTY}/Eigen3
            INSTALL_DIR ${INSTALL_DIRECTORY_THIRD_PARTY}/Eigen3
            CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
            UPDATE_COMMAND ""
            TEST_COMMAND ""
#            INSTALL_COMMAND ""
#            CONFIGURE_COMMAND ""
        )


    ExternalProject_Get_Property(external_EIGEN3 INSTALL_DIR)
    add_library(Eigen3 INTERFACE)
    add_library(Eigen3::Eigen ALIAS Eigen3)
    set(EIGEN3_ROOT_DIR ${INSTALL_DIR})
    set(EIGEN3_INCLUDE_DIR ${INSTALL_DIR}/include/eigen3)
    set(Eigen3_DIR ${INSTALL_DIR}/share/eigen3/cmake)
    add_dependencies(Eigen3 external_EIGEN3)
    target_include_directories(
            Eigen3
            INTERFACE
            $<BUILD_INTERFACE:${INSTALL_DIR}/include/eigen3>
            $<INSTALL_INTERFACE:third-party/Eigen3/include/eigen3>
    )
else()
    message("WARNING: Dependency Eigen3 not found and DOWNLOAD_EIGEN3 is OFF. Build will fail.")
endif()

if(BLAS_LIBRARIES)
    set(EIGEN3_COMPILER_FLAGS  -Wno-parentheses) # -Wno-parentheses
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" )
        list(APPEND EIGEN3_COMPILER_FLAGS -Wno-unused-but-set-variable)
    endif()
    if(MKL_FOUND)
        list(APPEND EIGEN3_COMPILER_FLAGS -DEIGEN_USE_MKL_ALL)
        list(APPEND EIGEN3_COMPILER_FLAGS -DEIGEN_USE_LAPACKE_STRICT)
        list(APPEND EIGEN3_INCLUDE_DIR ${MKL_INCLUDE_DIR})
        message(STATUS "Eigen3 will use MKL")
    elseif (BLAS_FOUND)
        list(APPEND EIGEN3_COMPILER_FLAGS -DEIGEN_USE_BLAS)
        list(APPEND EIGEN3_COMPILER_FLAGS -DEIGEN_USE_LAPACKE)
        message(STATUS "Eigen3 will use BLAS and LAPACKE")
    endif()
endif()



target_compile_options(Eigen3 INTERFACE ${EIGEN3_COMPILER_FLAGS})
