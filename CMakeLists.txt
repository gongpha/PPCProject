cmake_minimum_required (VERSION 3.8)

project ("PPCProject")

find_package( OpenGL REQUIRED )
include_directories( ${OPENGL_INCLUDE_DIRS} )

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )

add_subdirectory(glfw)
#add_subdirectory(cglm)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

file(GLOB PPCProject_SRC
     "*.c"
)

add_executable (PPCProject ${PPCProject_SRC})
target_link_libraries(PPCProject ${OPENGL_LIBRARIES} glfw)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET PPCProject PROPERTY C_STANDARD 23) # USE C. NO C++
endif()