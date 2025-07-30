include(FetchContent)
FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG        v3.1.1
)
FetchContent_MakeAvailable(cxxopts)

add_executable(osd main.cpp linalg.cpp combination.cpp
        simulation_utils.h
        osd.cpp
        osd.h
        permutation.cpp
        permutation.h)

target_link_libraries(osd PRIVATE cxxopts)

if(SANITIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")
endif()
