#include "ui/Window.h"
#include <iostream>
#include <sstream>
#include "nes/Controller.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include "ui/ImGuiExt.h"
#include "ui/GameView.h"

Window::Window()
    : console(nullptr), window(nullptr), audio(nullptr), gameView(nullptr), createServerView(nullptr),
      joinServerView(nullptr), server(nullptr), client(nullptr) {
    gameManager = new GameManager();
    server = new Server();
    client = new Client();
}

Window::~Window() { delete gameManager; }

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

void Window::run() {
    initGUI();
    // audio->openAudioDevice(44100);
    // audio->play();

    while (!glfwWindowShouldClose(window)) {
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

    gameView = new GameView(gameManager);
    createServerView = new CreateServerView();
    joinServerView = new JoinServerView();

    createServerView->addClickListener([this](UI_ID id, void *data) { onClick(id, data); });
    joinServerView->addClickListener([this](UI_ID id, void *data) { onClick(id, data); });
}

void Window::destoryGUI() {
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    gameView->close();
    createServerView->close();
    joinServerView->close();

    delete gameView;
    delete createServerView;
    delete joinServerView;
}

void Window::onClick(UI_ID id, void *data) {
    std::stringstream conv;
    if (id == UI_ID::CreateServerView_Btn_Create) {
        conv << reinterpret_cast<char *>(data);
        unsigned short port;
        conv >> port;
        if (conv.bad()) {
            std::cout << "Invaild Port:" << reinterpret_cast<char *>(data) << std::endl;
        } else {
            server->startServer(port);
            client->connect("127.0.0.1", port);
        }
    } else if (id == UI_ID::JoinServerView_Btn_Connect) {
        JoinServerView::Data *cdata = reinterpret_cast<JoinServerView::Data *>(data);
        conv << cdata->port;
        unsigned short port;
        conv >> port;
        if (conv.bad()) {
            std::cout << "Invaild Port:" << cdata->port << std::endl;
        } else {
            client->connect(cdata->ip, port);
        }
    }
}

void Window::renderGUI() {

    ImGui_ImplGlfwGL3_NewFrame();
    char buffer[125] = {0};

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                if (ImGui::showFileDialog(buffer, 125)) {
                    gameManager->startGame(buffer);
                    gameView->show();
                }
            }
            if (ImGui::MenuItem("Close")) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Multiplayer")) {
            if (ImGui::MenuItem("Create Server")) {
                createServerView->show();
            }
            if (ImGui::MenuItem("Connect to Server")) {
                joinServerView->show();
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
    if (createServerView->isShow()) {
        createServerView->render();
    }
    if (joinServerView->isShow()) {
        joinServerView->render();
    }
    ImGui::Render();

    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}
