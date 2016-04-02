//---------------------------------------------------------------------------
//
// Copyright (c) 2016 Taehyun Rhee, Joshua Scott, Ben Allen
//
// This software is provided 'as-is' for assignment of COMP308 in ECS,
// Victoria University of Wellington, without any express or implied warranty. 
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include "cgra_math.hpp"
#include "opengl.hpp"
#include "geometry.hpp"

using namespace std;
using namespace cgra;

// Window
//
GLFWwindow* g_window;


// Projection values
// 
float g_fovy = 20.0;
float g_znear = 0.1;
float g_zfar = 1000.0;


// Mouse controlled Camera values
//
bool g_leftMouseDown = false;
vec2 g_mousePosition;
float g_pitch = 0;
float g_yaw = 0;
float g_zoom = 1.0;

dvec3 camPos = dvec3(0.0, 0.0, 50.0);
double camUp = 1.0;
mat4 aRot = mat4::identity();

// Geometry loader and drawer
//
Geometry *g_geometry = nullptr;



//-------------------------------------------------------------
// [Assignment 1] :
// The following callback functions are used for processing
// user input. Modify in EITHER of the following ways.
//
// Modify keyCallback so that when the 'r' key is pressed
// rotate the camera around the model horizontally.
//
// or
//
// Modify the mouseButtonCallback and cursorPosCallback
// functions so when the left mouse button is pressed and
// dragged along the screen horizontally, the camera also
// roates around the model horizontally.
//-------------------------------------------------------------

// Mouse Button callback
// Called for mouse movement event on since the last glfwPollEvents
//
void cursorPosCallback(GLFWwindow* win, double xpos, double ypos) {
	//cout << "Mouse Movement Callback :: xpos=" << xpos << "ypos=" << ypos << endl;
	if (g_leftMouseDown) {
		g_yaw += (xpos - g_mousePosition.x)/100;
		g_pitch += (ypos - g_mousePosition.y)/100;
	}
	g_mousePosition.x = xpos;
	g_mousePosition.y = ypos;

}

//-------------------------------------------------------------
// [Assignment 1] :
// Modify the mouseCallback function in main.cpp so that when
// the right mouse button is clicked, the wireframe mode on
// Geometry is toggled on and off.
//-------------------------------------------------------------

// Mouse Button callback
// Called for mouse button event on since the last glfwPollEvents
//
void mouseButtonCallback(GLFWwindow *win, int button, int action, int mods) {
	//cout << "Mouse Button Callback :: button=" << button << "action=" << action << "mods=" << mods << endl;
	if (button == 1 && action == 0) {
		g_geometry->toggleWireFrame();
	}
	else if (button == 0) {
		if (action == 1) {
			g_leftMouseDown = true;
		}
		else if (action == 0) {
			g_leftMouseDown = false;
		}
	}
}


// Scroll callback
// Called for scroll event on since the last glfwPollEvents
//
void scrollCallback(GLFWwindow *win, double xoffset, double yoffset) {
	//cout << "Scroll Callback :: xoffset=" << xoffset << "yoffset=" << yoffset << endl;

	if (yoffset > 0) {
		camPos = camPos + dvec3(0.0, 0.0, 5.0);
	}
	else if (yoffset < 0) {
		camPos = camPos + dvec3(0.0, 0.0, -5.0);

	}
}



// Keyboard callback
// Called for every key event on since the last glfwPollEvents
//
void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {

	//cout << "Key Callback :: key=" << key << "scancode=" << scancode
	//	<< "action=" << action << "mods=" << mods << endl;

	glTranslatef(0.0, 1.0, 60.0);
	if (action == 1) {
		double rad = (std::_Pi * 10) / 180;
		if (key == 262) {
			g_yaw = +rad;
		}
		else if (key == 263) {
			g_yaw = -rad;
		}
		else if (key == 265) {
			g_pitch = +rad;
		}
		else if (key == 264) {
			g_pitch = -rad;
		}
	}

}


// Character callback
// Called for every character input event on since the last glfwPollEvents
//
void charCallback(GLFWwindow *win, unsigned int c) {
	// cout << "Char Callback :: c=" << char(c) << endl;
	// Not needed for this assignment, but useful to have later on
}


// Sets up where and what the light is
// 
void setupLight() {
	// No transform for the light
	// makes it move realitive to camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	vec4 direction(0.0, 0.0, 1.0, 0.0);
	vec4 diffuse(0.7, 0.7, 0.7, 1.0);
	vec4 ambient(0.2, 0.2, 0.2, 1.0);

	glLightfv(GL_LIGHT0, GL_POSITION, direction.dataPointer());
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse.dataPointer());
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient.dataPointer());

	glEnable(GL_LIGHT0);
}


// Sets up where the camera is in the scene
// 
void setupCamera(int width, int height) {
	// Set up the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(g_fovy, width / float(height), g_znear, g_zfar);

	// Set up the view part of the model view matrix
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(camPos.x, camPos.y, camPos.z,// position of camera
		0.0, 0.0, 0.0, // position to look at
		0.0, 1.0, 0.0);// up relative to camera

}


