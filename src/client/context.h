//
// Created by silverly on 20/05/2021.
//

#ifndef IVY_CONTEXT_H
#define IVY_CONTEXT_H

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720
#define WIN_DEFAULT_FRAMERATE 60
#define WIN_STICKY 1
#define WIN_VSYNC 0

namespace context {
    GLFWwindow *init();

    void setFullscreen(bool);

    int getWidth();
    int getHeight();

    void terminate();
}


#endif //IVY_CONTEXT_H
