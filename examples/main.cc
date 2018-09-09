
#include "deps/GL/glew.h"
#include "deps/GLFW/glfw3.h"

#include <iostream>
#include <memory>

#include "Mesh.h"
#include "OpenGLFun.h"
#include "camera.h"
#include "deps/glm/gtc/matrix_transform.hpp"
#include "deps/glm/gtc/type_ptr.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
Camera camera(glm::vec3(0.0f, 5.0f, 15.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
int main(int argc, char**argv) {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow * win = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SimpleEng", NULL, NULL);

	glfwMakeContextCurrent(win);
	glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);
	glfwSetCursorPosCallback(win, mouse_callback);
	glfwSetScrollCallback(win, scroll_callback);
	glewExperimental = true;

	if (glewInit() != GLEW_OK) {
		printf("Error to initialize GLEW.\n");
		return false;
	}
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	std::shared_ptr<Mesh> Mesh2B = std::make_shared<Mesh>(); 
	std::shared_ptr<Mesh> MeshQuad = std::make_shared<Mesh>();
	MeshQuad->createquad();
	Mesh2B->Load("2b.obj");
	unsigned int simpleProgram = OpenGL::CreateShader("vertex.glsl", "fragment.glsl");
	
	unsigned int textureDiffuse = OpenGL::LoadTexture("diffuse2B.png");
	unsigned int textureSpecular = OpenGL::LoadTexture("specular2B.png");


	// configure g-buffer framebuffer
	// ------------------------------
	unsigned int gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	unsigned int gPosition, gNormal, gAlbedoSpec;
	// position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	// normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	// color + specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	const unsigned int NR_LIGHTS = 32;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = ((rand() % 100) / 100.0) * 7.0 - 2.0;
		float yPos = ((rand() % 100) / 100.0) * 11.0 - 4.0;
		float zPos = ((rand() % 100) / 100.0) * 7.0 - 2.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}
	unsigned int lighting = OpenGL::CreateShader("lightVert.glsl", "lightFrag.glsl");

	std::vector<glm::vec3> objectPositions;
	objectPositions.push_back(glm::vec3(-5.0, 0, -5.0));
	objectPositions.push_back(glm::vec3(0.0, 0, -5.0));
	objectPositions.push_back(glm::vec3(5.0, 0, -5.0));
	objectPositions.push_back(glm::vec3(-5.0, 0, 0.0));
	objectPositions.push_back(glm::vec3(0.0, 0, 0.0));
	objectPositions.push_back(glm::vec3(5.0, 0, 0.0));
	objectPositions.push_back(glm::vec3(-5.0, 0, 5.0));
	objectPositions.push_back(glm::vec3(0.0, 0, 5.0));
	objectPositions.push_back(glm::vec3(5.0, 0, 5.0));
	glm::mat4 modelPosition;
	while (!glfwWindowShouldClose(win)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(win);
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		////
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(simpleProgram);
		
		glBindVertexArray(Mesh2B->vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureDiffuse);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureSpecular);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh2B->evo);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		OpenGL::setUniform("projectionview",simpleProgram, projection*view);
		
	
		for (unsigned int i = 0; i < objectPositions.size(); i++)
		{
			modelPosition = glm::mat4();
			modelPosition = glm::translate(modelPosition, objectPositions[i]);
			modelPosition = glm::scale(modelPosition, glm::vec3(0.05f));
			OpenGL::setUniform("model", simpleProgram, modelPosition);
			glDrawElements(GL_TRIANGLES, Mesh2B->nindex, GL_UNSIGNED_INT, (void*)0);
		}
	
		
		
		glBindVertexArray(0);
		glUseProgram(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//////////////////////////////
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(lighting);
		glBindVertexArray(MeshQuad->vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MeshQuad->evo);

		OpenGL::setUniform("gPosition", lighting, 0);
		OpenGL::setUniform("gNormal", lighting, 1);
		OpenGL::setUniform("gAlbedoSpec", lighting, 2);



		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
;
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			OpenGL::setUniform("lights[" + std::to_string(i) + "].Position", lighting, lightPositions[i]);
			OpenGL::setUniform("lights[" + std::to_string(i) + "].Color", lighting, lightColors[i]);
			
			const float constant = 1.0; 
			const float linear = 0.7;
			const float quadratic = 1.8;
			OpenGL::setUniform("lights[" + std::to_string(i) + "].Linear", lighting, linear);
			OpenGL::setUniform("lights[" + std::to_string(i) + "].Quadratic", lighting, quadratic);
			// then calculate radius of light volume/sphere
			const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
			float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
			OpenGL::setUniform("lights[" + std::to_string(i) + "].Radius", lighting, radius);
		}
		OpenGL::setUniform("viewPos", lighting, camera.Position);
		glDrawElements(GL_TRIANGLES, MeshQuad->nindex, GL_UNSIGNED_INT, (void*)0);
		///////////////
		glBindVertexArray(0);
		glUseProgram(0);


		////////////////////////

		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
												   // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
												   // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
												   // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, 800, SCR_WIDTH, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}



void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}