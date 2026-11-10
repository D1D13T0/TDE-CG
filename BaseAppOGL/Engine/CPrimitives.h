#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct PrimVertex
{
    float position[3];
    float texCoord[2];
    float normal[3];
};

class CPrimitives 
{

public:

    CPrimitives();
    ~CPrimitives();

    // CPrimitives generation methods
    void generateCube(float size = 1.0f);
    void generateSphere(float radius = 1.0f, int sectors = 36, int stacks = 18);
    void generateCylinder(float radius = 1.0f, float height = 2.0f, int sectors = 36);
    void generatePlane(float width = 2.0f, float height = 2.0f, int widthSegments = 1, int heightSegments = 1);
    void generatePyramid(float baseSize, float height);
    void generateCone(float radius = 1.0f, float height = 2.0f, int sectors = 36);
    void generateTorus(float majorRadius = 1.0f, float minorRadius = 0.3f, int majorSegments = 36, int minorSegments = 18);


    // OpenGL buffer management
    void setupBuffers();
    void render();
    void cleanup();

    // Getters
    const std::vector<PrimVertex>& getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const { return indices; }

private:
    std::vector<PrimVertex> vertices;
    std::vector<unsigned int> indices;

    unsigned int VAO;  // PrimVertex Array Object
    unsigned int VBO;  // PrimVertex Buffer Object
    unsigned int EBO;  // Element Buffer Object

    bool buffersInitialized;

    // Helper methods
    void clearData();
    void normalizeVector(float* vec);


};

#endif // PRIMITIVE_H
