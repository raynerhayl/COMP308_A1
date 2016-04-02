#
# CGRA Project
# If you add new source files, you must list them here
#

# TODO list your header files (.hpp) here
SET(headers
	"cgra_cow.hpp"
	"cgra_math.hpp"
	"geometry.hpp"
	"opengl.hpp"
	"simple_shader.hpp"
)


# TODO list your source files (.cpp) here
SET(sources
	"cgra_cow.cpp"
	"main.cpp"
	"geometry.cpp"
)

# Add executable target and link libraries
# You do not need to touch this
add_executable(${CGRA_PROJECT} ${headers} ${sources})
target_link_libraries(${CGRA_PROJECT} PRIVATE glew glfw ${GLFW_LIBRARIES})