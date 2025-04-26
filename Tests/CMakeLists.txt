project(Tests)

find_package(doctest)

add_executable(Tests main.cpp)
target_include_directories(Tests PRIVATE ../)
target_link_libraries(Tests PRIVATE EventSystem doctest::doctest)