// Render one frame to the current window given width and height
//
void render(int width, int height) {

	// Set viewport to be the whole window
	glViewport(0, 0, width, height);

	// Setup light
	setupLight();

	// Setup camera
	setupCamera(width, height);

	// Grey/Blueish background
	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable flags for normal rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	// Set the current material (for all objects) to red
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3f(1.0f, 0.0f, 0.0f); //red

	// Create a matrix which represents the current rotation
	mat4 cRot = mat4::identity();
	cRot = cRot * mat4::rotateY(g_yaw);
	cRot = cRot * mat4::rotateX(g_pitch);

	// Reset the current rotation
	g_yaw = 0;
	g_pitch = 0;

	// Combine the current rotation with the accumulated rotation
	mat4 tMat = cRot * aRot;
	aRot = mat4(tMat);

	float data[16];

	int index = 0;
	for (int v = 0; v < 4; v++) {
		data[index] = aRot[v].x;
		data[index + 1] = aRot[v].y;
		data[index + 2] = aRot[v].z;
		data[index + 3] = aRot[v].w;
		index += 4;
	}

	// Render geometry

	glMatrixMode(GL_MODELVIEW);
	glMultMatrixf(data); // apply the accumulated rotation to the image.

	g_geometry->renderGeometry();

	// Disable flags for cleanup (optional)
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);
	glDisable(GL_COLOR_MATERIAL);
}


// Forward decleration for cleanliness (Ignore)
void APIENTRY debugCallbackARB(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, GLvoid*);


//Main program
// 
int main(int argc, char **argv) {

	// Check argument list
	if (argc != 2) {
		cout << "Obj filename expected, eg:" << endl << "    ./a1 teapot.obj" << endl;
		abort(); // Unrecoverable error
	}



	// Initialize the GLFW library
	if (!glfwInit()) {
		cerr << "Error: Could not initialize GLFW" << endl;
		abort(); // Unrecoverable error
	}

	// Get the version for GLFW for later
	int glfwMajor, glfwMinor, glfwRevision;
	glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);

	// Create a windowed mode window and its OpenGL context
	g_window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
	if (!g_window) {
		cerr << "Error: Could not create GLFW window" << endl;
		abort(); // Unrecoverable error
	}

	// Make the g_window's context is current.
	// If we have multiple windows we will need to switch contexts
	glfwMakeContextCurrent(g_window);



	// Initialize GLEW
	// must be done after making a GL context current (glfwMakeContextCurrent in this case)
	glewExperimental = GL_TRUE; // required for full GLEW functionality for OpenGL 3.0+
	GLenum err = glewInit();
	if (GLEW_OK != err) { // Problem: glewInit failed, something is seriously wrong.
		cerr << "Error: " << glewGetErrorString(err) << endl;
		abort(); // Unrecoverable error
	}



	// Print out our OpenGL verisions
	cout << "Using OpenGL " << glGetString(GL_VERSION) << endl;
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	cout << "Using GLFW " << glfwMajor << "." << glfwMinor << "." << glfwRevision << endl;



	// Attach input callbacks to g_window
	glfwSetCursorPosCallback(g_window, cursorPosCallback);
	glfwSetMouseButtonCallback(g_window, mouseButtonCallback);
	glfwSetScrollCallback(g_window, scrollCallback);
	glfwSetKeyCallback(g_window, keyCallback);
	glfwSetCharCallback(g_window, charCallback);



	// Enable GL_ARB_debug_output if available. Not nessesary, just helpful
	if (glfwExtensionSupported("GL_ARB_debug_output")) {
		// This allows the error location to be determined from a stacktrace
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		// Set the up callback
		glDebugMessageCallbackARB(debugCallbackARB, nullptr);
		glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
		cout << "GL_ARB_debug_output callback installed" << endl;
	}
	else {
		cout << "GL_ARB_debug_output not available. No worries." << endl;
	}



	// Initialize out scene
	g_geometry = new Geometry(argv[1]);



	// Loop until the user closes the window
	while (!glfwWindowShouldClose(g_window)) {

		// Make sure we draw to the WHOLE window
		int width, height;

		glfwGetFramebufferSize(g_window, &width, &height);

		// Main Render
		render(width, height);

		// Swap front and back buffers
		glfwSwapBuffers(g_window);

		// Poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();
}






//-------------------------------------------------------------
// Fancy debug stuff
//-------------------------------------------------------------

// function to translate source to string
string getStringForSource(GLenum source) {

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		return("API");
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return("Window System");
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return("Shader Compiler");
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return("Third Party");
	case GL_DEBUG_SOURCE_APPLICATION:
		return("Application");
	case GL_DEBUG_SOURCE_OTHER:
		return("Other");
	default:
		return("n/a");
	}
}

// function to translate severity to string
string getStringForSeverity(GLenum severity) {

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		return("HIGH!");
	case GL_DEBUG_SEVERITY_MEDIUM:
		return("Medium");
	case GL_DEBUG_SEVERITY_LOW:
		return("Low");
	default:
		return("n/a");
	}
}

// function to translate type to string
string getStringForType(GLenum type) {
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		return("Error");
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return("Deprecated Behaviour");
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return("Undefined Behaviour");
	case GL_DEBUG_TYPE_PORTABILITY:
		return("Portability Issue");
	case GL_DEBUG_TYPE_PERFORMANCE:
		return("Performance Issue");
	case GL_DEBUG_TYPE_OTHER:
		return("Other");
	default:
		return("n/a");
	}
}

// actually define the function
void APIENTRY debugCallbackARB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* message, GLvoid*) {
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) return;

	cerr << endl; // extra space

	cerr << "Type: " <<
		getStringForType(type) << "; Source: " <<
		getStringForSource(source) << "; ID: " << id << "; Severity: " <<
		getStringForSeverity(severity) << endl;

	cerr << message << endl;

	if (type == GL_DEBUG_TYPE_ERROR_ARB) throw runtime_error("");
}