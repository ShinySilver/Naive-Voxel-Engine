//
// Created by silverly on 20/05/2021.
//

#ifndef IVY_CONTEXT_H
#define IVY_CONTEXT_H

namespace context {
    GLFWwindow *init();

    void setFullscreen(bool);

    int getWidth();
    int getHeight();

    void terminate();
}


#endif //IVY_CONTEXT_H
