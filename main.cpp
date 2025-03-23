// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <glad/gl.h>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "Heatmap.h"
#include "BoidCloud.h"
#include "MainCamera.h"
#include "glm/geometric.hpp"
#include "GameGlobals.h"
#include "glm/trigonometric.hpp"
#include "ModelLoading.h"
#include <glm/mat4x4.hpp>
#include "stb_image.h"

#define GL_CALL(_CALL)      do { _CALL; GLenum gl_err = glGetError(); if (gl_err != 0) fprintf(stderr, "%s:%d GL error 0x%x returned from '%s'.\n", __FILE__, __LINE__, gl_err, #_CALL); } while (0)  // Call with error check


static void printGLError()
{
    GLenum gl_err = glGetError();
    if (gl_err != 0)
        fprintf(stderr, "%s:%d GL error 0x%x \n", __FILE__, __LINE__, gl_err);
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    MainCamera::screen_width = width;
    MainCamera::screen_height = height;
}

void processInput(GLFWwindow* window)
{
    float cameraSpeed = 2.5f * GameGlobals::deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        MainCamera::camera_position += cameraSpeed * MainCamera::camera_front;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        MainCamera::camera_position -= cameraSpeed * MainCamera::camera_front;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        MainCamera::camera_position -= cameraSpeed * glm::normalize(glm::cross(MainCamera::camera_front,
                                                                               MainCamera::camera_up));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        MainCamera::camera_position += cameraSpeed * glm::normalize(glm::cross(MainCamera::camera_front,
                                                                               MainCamera::camera_up));
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            MainCamera::mouse_info.mouse_look = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else if (action == GLFW_RELEASE)
        {
            MainCamera::mouse_info.mouse_look = false;
            MainCamera::first_mouse = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void mouseMovementCallback(GLFWwindow* window, double xpos, double ypos)
{
    MainCamera::handleMouseMovement(xpos, ypos);
}

void GLAPIENTRY OpenGlDebugMessageCallback(GLenum source,
                                           GLenum type,
                                           GLuint id,
                                           GLenum severity,
                                           GLsizei length,
                                           const GLchar* message,
                                           const void* userParam)
{
    const char* severity_str;


#define ENUM_TO_STR_CASE(sym, name) \
    case (name):                 \
        (sym) = #name;  \
        break;

    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

    switch (severity)
    {
    ENUM_TO_STR_CASE(severity_str, GL_DEBUG_SEVERITY_LOW)
    ENUM_TO_STR_CASE(severity_str, GL_DEBUG_SEVERITY_MEDIUM)
    ENUM_TO_STR_CASE(severity_str, GL_DEBUG_SEVERITY_HIGH)
    ENUM_TO_STR_CASE(severity_str, GL_DEBUG_SEVERITY_NOTIFICATION)
    default:
        severity_str = "Unknown";
        break;
    }
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = %s, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity_str, message);
}

int main(int, char**)
{
    stbi_set_flip_vertically_on_load(true);
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    MainCamera::screen_width = 1280;
    MainCamera::screen_height = 720;
    GLFWwindow* window = glfwCreateWindow(MainCamera::screen_width, MainCamera::screen_height,
                                          "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mouseMovementCallback);
    glfwSwapInterval(1); // Enable vsync

    // setup GLAD
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        fprintf(stderr, "Failed to initialize OpenGL context\n");
        return 1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    printGLError();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    struct
    {
        bool show_demo_window = false;
        bool show_another_window = false;
    } imgui_demo_windows;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // heatmap
    printGLError();
    Heatmap heatmap(1280, 720);
    heatmap.fillWithNoise();
    printGLError();

    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(OpenGlDebugMessageCallback, 0);
    // boids
    BoidCloud cloud;
    const BoidCloud::BoidParams BOID_PARAMS_DEFAULT = {1, 1.0f, glm::vec3(20.f, 20.f, 20.f)};
    BoidCloud::BoidParams boidParams = {1, 1.0f, glm::vec3(20.f, 20.f, 20.f)};
    cloud.setBoidParameters(boidParams);

    Model model = loadModel(std::string("resources/fish/fish.obj"));
    Shader shader("resources/1.model_loading.vsh.glsl", "resources/1.model_loading.fsh.glsl");

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        GameGlobals::updateDeltaTime();

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        if (!io.WantCaptureKeyboard)
        {
            processInput(window);
        }
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (imgui_demo_windows.show_demo_window)
            ImGui::ShowDemoWindow(&imgui_demo_windows.show_demo_window);

        // boid parameters
        {
            ImGui::Begin("Boid Parameters");
            bool didChange = ImGui::SliderFloat("Scale", &boidParams.scale, 0.0f, 1.0f) ||
                             ImGui::SliderInt("Quantity", &boidParams.quantity, 0, 100);
            if (ImGui::Button("Reset"))
            {
                didChange = true;
                boidParams = BOID_PARAMS_DEFAULT;
            }
            if (didChange)
            {
                cloud.setBoidParameters(boidParams);
            }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
                     clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cloud.update();
        cloud.draw();

        draw(&model, &shader);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
