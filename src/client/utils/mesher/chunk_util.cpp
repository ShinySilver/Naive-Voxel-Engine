/*
 * ChunkUtil.cpp
 *
 *  Created on: 5 mai 2020
 *      Author: silverly
 */

#include "chunk_util.h"

#include <initializer_list>
#include <iostream>
#include <ostream>
#include <iostream>
#include <vector>

#define CHUNK_WIDTH  (16)
#define CHUNK_HEIGHT (16)

#define CENTER_CHUNKS

#define VOXEL_SIZE (2.0f)

#define SOUTH  (0)
#define NORTH  (1)
#define EAST   (2)
#define WEST   (3)
#define TOP    (4)
#define BOTTOM (5)

namespace ChunkUtil {

    namespace {
// Private functions

/**
 * This function returns an instance of VoxelFace containing the attributes for
 * one side of a voxel.  In this simple demo we just return a value from the
 * sample data array.  However, in an actual voxel engine, this function would
 * check if the voxel face should be culled, and set per-face and per-vertex
 * values as well as voxel values in the returned instance.
 */
        ChunkUtil::VoxelFace *getVoxelFace(int x, int y, int z, int side,
                                           BasicChunk &chunk) {

            VoxelFace *voxelFace = &chunk.get(x, y, z).uniformFace;
            voxelFace->side = side;
            return voxelFace;
        }

/**
 * This function renders a single quad in the scene. This quad may represent many adjacent voxel
 * faces - so in order to create the illusion of many faces, you might consider using a tiling
 * function in your voxel shader. For this reason I've included the quad width and height as parameters.
 *
 * For example, if your texture coordinates for a single voxel face were 0 - 1 on a given axis, they should now
 * be 0 - width or 0 - height. Then you can calculate the correct texture coordinate in your fragement
 * shader using coord.xy = fract(coord.xy).
 *
 *
 * @param bottomLeft
 * @param topLeft
 * @param topRight
 * @param bottomRight
 * @param width
 * @param height
 * @param voxel
 * @param backFace
 */
        void quad(glm::vec3 bottomLeft, glm::vec3 topLeft, glm::vec3 topRight,
                  glm::vec3 bottomRight, int width, int height, VoxelFace *voxelFace,
                  Mesh *mesh, bool backFace) {
            std::cout << "QUAD!\n";

            glm::vec3 vertices[4];


#ifdef CENTER_CHUNKS
            float offset = -CHUNK_WIDTH * VOXEL_SIZE / 2.0f;
            vertices[2] = offset + topLeft * VOXEL_SIZE;
            vertices[3] = offset + topRight * VOXEL_SIZE;
            vertices[0] = offset + bottomLeft * VOXEL_SIZE;
            vertices[1] = offset + bottomRight * VOXEL_SIZE;

#else
            vertices[2] = topLeft * VOXEL_SIZE;
            vertices[3] = topRight * VOXEL_SIZE;
            vertices[0] = bottomLeft * VOXEL_SIZE;
            vertices[1] = bottomRight * VOXEL_SIZE;
#endif

            std::vector<int> indexes;
            if (backFace) {
                indexes = {2, 0, 1, 1, 3, 2};
            } else {
                indexes = {2, 3, 1, 1, 0, 2};
            }

            for (int i : indexes) {
                mesh->vertices.emplace_back(vertices[i]);
                mesh->colors.emplace_back(voxelFace->color);
            }
            /*
             mesh.setBuffer(Type.Position, 3, BufferUtils.createFloatBuffer(vertices));
             mesh.setBuffer(Type.Color, 4, colorArray);
             mesh.setBuffer(Type.Index, 3, BufferUtils.createIntBuffer(indexes));
             mesh.updateBound();

             Geometry geo = new Geometry("ColoredMesh", mesh);
             Material mat = new Material(assetManager,
             "Common/MatDefs/Misc/Unshaded.j3md");
             mat.setBoolean("VertexColor", true);

             // To see the actual rendered quads rather than the wireframe, just comment outthis line.
             mat.getAdditionalRenderState().setWireframe(true);

             geo.setMaterial(mat);

             rootNode.attachChild(geo);
             */
        }

    } // End of private namespace

