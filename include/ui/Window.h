#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "nes/Console.h"

class Window {
public:
    static constexpr int WIDTH = 256;
    static constexpr int HEIGHT = 240;
    static constexpr int SCALE = 3;
    static constexpr int PADDING = 0;

public:
    Window(Console *console);
    ~Window();

    bool init(const char *title);
    void close();
    void run();

private:
    bool readKey(int key);
    void readKeys();

    GLuint createTexture();
    void setTexture(GLuint texture, Image *image);

    void drawQuad();

private:
    Console *console;
    GLFWwindow *window;
    const char *title;
};

#endif