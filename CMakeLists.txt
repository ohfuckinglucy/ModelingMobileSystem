cmake_minimum_required(VERSION 3.24)
project(PlutoSDR_Notes LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/thirdparty)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(FUNC_DIR ${PROJECT_SOURCE_DIR}/functions)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

# ---------- Threads ----------
find_package(Threads REQUIRED)

# ---------- OpenGL ----------
find_package(OpenGL REQUIRED)

# ---------- FFTW3 ----------
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW3 REQUIRED fftw3)

# ---------- SDL2 ----------
find_package(SDL2 CONFIG QUIET)
if(NOT SDL2_FOUND)
    message(STATUS "SDL2 not found, fetching...")
    FetchContent_Declare(SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG release-2.28.5
    )
    FetchContent_MakeAvailable(SDL2)
endif()

# ---------- GLEW ----------
find_package(GLEW QUIET)
if(NOT GLEW_FOUND)
    message(STATUS "GLEW not found, fetching...")
    FetchContent_Declare(glew
        GIT_REPOSITORY https://github.com/nigels-com/glew
        GIT_TAG glew-2.2.0
    )
    FetchContent_MakeAvailable(glew)
endif()

# ---------- ImGui ----------
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_compile_options(imgui PRIVATE -fPIC)
target_link_libraries(imgui PUBLIC SDL2::SDL2 OpenGL::GL)

# ---------- ImPlot ----------
FetchContent_Declare(implot
    GIT_REPOSITORY https://github.com/epezent/implot
    GIT_TAG master
)
FetchContent_MakeAvailable(implot)

add_library(implot STATIC
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_demo.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
)
target_include_directories(implot PUBLIC ${implot_SOURCE_DIR})
target_link_libraries(implot PUBLIC imgui)

# ---------- Исходники ----------
set(COMMON_SRCS
    ${FUNC_DIR}/bits_operation.cpp
    ${FUNC_DIR}/error_detector.cpp
    ${FUNC_DIR}/interleaving.cpp
    ${FUNC_DIR}/modulator.cpp
    ${FUNC_DIR}/imgui_funcs.cpp
    ${SRC_DIR}/back.cpp
)

# ---------- main.elf ----------
add_executable(main.elf
    ${SRC_DIR}/main.cpp
    ${COMMON_SRCS}
)
target_include_directories(main.elf PRIVATE ${INCLUDE_DIR} ${FFTW3_INCLUDE_DIRS})
target_link_libraries(main.elf PRIVATE
    imgui
    implot
    ${FFTW3_LIBRARIES}
    GLEW::GLEW
    SDL2::SDL2
    OpenGL::GL
    Threads::Threads
)