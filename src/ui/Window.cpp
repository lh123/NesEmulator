#include "ui/Window.h"
#include <cstdio>
#include "nes/Controller.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include "ui/ImGuiExt.h"
#include "ui/GameView.h"

Window::Window() : console(nullptr), window(nullptr), audio(nullptr) {
    // audio = new Audio(console);
    // console->setAudioSampleRate(44100);
}

Window::~Window() { delete gameView; }

bool Window::init(const char *title) {
    if (!glfwInit()) {
        return false;
    }
    if (!audio->init()) {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(WIDTH * SCALE, HEIGHT * SCALE, title, nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc(glfwGetProcAddress)))) {
        return false;
    }

    glfwSwapInterval(1);
    return true;
}

GLuint Window::createTexture() {
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return texture;
}

void Window::setTexture(GLuint texture, Image *image) {
    int w = image->width();
    int h = image->height();
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixel());
}

void Window::drawQuad() {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    float s1 = float(w) / float(WIDTH);
    float s2 = float(h) / float(HEIGHT);
    float f = float(1 - PADDING);
    float x, y;
    if (s1 >= s2) {
        x = f * s2 / s1;
        y = f;
    } else {
        x = f;
        y = f * s1 / s2;
    }
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex3d(-x, -y, 1);
    glTexCoord2f(1, 1);
    glVertex3d(x, -y, 1);
    glTexCoord2f(1, 0);
    glVertex3d(x, y, 1);
    glTexCoord2f(0, 0);
    glVertex3d(-x, y, 1);
    glEnd();
}

void Window::run() {
    initGUI();
    // audio->openAudioDevice(44100);
    // audio->play();
    // glEnable(GL_TEXTURE_2D);
    // GLuint texture = createTexture();

    // double timestamp = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        // double now = glfwGetTime();
        // double elapsed = now - timestamp;
        // timestamp = now;
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        renderGUI();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    destoryGUI();
    glfwTerminate();
}

void Window::close() {}

void Window::initGUI() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // ImGuiIO &io = ImGui::GetIO();

    ImGui_ImplGlfwGL3_Init(window, true);

    ImGui::StyleColorsDark();
    gameView = new GameView();
}

void Window::renderGUI() {

    ImGui_ImplGlfwGL3_NewFrame();
    char buffer[125] = {0};

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                if (ImGui::showFileDialog(buffer, 125)) {
                    gameView->setGamePath(buffer);
                    gameView->show();
                }
            }
            if (ImGui::MenuItem("Close")) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Multiplayer")) {
            if(ImGui::MenuItem("Create Server")) {

            }
            if(ImGui::MenuItem("Connect to Server")) {
                
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Setting")) {
            if (ImGui::MenuItem("KeyMap")) {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (gameView->isShow()) {
        gameView->render();
    }

    ImGui::Render();

    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::destoryGUI() {
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
}