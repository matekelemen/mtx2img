cmake_minimum_required(VERSION 3.15.0)
project(mtx2img LANGUAGES CXX VERSION 1.1.0)

# Language requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Target definition
add_executable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME}
                      PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY
                      "${CMAKE_BINARY_DIR}/bin")

# Headers and sources
target_include_directories(${PROJECT_NAME}
                           SYSTEM PRIVATE
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external>")

target_include_directories(${PROJECT_NAME}
                           PUBLIC
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                           "$<INSTALL_INTERFACE:include>")

target_sources(${PROJECT_NAME}
               PRIVATE
               "${CMAKE_CURRENT_SOURCE_DIR}/src/mtx2img.cpp"
               "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Compiler arguments
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME}
                           PRIVATE
                           -Wall -Wpedantic -Wextra -Werror)
endif()

# Package
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VENDOR
    "Máté Kelemen")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Generate sparsity pattern images from sparse matrices in matrix market format.")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/license.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/readme.md")

set(CPACK_GENERATOR "ZIP")
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /.vscode
    /build)
include (CPack)

# Generate CMake config
include(CMakePackageConfigHelpers)
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION "bin/cmake/${PROJECT_NAME}"
                              NO_SET_AND_CHECK_MACRO
                              NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION "${${PROJECT_NAME}_VERSION_MAJOR.${PROJECT_NAME}_VERSION_MINOR}"
                                 COMPATIBILITY AnyNewerVersion)

# Install
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION "bin"
        INCLUDES DESTINATION "include")

install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        DESTINATION "bin/cmake/${PROJECT_NAME}")

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION "bin/cmake/${PROJECT_NAME}")

# Export
set(CMAKE_EXPORT_PACKAGE_REGISTRY)
export(TARGETS ${PROJECT_NAME}
       FILE ${PROJECT_NAME}Targets.cmake)
export(PACKAGE ${PROJECT_NAME})
