#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "mesh.h"
#include "mesh_renderer.h"

#define WIN_WIDTH	740
#define WIN_HEIGHT	680
#define WIN_NAME	"Physics-Sandbox"

void temp_error_callback(int error_code, const char* descr);

void main_loop(GLFWwindow *window) {
    glfwMakeContextCurrent(window);

    sMesh sphere, cube;
    sphere.load_OBJ_mesh("resources/sphere.obj");
    cube.load_OBJ_mesh("resources/cube_t.obj");

    sMeshRenderer sphere_renderer, cube_renderer;
    sphere_renderer.create_from_mesh(&sphere);
    cube_renderer.create_from_mesh(&cube);

    sphere.clean();
    cube.clean();

    while(!glfwWindowShouldClose(window)) {
        int width, heigth;
        double temp_mouse_x, temp_mouse_y;

        glfwPollEvents();
        glfwGetFramebufferSize(window, &width, &heigth);
        glViewport(0,0, width, heigth);

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Test");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

int main(int argc, char *argv[]) {
    if (!glfwInit()) {
        return EXIT_FAILURE;
    }

    // GLFW config
    glfwSetErrorCallback(temp_error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_NAME, NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!window) {
        std::cout << "Error, could not create window" << std::endl;
    } else {
        if (!gl3wInit()) {
            //IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();
            // Platform IMGUI
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 130");
            ImGui::StyleColorsDark();
            main_loop(window);
            //test_loop(window);
        } else {
            std::cout << "Cannot init gl3w" << std::endl;
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}



// GLFW CALLBACKS =========
void temp_error_callback(int error_code, const char* descr) {
    std::cout << "GLFW Error: " << error_code << " " << descr << std::endl;
}
