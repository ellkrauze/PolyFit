# SET(CGAL_DIR "C:/Users/che132/projects/vcpkg/installed/x64-windows/include/CGAL")

# SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "C:/local/boost_1_87_0")
# SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/local/boost_1_87_0/lib")

# SET(QT_ADDITIONAL_PACKAGES_PREFIX_PATH "C:/Qt/6.7.3/msvc2022_64")
# SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/Qt/6.7.3/msvc2022_64/lib/cmake")

# SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/Users/che132/projects/vcpkg/installed/x64-windows/share")
# set(GMP_INCLUDE_DIR "C:/Users/che132/projects/vcpkg/installed/x64-windows/include")
# set(GMP_LIBRARIES "C:/Users/che132/projects/vcpkg/installed/x64-windows/lib/gmp.lib")

# set(MPFR_INCLUDE_DIR "C:/Users/che132/projects/vcpkg/installed/x64-windows/include")
# set(MPFR_LIBRARIES "C:/Users/che132/projects/vcpkg/installed/x64-windows/lib/mpfr.lib")

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
link_libraries(${OPENGL_LIBRARIES})

cmake_minimum_required(VERSION 3.12)

project(PolyFit)

add_subdirectory(code)


