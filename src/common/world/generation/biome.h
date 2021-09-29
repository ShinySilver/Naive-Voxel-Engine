//
// Created by silverly on 14/09/2021.
//

#ifndef IVY_BIOME_H
#define IVY_BIOME_H


class biome : public decorator{
public:
    void get_gen_parameters();
    void get_decorators();
    virtual void decorate(&Chunk c, float density) final;
};


#endif //IVY_BIOME_H
