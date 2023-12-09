include(CMakeDependentOption)

set(SKY_CXX_STD default CACHE STRING "C++ standard version.")
option(SKY_VERBOSE_BUILD "Enables debug output from CMake." OFF)


# Enable verbose configure when passing -Wdev to CMake
if (DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS AND
    NOT CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
  set(SKY_VERBOSE_BUILD ON)
endif()

if (SKY_VERBOSE_BUILD)
  message(STATUS "[skyr-url]: verbose build enabled.")
endif()

CMAKE_DEPENDENT_OPTION(SKYR_URL_INSTALL
  "Generate an install target for skyr-url"
  ON "${skyr_IS_TOP_LEVEL_PROJECT}" OFF)
  
