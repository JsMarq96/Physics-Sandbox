#include <cstddef>
#include <cstdint>
#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "glm/ext/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "camera.h"
#include "mesh.h"
#include "mesh_renderer.h"
#include "trasform.h"

#define WIN_WIDTH	740
#define WIN_HEIGHT	680
#define WIN_NAME	"Physics-Sandbox"

#define INSTANCE_COUNT 50

void temp_error_callback(int error_code, const char* descr);


enum eShape : uint8_t {
   SPHERE = 0,
   PLANE,
   BOX,
   SHAPE_COUNT
};

sTransform transforms[INSTANCE_COUNT];
sSpeed speeds[INSTANCE_COUNT];
bool is_static[INSTANCE_COUNT];
eShape shapes[INSTANCE_COUNT];
float mass[INSTANCE_COUNT];
float inv_mass[INSTANCE_COUNT];
uint16_t obj_count = 0;

glm::mat4 cube_models[INSTANCE_COUNT];
glm::mat4 sphere_models[INSTANCE_COUNT];

inline uint16_t add_sphere(const glm::vec3 &position,
                           const float radius,
                           const float mass_i,
                           const bool is_static_i) {
    transforms[obj_count].position = position;
    transforms[obj_count].scale = {radius, radius, radius};
    shapes[obj_count] = SPHERE;
    is_static[obj_count] = is_static_i;

    if (is_static_i) {
        inv_mass[obj_count] = 0.0f;
        mass[obj_count] = 0.0f;
    } else {
        inv_mass[obj_count] = 1.0f / mass_i;
        mass[obj_count] = mass_i;
    }

    return obj_count++;
}


void main_loop(GLFWwindow *window) {
    glfwMakeContextCurrent(window);

    sCamera camera;
    camera.position = {5.0f, 0.50f, 5.0f};
    camera.look_at({0.0f, 0.001f, 0.0f});

    sMesh sphere, cube;
    sphere.load_OBJ_mesh("resources/sphere.obj");
    cube.load_OBJ_mesh("resources/cube_t.obj");

    sMeshRenderer sphere_renderer, cube_renderer;
    sphere_renderer.create_from_mesh(&sphere);
    cube_renderer.create_from_mesh(&cube);

    sphere.clean();
    cube.clean();

    memset(is_static, false, sizeof(is_static));


    uint16_t sphere_obj = add_sphere({0.0f, 50.0f, 0.0f},
                                     1.0f,
                                     10.0f,
                                     false);

    add_sphere({0.0f, 0.0f, 0.0f}, 1.0f, 20.0f, true);

    double start_time = glfwGetTime(), prev_frame_time;
    while(!glfwWindowShouldClose(window)) {
        int width, heigth;
        double temp_mouse_x, temp_mouse_y;

        glfwPollEvents();
        glfwGetFramebufferSize(window, &width, &heigth);
        glViewport(0,0, width, heigth);

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        double curr_frame_time = glfwGetTime();
        double elapsed_time = curr_frame_time - prev_frame_time;
        prev_frame_time = curr_frame_time;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Speed");
        for(uint16_t i = 0; i < obj_count; i++) {
            if (!is_static[i]) {
                continue;
            }

            char obj_name[] = "Obj 00";
            obj_name[5] += i;
            if(ImGui::TreeNode(obj_name)) {
                 ImGui::Text("Position %f %f %f",
                             transforms[i].position.x,
                             transforms[i].position.y,
                             transforms[i].position.z);
                 ImGui::Text("Speed %f %f %f",
                             speeds[i].linear.x,
                             speeds[i].linear.y,
                             speeds[i].linear.z);
                 ImGui::TreePop();
            }
        }
        ImGui::End();


        // Update
        for(uint16_t i = 0; i < obj_count; i++) {
            if (!is_static[i]) {
                continue;
            }
            // Add gravity
            speeds[i].linear += glm::vec3{0.0f, -9.8f, 0.0f};
        }


        // Integrate
        for(uint16_t i = 0; i < obj_count; i++) {
            if (!is_static[i]) {
                continue;
            }

            speeds[i].linear += (float) elapsed_time * speeds[i].linear;
            // TODO: angular speed integration

            // Add some energy loss
            speeds[i].linear *= 0.999f;
            speeds[i].angular *= 0.999f;
        }

        uint16_t sphere_count = 0;
        uint16_t cube_count = 0;
        for(uint16_t i = 0; i < obj_count; i++) {
            if (shapes[i] == BOX) {
                cube_models[cube_count++] = transforms[i].get_model();
            } else if (shapes[i] == SPHERE) {
                sphere_models[sphere_count++] = transforms[i].get_model();
            } else if (shapes[i] == PLANE) {
                cube_models[cube_count++] = glm::scale(transforms[i].get_model(),
                                                       glm::vec3{1.0f, 0.01f, 1.0f});
            }
        }

        glm::mat4 vp_mat;
        camera.get_perspective_viewprojection_matrix(90.0f,
                                                     1000.0f,
                                                     0.01f,
                                                     (float) width / heigth,
                                                     &vp_mat);

        sphere_renderer.render(sphere_models,
                               {0.0f, 1.0f, 0.0f, 1.0f},
                               sphere_count,
                               vp_mat,
                               true);
        cube_renderer.render(cube_models,
                             {0.0f, 1.0f, 0.0f, 1.0f},
                             cube_count,
                             vp_mat,
                             true);



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
