//
// Created by silverly on 14/09/2021.
//

#ifndef IVY_DECORATOR_H
#define IVY_DECORATOR_H


class decorator {
public:
    virtual void decorate(&Chunk c, float density = 1.0) = 0;
};


#endif //IVY_DECORATOR_H
