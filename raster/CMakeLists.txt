add_executable(raster
    WIN32
    include/raster/Canvas.h
    src/raster/Canvas.cpp
    include/raster/Raster.h
    src/raster/Raster.cpp
    src/raster/main.cpp)

target_include_directories(raster
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
