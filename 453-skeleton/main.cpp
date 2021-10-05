#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Window.h"

#include "../thirdparty/imgui/imgui.h"
#include "../thirdparty/imgui/imgui_impl_opengl3.h"
#include "../thirdparty/imgui/imgui_impl_glfw.h"

float R, G, B;
int MAX_ITER = 10;
float colors[500][3];
int test = 0;


struct State {
	int iteration = -1;
	int shape = 1;


	bool operator == (State const& other) const{
		return iteration == other.iteration;
	}

	void setIteration(int i) {
		iteration = i;
	}
};

// EXAMPLE CALLBACKS
class MyCallbacks : public CallbackInterface {

public:
	MyCallbacks(ShaderProgram& shader) : shader(shader) {}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		if(action == GLFW_PRESS || action == GLFW_REPEAT) {
			if (key == GLFW_KEY_R) {
				shader.recompile();
			}
			if (key == GLFW_KEY_T) {
				state.shape = 1;
				MAX_ITER = 10;
			}
			if (key == GLFW_KEY_R) {
				state.shape = 2;
				MAX_ITER = 10;
			}
			if (key == GLFW_KEY_S) {
				state.shape = 3;
				MAX_ITER = 7;
			}
			if (key == GLFW_KEY_D) {
				state.shape = 4;
				MAX_ITER = 20;
			}
			if (key == GLFW_KEY_H) {
				state.shape = 5;
				MAX_ITER = 5;
			}
			if (key == GLFW_KEY_LEFT) {
				if(state.iteration > 0)
					state.iteration--;
			}
			if (key == GLFW_KEY_RIGHT) {
				if (state.iteration < MAX_ITER) {
					state.iteration++;
				}
			}
		}
		

	}

	State getState() {
		return state;
	}

private:
	ShaderProgram& shader;
	State state;
};

void divide_triangle(CPU_Geometry &cpuGeom, float *a, float *b, float *c, int m, int t) {
	float v0[3] = {0.f,0.f,0.f}, v1[3] = { 0.f,0.f,0.f }, v2[3] = { 0.f,0.f,0.f };
	int j;

	if (m > 0) {
		for (j = 0; j < 2; j++)	
			v0[j] = (a[j] + b[j]) * 0.5f;
		for (j = 0; j < 2; j++)
			v1[j] = (a[j] + c[j]) * 0.5f;
		for (j = 0; j < 2; j++)
			v2[j] = (b[j] + c[j]) * 0.5f;
		
		divide_triangle(cpuGeom, a, v0, v1, m - 1, 1);
		divide_triangle(cpuGeom, c, v1, v2, m - 1, 2);
		divide_triangle(cpuGeom, b, v2, v0, m - 1, 3);
	}
	else {
		cpuGeom.verts.push_back(glm::vec3(a[0], a[1], a[2]));
		cpuGeom.verts.push_back(glm::vec3(b[0], b[1], b[2]));
		cpuGeom.verts.push_back(glm::vec3(c[0], c[1], c[2]));

		if (t == 1) {
			cpuGeom.cols.push_back(glm::vec3(0.5f, 0.f, 0.f));
			cpuGeom.cols.push_back(glm::vec3(0.5f, 0.f, 0.f));
			cpuGeom.cols.push_back(glm::vec3(0.5f, 0.f, 0.f));
		}
		else if (t == 2) {
			cpuGeom.cols.push_back(glm::vec3(0.f, 0.5f, 0.f));
			cpuGeom.cols.push_back(glm::vec3(0.f, 0.5f, 0.f));
			cpuGeom.cols.push_back(glm::vec3(0.f, 0.5f, 0.f));
		}
		else {
			cpuGeom.cols.push_back(glm::vec3(0.f, 0.f, 0.5f));
			cpuGeom.cols.push_back(glm::vec3(0.f, 0.f, 0.5f));
			cpuGeom.cols.push_back(glm::vec3(0.f, 0.f, 0.5f));
		}
		
	}
}

