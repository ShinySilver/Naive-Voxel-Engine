//
// Created by silverly on 23/05/2021.
//

#ifndef IVY_ENTITY_H
#define IVY_ENTITY_H


class Entity {
public:
    Entity();
    Entity(float x, float y, float z); // convert all to glm
    Entity(float x, float y, float z, float rx, float ry, float rz);
    virtual ~Entity() = 0;

    virtual void init() = 0;
    virtual void prepare() = 0; // Parallel, limit yourself to read-only access to all but the private fields
    virtual void apply() = 0; // Parallel, apply the modifications to the public field
    virtual void render() = 0; // Sync, main thread
protected:
    int x, y, z, rx, ry, rz;
};


#endif //IVY_ENTITY_H
