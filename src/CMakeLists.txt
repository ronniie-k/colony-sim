file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.cpp)
file(GLOB_RECURSE HEADER_FILES *.hpp)

add_executable(colony-sim ${SOURCE_FILES})

target_include_directories(colony-sim
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    SYSTEM PRIVATE thirdparty/vma/include
    SYSTEM PRIVATE /usr/include/stb
)


# libs
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

# link Vulkan and GLFW to the executable
target_link_libraries(colony-sim 
                      Vulkan::Vulkan 
                      glfw
                      GPUOpen::VulkanMemoryAllocator)
