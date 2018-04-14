#include "ui/Window.h"
#include <cstdio>

Window::Window(Console *console) : console(console), window(nullptr) {}

Window::~Window() {}

bool Window::init(const char *title) {
    if (!glfwInit()) {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(WIDTH, HEIGHT, title, nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc(glfwGetProcAddress)))) {
        return false;
    }
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
    glEnable(GL_TEXTURE_2D);
    GLuint texture = createTexture();

    // double timestamp = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        // double now = glfwGetTime();
        // double elapsed = now - timestamp;
        readKeys();
        console->stepFrame();
        setTexture(texture, console->buffer());
        glClear(GL_COLOR_BUFFER_BIT);
        drawQuad();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

bool Window::readKey(int key) { return glfwGetKey(window, key) == GLFW_PRESS; }

void Window::readKeys() {
    console->setPressed(1, Button::A, readKey(GLFW_KEY_Z));
    console->setPressed(1, Button::B, readKey(GLFW_KEY_X));
    console->setPressed(1, Button::Select, readKey(GLFW_KEY_S));
    console->setPressed(1, Button::Start, readKey(GLFW_KEY_ENTER));
    console->setPressed(1, Button::Up, readKey(GLFW_KEY_UP));
    console->setPressed(1, Button::Down, readKey(GLFW_KEY_DOWN));
    console->setPressed(1, Button::Left, readKey(GLFW_KEY_LEFT));
    console->setPressed(1, Button::Right, readKey(GLFW_KEY_RIGHT));
}

void Window::close() { glfwTerminate(); }