CPU_Geometry generate_squares(CPU_Geometry& cpuGeom, float* a, float* b, float* c, float* d, int m) {

	float v0[3] = { 0.f,0.f,0.f }, v1[3] = { 0.f,0.f,0.f }, v2[3] = { 0.f,0.f,0.f }, v3[3] = { 0.f,0.f,0.f };
	int j;

	if (m > 0) {
		for (j = 0; j < 2; j++)
			v0[j] = a[j] * 0.5f;
		for (j = 0; j < 2; j++)
			v1[j] = b[j] * 0.5f;
		for (j = 0; j < 2; j++)
			v2[j] = c[j] * 0.5f;
		for (j = 0; j < 2; j++)
			v3[j] = d[j] * 0.5f;

		cpuGeom = generate_squares(cpuGeom, v0, v1, v2, v3, m - 1);
	}
	
		cpuGeom.verts.push_back(glm::vec3(a[0], a[1], a[2]));
		cpuGeom.verts.push_back(glm::vec3(b[0], b[1], b[2]));
		cpuGeom.verts.push_back(glm::vec3(c[0], c[1], c[2]));
		cpuGeom.verts.push_back(glm::vec3(d[0], d[1], d[2]));
		cpuGeom.verts.push_back(glm::vec3(a[0], a[1], a[2]));
		cpuGeom.verts.push_back(glm::vec3(c[0], c[1], c[2]));
		cpuGeom.verts.push_back(glm::vec3(b[0], b[1], b[2]));
		cpuGeom.verts.push_back(glm::vec3(d[0], d[1], d[2]));

		cpuGeom.verts.push_back(glm::vec3(a[0], a[1] - a[1], a[2]));
		cpuGeom.verts.push_back(glm::vec3(b[0] - b[0], b[1], b[2]));
		cpuGeom.verts.push_back(glm::vec3(c[0] - c[0], c[1], c[2]));
		cpuGeom.verts.push_back(glm::vec3(d[0], d[1] - d[1], d[2]));
		cpuGeom.verts.push_back(glm::vec3(a[0], a[1] - a[1], a[2]));
		cpuGeom.verts.push_back(glm::vec3(c[0] - c[0], c[1], c[2]));
		cpuGeom.verts.push_back(glm::vec3(b[0] - b[0], b[1], b[2]));
		cpuGeom.verts.push_back(glm::vec3(d[0], d[1] - d[1], d[2]));



		cpuGeom.cols.push_back(glm::vec3(0.f, 0.f, 0.5f));
		cpuGeom.cols.push_back(glm::vec3(0.f, 0.f, 0.5f));
		cpuGeom.cols.push_back(glm::vec3(0.f, 0.f, 0.5f));
		cpuGeom.cols.push_back(glm::vec3(0.f, 0.f, 0.5f));
		cpuGeom.cols.push_back(glm::vec3(0.f, 0.f, 0.5f));
		cpuGeom.cols.push_back(glm::vec3(0.f, 0.f, 0.5f));
		cpuGeom.cols.push_back(glm::vec3(0.f, 0.f, 0.5f));
		cpuGeom.cols.push_back(glm::vec3(0.f, 0.f, 0.5f));

		cpuGeom.cols.push_back(glm::vec3(0.f, 0.5f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(0.f, 0.5f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(0.f, 0.5f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(0.f, 0.5f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(0.f, 0.5f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(0.f, 0.5f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(0.f, 0.5f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(0.f, 0.5f, 0.f));

	

	return cpuGeom;
	

}

void generate_snowflake(CPU_Geometry& cpuGeom, float* a, float* b, int m, float* col) {
	float v0[3] = { 0.f,0.f,0.f }, v1[3] = { 0.f,0.f,0.f }, v2[3] = { 0.f,0.f,0.f }, v3[3] = { 0.f,0.f,0.f }, v4[3] = { 0.f,0.f,0.f };

	if (m > 0) {

		v1[0] = a[0] + (b[0] - a[0]) * 1.f / 3.f;
		v1[1] = a[1] + (b[1] - a[1]) * 1.f / 3.f;

		v3[0] = a[0] + (b[0] - a[0]) * 2.f / 3.f;
		v3[1] = a[1] + (b[1] - a[1]) * 2.f / 3.f;

		v0[0] = v3[0] - v1[0];
		v0[1] = v3[1] - v1[1];

		v2[0] = v0[0] * cos(glm::radians(60.0f)) - v0[1] * sin(glm::radians(60.0f));
		v2[1] = v0[0] * sin(glm::radians(60.0f)) + v0[1] * cos(glm::radians(60.0f));
		v2[0] += v1[0];
		v2[1] += v1[1];

		generate_snowflake(cpuGeom, a, v1, m - 1, colors[test]);
		generate_snowflake(cpuGeom, v1, v2, m - 1, colors[test]);
		generate_snowflake(cpuGeom, v2, v3, m - 1, colors[test]);
		generate_snowflake(cpuGeom, v3, b, m - 1, colors[test]);

		test++;
		if (test > 500)
			test = 0;
	}
	else {

		glm::mat4 RotationMatrix = glm::rotate(glm::mat4(1.0), glm::radians(120.0f), glm::vec3(0.f, 0.f, 1.f));
		glm::mat4 RotationMatrix2 = glm::rotate(glm::mat4(1.0), glm::radians(240.0f), glm::vec3(0.f, 0.f, 1.f));

		cpuGeom.verts.push_back(glm::vec3(a[0], a[1], a[2]));
		cpuGeom.verts.push_back(glm::vec3(b[0], b[1], b[2]));
		cpuGeom.verts.push_back(glm::vec3(RotationMatrix * glm::vec4(a[0], a[1], a[2], 1.f)));
		cpuGeom.verts.push_back(glm::vec3(RotationMatrix * glm::vec4(b[0], b[1], b[2], 1.f)));
		cpuGeom.verts.push_back(glm::vec3(RotationMatrix2 * glm::vec4(a[0], a[1], a[2], 1.f)));
		cpuGeom.verts.push_back(glm::vec3(RotationMatrix2 * glm::vec4(b[0], b[1], b[2], 1.f)));

		cpuGeom.cols.push_back(glm::vec3(col[0], col[1], col[2]));
		cpuGeom.cols.push_back(glm::vec3(col[0], col[1], col[2]));
		cpuGeom.cols.push_back(glm::vec3(col[0], col[1], col[2]));
		cpuGeom.cols.push_back(glm::vec3(col[0], col[1], col[2]));
		cpuGeom.cols.push_back(glm::vec3(col[0], col[1], col[2]));
		cpuGeom.cols.push_back(glm::vec3(col[0], col[1], col[2]));
	}

	
}



void generate_dragon(CPU_Geometry& cpuGeom, float* a, float* b, int m, int id, float* col) {
	float v0[3] = { 0.f,0.f,0.f };

	if (m > 0) {
		if (id == 1) {  //First segment
			if (a[1] == b[1]) {  //Line is horizontal
				if (a[0] < b[0]) {  //Line goes from left to right
					v0[0] = ((b[0] - a[0]) / 2.f) + a[0];
					v0[1] = a[1] - abs(b[0] - a[0]) / 2.f;
				}
				else {  //Line goes from right to left
					v0[0] = ((b[0] - a[0]) / 2.f) + a[0];
					v0[1] = a[1] + abs(b[0] - a[0]) / 2.f;
				}
			}
			else if (a[0] == b[0]) {  //Line is vertical
				if (a[1] < b[1]) {  //Line goes from bottom to top
					v0[0] = a[0] + abs(b[1] - a[1]) / 2.f;
					v0[1] = ((b[1] - a[1]) / 2.f) + a[1];
				}
				else {  //Line goes from top to bottom
					v0[0] = a[0] - abs(b[1] - a[1]) / 2.f;
					v0[1] = ((b[1] - a[1]) / 2.f) + a[1];
				}
			}
			else if (b[0] > a[0]) {
				if (b[1] > a[1]) {  //Line goes bottom left to top right
					v0[0] = b[0];
					v0[1] = a[1];
				}
				else {  //Line goes top left to bottom right
					v0[0] = a[0];
					v0[1] = b[1];
				}
			}
			else if (b[0] <= a[0]) {
				if (b[1] > a[1]) {  //Line goes bottom right to top left
					v0[0] = a[0];
					v0[1] = b[1];
				}
				else {  //Line goes top right to bottom left
					v0[0] = b[0];
					v0[1] = a[1];
				}
			}


		}

		else if (id == 2) {  //Second segment
			if (a[1] == b[1]) {  //Line is horizontal
				if (a[0] > b[0]) {  //Line goes from right to left
					v0[0] = ((b[0] - a[0]) / 2.f) + a[0];
					v0[1] = a[1] - abs(b[0] - a[0]) / 2.f;
				}
				else {  //Line goes from left to right
					v0[0] = ((b[0] - a[0]) / 2.f) + a[0];
					v0[1] = a[1] + abs(b[0] - a[0]) / 2.f;
				}
			}
			else if (a[0] == b[0]) {  //Line is vertical
				if (a[1] > b[1]) {  //Line goes from top to bottom
					v0[0] = a[0] + abs(b[1] - a[1]) / 2.f;
					v0[1] = ((b[1] - a[1]) / 2.f) + a[1];
				}
				else {  //Line goes from bottom to top
					v0[0] = a[0] - abs(b[1] - a[1]) / 2.f;
					v0[1] = ((b[1] - a[1]) / 2.f) + a[1];
				}
			}
			else if (b[0] > a[0]) {
				if (b[1] < a[1]) {  //Line goes top left to bottom right
					v0[0] = b[0];
					v0[1] = a[1];
				}
				else {  //Line goes bottom left to top right
					v0[0] = a[0];
					v0[1] = b[1];
				}
			}
			else if (b[0] <= a[0]) {
				if (b[1] < a[1]) {  //Line goes top right to bottom left
					v0[0] = a[0];
					v0[1] = b[1];
				}
				else {  //Line goes bottom right to top left
					v0[0] = b[0];
					v0[1] = a[1];
				}
			}
		}


		
		generate_dragon(cpuGeom, a, v0, m - 1,1,colors[test]);
		generate_dragon(cpuGeom, v0, b, m - 1,2,colors[test]);
		test++;
		if (test > 500)
			test = 0;
		

	}
	else {
		cpuGeom.verts.push_back(glm::vec3(a[0], a[1], a[2]));
		cpuGeom.verts.push_back(glm::vec3(b[0], b[1], b[2]));

		cpuGeom.cols.push_back(glm::vec3(col[0], col[1], col[2]));
		cpuGeom.cols.push_back(glm::vec3(col[0], col[1], col[2]));

	}
}

void draw_hilbert(CPU_Geometry& cpuGeom, float* a, float* b, float* c, float* d, int m) {
	float v0[3] = { 0.f,0.f,0.f }, v1[3] = { 0.f,0.f,0.f }, v2[3] = { 0.f,0.f,0.f }, v3[3] = { 0.f,0.f,0.f },
		v4[3] = { 0.f,0.f,0.f }, v5[3] = { 0.f,0.f,0.f }, v6[3] = { 0.f,0.f,0.f }, v7[3] = { 0.f,0.f,0.f },
		v8[3] = { 0.f,0.f,0.f }, v9[3] = { 0.f,0.f,0.f }, v10[3] = { 0.f,0.f,0.f }, v11[3] = { 0.f,0.f,0.f },
		v12[3] = { 0.f,0.f,0.f }, v13[3] = { 0.f,0.f,0.f }, v14[3] = { 0.f,0.f,0.f }, v15[3] = { 0.f,0.f,0.f };

	if (m > 0) {
		v0[0] = a[0] / 2.f - (0.5);
		v0[1] = a[1] / 2.f + (0.5);

		v1[0] = b[0] / 2.f - (0.5);
		v1[1] = b[1] / 2.f + (0.5);
							 				 
		v2[0] = c[0] / 2.f - (0.5);
		v2[1] = c[1] / 2.f + (0.5);
							 				 
		v3[0] = d[0] / 2.f - (0.5);
		v3[1] = d[1] / 2.f + (0.5);

		//2
		v4[0] = a[0] / 2.f + (0.5);
		v4[1] = a[1] / 2.f + (0.5);

		v5[0] = b[0] / 2.f + (0.5);
		v5[1] = b[1] / 2.f + (0.5);

		v6[0] = c[0] / 2.f + (0.5);
		v6[1] = c[1] / 2.f + (0.5);

		v7[0] = d[0] / 2.f + (0.5);
		v7[1] = d[1] / 2.f + (0.5);

		float si = sin(glm::radians(-90.0f));
		float co = cos(glm::radians(-90.0f));
		//3

		v8[0] = a[0] / 2.f * co - a[1] / 2.f * si - 0.5f;
		v8[1] = a[0] / 2.f * si + a[1] / 2.f * co - 0.5f;

		v9[0] = b[0] / 2.f * co - b[1] / 2.f * si - 0.5f;
		v9[1] = b[0] / 2.f * si + b[1] / 2.f * co - 0.5f;

		v10[0] = c[0] / 2.f * co - c[1] / 2.f * si - 0.5f;
		v10[1] = c[0] / 2.f * si + c[1] / 2.f * co - 0.5f;

		v11[0] = d[0] / 2.f * co - d[1] / 2.f * si - 0.5f;
		v11[1] = d[0] / 2.f * si + d[1] / 2.f * co - 0.5f;

		si = sin(glm::radians(90.0f));
		co = cos(glm::radians(90.0f));
		//4
		v12[0] = a[0] / 2.f * co - a[1] / 2.f * si + (0.5);
		v12[1] = a[0] / 2.f * si + a[1] / 2.f * co - (0.5);

		v13[0] = b[0] / 2.f * co - b[1] / 2.f * si + (0.5);
		v13[1] = b[0] / 2.f * si + b[1] / 2.f * co - (0.5);

		v14[0] = c[0] / 2.f * co - c[1] / 2.f * si + (0.5);
		v14[1] = c[0] / 2.f * si + c[1] / 2.f * co - (0.5);

		v15[0] = d[0] / 2.f * co - d[1] / 2.f * si + (0.5);
		v15[1] = d[0] / 2.f * si + d[1] / 2.f * co - (0.5);

		draw_hilbert(cpuGeom, v0,v1,v2,v3, m-1);
		draw_hilbert(cpuGeom, v4, v5, v6, v7, m - 1);
		draw_hilbert(cpuGeom, v8, v9, v10, v11, m - 1);
		draw_hilbert(cpuGeom, v12, v13, v14, v15, m - 1);

		/*cpuGeom.verts.push_back(glm::vec3(v8[0], v8[1], a[2]));
		cpuGeom.verts.push_back(glm::vec3(v0[0], v0[1], b[2]));
		cpuGeom.verts.push_back(glm::vec3(v3[0], v3[1], a[2]));
		cpuGeom.verts.push_back(glm::vec3(v4[0], v4[1], b[2]));
		cpuGeom.verts.push_back(glm::vec3(v7[0], v7[1], a[2]));
		cpuGeom.verts.push_back(glm::vec3(v15[0], v15[1], b[2]));

		cpuGeom.cols.push_back(glm::vec3(1.f, 0.f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(1.f, 0.f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(1.f, 0.f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(1.f, 0.f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(1.f, 0.f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(1.f, 0.f, 0.f));*/
	}
	else {
		
		cpuGeom.verts.push_back(glm::vec3(a[0], a[1], a[2]));
		cpuGeom.verts.push_back(glm::vec3(b[0], b[1], b[2]));
		cpuGeom.verts.push_back(glm::vec3(b[0], b[1], b[2]));
		cpuGeom.verts.push_back(glm::vec3(c[0], c[1], c[2]));
		cpuGeom.verts.push_back(glm::vec3(c[0], c[1], c[2]));
		cpuGeom.verts.push_back(glm::vec3(d[0], d[1], d[2]));

		cpuGeom.cols.push_back(glm::vec3(1.f, 0.f,0.f));
		cpuGeom.cols.push_back(glm::vec3(1.f, 0.f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(1.f, 0.f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(1.f, 0.f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(1.f, 0.f, 0.f));
		cpuGeom.cols.push_back(glm::vec3(1.f, 0.f, 0.f));
	}
}


int main() {

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(APPLE)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
	
	Log::debug("Starting main");

	// WINDOW
	glfwInit();
	Window window(800, 800, "CPSC 453"); // can set callbacks at construction if desired

	GLDebug::enable();

	// SHADERS
	ShaderProgram shader("shaders/test.vert", "shaders/test.frag");

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// CALLBACKS
	auto callbacks = std::make_shared<MyCallbacks>(shader);
	window.setCallbacks(callbacks); // can also update callbacks to new ones

	// GEOMETRY
	CPU_Geometry cpuGeom;
	GPU_Geometry gpuGeom;

	State state;

	

	// generate triangles
	float x[3] = { -0.5f,-0.5f,0.f };
	float y[3] = { 0.5f,-0.5f,0.f };
	float z[3] = { 0.f,0.5f,0.f };
	
	divide_triangle(cpuGeom, x, y, z, state.iteration + 1,1);

	

	
	 
	gpuGeom.setVerts(cpuGeom.verts);
	gpuGeom.setCols(cpuGeom.cols);

	srand(static_cast <unsigned> (time(0)));

	for (int i = 0; i < 500; i++) {
		for (int j = 0; j < 3; j++) {
			colors[i][j] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		}
	}

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	
	// RENDER LOOP
	while (!window.shouldClose()) {
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		



		if (!(state == callbacks->getState()) && callbacks->getState().shape == 1) {
			cpuGeom.verts.clear();
			cpuGeom.cols.clear();
			float a[3] = { -0.5f,-0.5f,0.f };
			float b[3] = { 0.5f,-0.5f,0.f };
			float c[3] = { 0.f,0.5f,0.f };
			divide_triangle(cpuGeom, a, b, c, callbacks->getState().iteration,1);
			gpuGeom.setVerts(cpuGeom.verts);
			gpuGeom.setCols(cpuGeom.cols);
		}

		if (!(state == callbacks->getState()) && callbacks->getState().shape == 2) {
			cpuGeom.verts.clear();
			cpuGeom.cols.clear();
			float a[3] = { -0.5f, -0.5f,0.f };
			float b[3] = { 0.5f, -0.5f,0.f };
			float c[3] = { -0.5f, 0.5f,0.f };
			float d[3] = { 0.5f, 0.5f,0.f };
			cpuGeom = generate_squares(cpuGeom, a, b, c, d, callbacks->getState().iteration);
			gpuGeom.setVerts(cpuGeom.verts);
			gpuGeom.setCols(cpuGeom.cols);
		}

		if (!(state == callbacks->getState()) && callbacks->getState().shape == 3) {
			
			cpuGeom.verts.clear();
			cpuGeom.cols.clear();
			float a[3] = { -0.5f,0.5f - 0.21f,0.f };
			float b[3] = { 0.5f,0.5f - 0.21f,0.f };
			test = 0;
			generate_snowflake(cpuGeom, a, b, callbacks->getState().iteration, colors[test]);
			gpuGeom.setVerts(cpuGeom.verts);
			gpuGeom.setCols(cpuGeom.cols);
		}

		if (!(state == callbacks->getState()) && callbacks->getState().shape == 4) {
			cpuGeom.verts.clear();
			cpuGeom.cols.clear();
			float a[3] = { 0.f,0.f,0.f };
			float b[3] = { 0.4f,0.f,0.f };
			test = 0;
			generate_dragon(cpuGeom, a, b, callbacks->getState().iteration, 1, colors[test]);

			
			gpuGeom.setVerts(cpuGeom.verts);
			gpuGeom.setCols(cpuGeom.cols);
		}

		if (!(state == callbacks->getState()) && callbacks->getState().shape == 5) {
			cpuGeom.verts.clear();
			cpuGeom.cols.clear();
			float a[3] = { -0.5f,-0.5f,0.f };
			float b[3] = { -0.5f,0.5f,0.f };
			float c[3] = { 0.5f,0.5f,0.f };
			float d[3] = { 0.5f,-0.5f,0.f };
			draw_hilbert(cpuGeom, a, b, c, d, callbacks->getState().iteration);


			gpuGeom.setVerts(cpuGeom.verts);
			gpuGeom.setCols(cpuGeom.cols);
		}

		shader.use();
		gpuGeom.bind();

		glEnable(GL_FRAMEBUFFER_SRGB);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if(callbacks->getState().shape == 1)
			glDrawArrays(GL_TRIANGLES, 0, pow(3, callbacks->getState().iteration +1));
		if (callbacks->getState().shape == 2)
			glDrawArrays(GL_LINES, 0, (callbacks->getState().iteration + 1) * 16);
		if (callbacks->getState().shape == 3)
			glDrawArrays(GL_LINES, 0, pow(4, callbacks->getState().iteration + 1) * 3);
		if (callbacks->getState().shape == 4)
			glDrawArrays(GL_LINES, 0, pow(2, callbacks->getState().iteration + 1));
		if (callbacks->getState().shape == 5)
			glDrawArrays(GL_LINES, 0, pow(4, callbacks->getState().iteration + 1)*2);
		glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("CPSC 453 Assignment 1 by Hassan Tariq");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("Use the arrow keys to change iterations");
			ImGui::Text("Press T for Triangle");
			ImGui::Text("Press R for Rectangle");
			ImGui::Text("Press S for Snowflake");
			ImGui::Text("Press D for Dragon Curve");
			ImGui::Text("Press H for Hilbert Curve");
			ImGui::Text("");
			ImGui::Text("");
			// Display some text (you can use a format strings too)
			//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			//ImGui::Checkbox("Another Window", &show_another_window);

			//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			//if (ImGui::Button("+"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				//callbacks->getState().setIteration(5);

			ImGui::SameLine();
			ImGui::Text("Iteration = %d", callbacks->getState().iteration);


			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.swapBuffers();

	}

	glfwTerminate();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	return 0;
}
