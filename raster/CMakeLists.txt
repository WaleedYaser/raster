add_executable(raster
    WIN32

    include/raster/Canvas.h
    include/raster/Input.h
    include/raster/Vec3.h
    include/raster/Mat4.h
    include/raster/Raster.h
    include/raster/stb_image.h
    include/raster/stb_image_write.h
    include/raster/Image.h

    src/raster/Canvas.cpp
    src/raster/Raster.cpp
    src/raster/stb_image.cpp
    src/raster/stb_image_write.cpp
    src/raster/Image.cpp
    src/raster/main.cpp)

target_include_directories(raster
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