    ChunkUtil::Mesh *greedyMesh(BasicChunk &chunk) {

        Mesh *mesh = new Mesh();

        /*
         * These are just working variables for the algorithm - almost all taken
         * directly from Mikola Lysenko's javascript implementation.
         */
        int i, j, k, l, w, h, u, v, n, side = 0;

        int x[] = {0, 0, 0};
        int q[] = {0, 0, 0};
        int du[] = {0, 0, 0};
        int dv[] = {0, 0, 0};

        /*
         * We create a mask - this will contain the groups of matching voxel faces
         * as we proceed through the chunk in 6 directions - once for each face.
         */
        VoxelFace *mask[CHUNK_WIDTH * CHUNK_HEIGHT]; // TODO: transfer this from the stack to the heap?

        /*
         * These are just working variables to hold two faces during comparison.
         */
        VoxelFace *voxelFace, *voxelFace1;

        /**
         * We start with the lesser-spotted bool for-loop (also known as the old flippy floppy).
         *
         * The variable backFace will be TRUE on the first iteration and FALSE on the second - this allows
         * us to track which direction the indices should run during creation of the quad.
         *
         * This loop runs twice, and the inner loop 3 times - totally 6 iterations - one for each
         * voxel face.
         */
        for (bool backFace = true, b = false; b != backFace;
             backFace = backFace && b, b = !b) {

            /*
             * We sweep over the 3 dimensions - most of what follows is well described by Mikola Lysenko
             * in his post - and is ported from his Javascript implementation.  Where this implementation
             * diverges, I've added commentary.
             */
            for (int d = 0; d < 3; d++) {

                u = (d + 1) % 3;
                v = (d + 2) % 3;

                x[0] = 0;
                x[1] = 0;
                x[2] = 0;

                q[0] = 0;
                q[1] = 0;
                q[2] = 0;
                q[d] = 1;

                /*
                 * Here we're keeping track of the side that we're meshing.
                 */
                if (d == 0) {
                    side = backFace ? WEST : EAST;
                } else if (d == 1) {
                    side = backFace ? BOTTOM : TOP;
                } else if (d == 2) {
                    side = backFace ? SOUTH : NORTH;
                }

                /*
                 * We move through the dimension from front to back
                 */
                for (x[d] = -1; x[d] < CHUNK_WIDTH;) {

                    /*
                     * -------------------------------------------------------------------
                     *   We compute the mask
                     * -------------------------------------------------------------------
                     */
                    n = 0;

                    for (x[v] = 0; x[v] < CHUNK_HEIGHT; x[v]++) {

                        for (x[u] = 0; x[u] < CHUNK_WIDTH; x[u]++) {
                            /*
                             * Here we retrieve two voxel faces for comparison.
                             */
                            voxelFace =
                                    (x[d] >= 0) ?
                                    getVoxelFace(x[0], x[1], x[2], side,
                                                 chunk) :
                                    nullptr;
                            voxelFace1 =
                                    (x[d] < CHUNK_WIDTH - 1) ?
                                    getVoxelFace(x[0] + q[0], x[1] + q[1],
                                                 x[2] + q[2], side, chunk) :
                                    nullptr;

                            /*
                             * Note that we're using the equals function in the voxel face class here, which lets the faces
                             * be compared based on any number of attributes.
                             *
                             * Also, we choose the face to add to the mask depending on whether we're moving through on a backface or not.
                             */
                            mask[n++] =
                                    ((voxelFace != nullptr && voxelFace1 != nullptr
                                      && (voxelFace->equals(*voxelFace1) ||
                                          !voxelFace->transparent && !voxelFace1->transparent))) ?
                                    nullptr :
                                    backFace ? voxelFace1 : voxelFace;
                        }
                    }

                    x[d]++;

                    /*
                     * Now we generate the mesh for the mask
                     */
                    n = 0;

                    for (j = 0; j < CHUNK_HEIGHT; j++) {

                        for (i = 0; i < CHUNK_WIDTH;) {

                            if (mask[n] != nullptr) {

                                /*
                                 * We compute the width
                                 */
                                for (w = 1;
                                     i + w < CHUNK_WIDTH
                                     && mask[n + w] != nullptr
                                     && mask[n + w]->equals(mask[n]);
                                     w++) {
                                }

                                /*
                                 * Then we compute height
                                 */
                                bool done = false;

                                for (h = 1; j + h < CHUNK_HEIGHT; h++) {

                                    for (k = 0; k < w; k++) {

                                        if (mask[n + k + h * CHUNK_WIDTH] == nullptr
                                            || !mask[n + k + h * CHUNK_WIDTH]->equals(
                                                mask[n])) {
                                            done = true;
                                            break;
                                        }
                                    }

                                    if (done) {
                                        break;
                                    }
                                }

                                /*
                                 * Here we check the "transparent" attribute in the VoxelFace class to ensure that we don't mesh
                                 * any culled faces.
                                 */
                                if (!mask[n]->transparent) {
                                    /*
                                     * Add quad
                                     */
                                    x[u] = i;
                                    x[v] = j;

                                    du[0] = 0;
                                    du[1] = 0;
                                    du[2] = 0;
                                    du[u] = w;

                                    dv[0] = 0;
                                    dv[1] = 0;
                                    dv[2] = 0;
                                    dv[v] = h;

                                    /*
                                     * And here we call the quad function in order to render a merged quad in the scene.
                                     *
                                     * We pass mask[n] to the function, which is an instance of the VoxelFace class containing
                                     * all the attributes of the face - which allows for variables to be passed to shaders - for
                                     * example lighting values used to create ambient occlusion.
                                     */
                                    quad(glm::vec3(x[0], x[1], x[2]),
                                         glm::vec3(x[0] + du[0], x[1] + du[1],
                                                   x[2] + du[2]),
                                         glm::vec3(x[0] + du[0] + dv[0],
                                                   x[1] + du[1] + dv[1],
                                                   x[2] + du[2] + dv[2]),
                                         glm::vec3(x[0] + dv[0], x[1] + dv[1],
                                                   x[2] + dv[2]), w, h, mask[n],
                                         mesh, backFace);
                                }

                                /*
                                 * We zero out the mask
                                 */
                                for (l = 0; l < h; ++l) {

                                    for (k = 0; k < w; ++k) {
                                        mask[n + k + l * CHUNK_WIDTH] = nullptr;
                                    }
                                }

                                /*
                                 * And then finally increment the counters and continue
                                 */
                                i += w;
                                n += w;

                            } else {

                                i++;
                                n++;
                            }
                        }
                    }
                }
            }
        }
        return mesh;
    }

    ChunkUtil::Mesh *naiveSurfaceNetsMesh(BasicChunk &chunk) {
        return nullptr;
    }

    ChunkUtil::Mesh *naiveMeshWithCulling(BasicChunk &chunk) {
        return nullptr;
    }

    void generateChunkMesh(BasicChunk &chunk, MesherType type) {
        switch (type) {
            default:
            case GREEDY:
                greedyMesh(chunk);
                break;
            case NAIVE_SURFACE_NET:
                naiveSurfaceNetsMesh(chunk);
                break;

            case NAIVE_WITH_CULLING:
                naiveMeshWithCulling(chunk);
                break;
        }
    }

} // End of namespace ChunkUtil
