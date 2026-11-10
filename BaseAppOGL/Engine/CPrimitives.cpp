#pragma warning(disable : 4244)
#pragma warning(disable : 4267)

#include "CPrimitives.h"
#include <algorithm>
#include <GL/glew.h>  // or #include <glad/glad.h> depending on your setup

CPrimitives::CPrimitives() : VAO(0), VBO(0), EBO(0), buffersInitialized(false) {
}

CPrimitives::~CPrimitives() {
    cleanup();
}

void CPrimitives::clearData() {
    vertices.clear();
    indices.clear();
}

void CPrimitives::normalizeVector(float* vec) {
    float length = std::sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    if (length > 0.0f) {
        vec[0] /= length;
        vec[1] /= length;
        vec[2] /= length;
    }
}

void CPrimitives::generateCube(float size) {
    clearData();
    
    float halfSize = size / 2.0f;
    
    // Define the 6 faces of the cube
    // Each face has 4 vertices
    
    // Front face (+Z)
    vertices.push_back({{-halfSize, -halfSize,  halfSize}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}});
    vertices.push_back({{ halfSize, -halfSize,  halfSize}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}});
    vertices.push_back({{ halfSize,  halfSize,  halfSize}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}});
    vertices.push_back({{-halfSize,  halfSize,  halfSize}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}});
    
    // Back face (-Z)
    vertices.push_back({{ halfSize, -halfSize, -halfSize}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}});
    vertices.push_back({{-halfSize, -halfSize, -halfSize}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}});
    vertices.push_back({{-halfSize,  halfSize, -halfSize}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}});
    vertices.push_back({{ halfSize,  halfSize, -halfSize}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}});
    
    // Top face (+Y)
    vertices.push_back({{-halfSize,  halfSize,  halfSize}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}});
    vertices.push_back({{ halfSize,  halfSize,  halfSize}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}});
    vertices.push_back({{ halfSize,  halfSize, -halfSize}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}});
    vertices.push_back({{-halfSize,  halfSize, -halfSize}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}});
    
    // Bottom face (-Y)
    vertices.push_back({{-halfSize, -halfSize, -halfSize}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}});
    vertices.push_back({{ halfSize, -halfSize, -halfSize}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}});
    vertices.push_back({{ halfSize, -halfSize,  halfSize}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}});
    vertices.push_back({{-halfSize, -halfSize,  halfSize}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}});
    
    // Right face (+X)
    vertices.push_back({{ halfSize, -halfSize,  halfSize}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}});
    vertices.push_back({{ halfSize, -halfSize, -halfSize}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}});
    vertices.push_back({{ halfSize,  halfSize, -halfSize}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}});
    vertices.push_back({{ halfSize,  halfSize,  halfSize}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}});
    
    // Left face (-X)
    vertices.push_back({{-halfSize, -halfSize, -halfSize}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}});
    vertices.push_back({{-halfSize, -halfSize,  halfSize}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}});
    vertices.push_back({{-halfSize,  halfSize,  halfSize}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}});
    vertices.push_back({{-halfSize,  halfSize, -halfSize}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}});
    
    // Define indices for the 6 faces (2 triangles per face)
    for (int i = 0; i < 6; i++) {
        int base = i * 4;
        indices.push_back(base);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }
}

void CPrimitives::generateSphere(float radius, int sectors, int stacks) {
    clearData();
    
    float sectorStep = 2 * M_PI / sectors;
    float stackStep = M_PI / stacks;
    
    // Generate vertices
    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = M_PI / 2 - i * stackStep;  // from pi/2 to -pi/2
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);
        
        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * sectorStep;  // from 0 to 2pi
            
            PrimVertex v;
            // Position
            v.position[0] = xy * cosf(sectorAngle);
            v.position[1] = xy * sinf(sectorAngle);
            v.position[2] = z;
            
            // Texture coordinates
            v.texCoord[0] = (float)j / sectors;
            v.texCoord[1] = (float)i / stacks;
            
            // Normal (normalized position for a sphere centered at origin)
            v.normal[0] = v.position[0] / radius;
            v.normal[1] = v.position[1] / radius;
            v.normal[2] = v.position[2] / radius;
            
            vertices.push_back(v);
        }
    }
    
    // Generate indices
    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;
        
        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            
            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

void CPrimitives::generateCylinder(float radius, float height, int sectors) {
    clearData();
    
    float halfHeight = height / 2.0f;
    float sectorStep = 2 * M_PI / sectors;
    
    // Generate side vertices
    for (int i = 0; i <= 1; ++i) {
        float y = (i == 0) ? -halfHeight : halfHeight;
        
        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * sectorStep;
            float x = radius * cosf(sectorAngle);
            float z = radius * sinf(sectorAngle);
            
            PrimVertex v;
            v.position[0] = x;
            v.position[1] = y;
            v.position[2] = z;
            
            v.texCoord[0] = (float)j / sectors;
            v.texCoord[1] = (float)i;
            
            // Normal for cylinder side (perpendicular to axis)
            v.normal[0] = x / radius;
            v.normal[1] = 0.0f;
            v.normal[2] = z / radius;
            
            vertices.push_back(v);
        }
    }
    
    // Generate side indices
    for (int i = 0; i < sectors; ++i) {
        int k1 = i;
        int k2 = i + sectors + 1;
        
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
        
        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
    }
    
    // Generate top and bottom caps
    int baseIndex = vertices.size();
    
    // Bottom cap center
    vertices.push_back({{0.0f, -halfHeight, 0.0f}, {0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}});
    
    for (int i = 0; i <= sectors; ++i) {
        float sectorAngle = i * sectorStep;
        float x = radius * cosf(sectorAngle);
        float z = radius * sinf(sectorAngle);
        
        vertices.push_back({{x, -halfHeight, z}, 
                           {0.5f + 0.5f * cosf(sectorAngle), 0.5f + 0.5f * sinf(sectorAngle)}, 
                           {0.0f, -1.0f, 0.0f}});
    }
    
    // Bottom cap indices
    for (int i = 0; i < sectors; ++i) {
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + i + 2);
        indices.push_back(baseIndex + i + 1);
    }
    
    // Top cap center
    int topBaseIndex = vertices.size();
    vertices.push_back({{0.0f, halfHeight, 0.0f}, {0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}});
    
    for (int i = 0; i <= sectors; ++i) {
        float sectorAngle = i * sectorStep;
        float x = radius * cosf(sectorAngle);
        float z = radius * sinf(sectorAngle);
        
        vertices.push_back({{x, halfHeight, z}, 
                           {0.5f + 0.5f * cosf(sectorAngle), 0.5f + 0.5f * sinf(sectorAngle)}, 
                           {0.0f, 1.0f, 0.0f}});
    }
    
    // Top cap indices
    for (int i = 0; i < sectors; ++i) {
        indices.push_back(topBaseIndex);
        indices.push_back(topBaseIndex + i + 1);
        indices.push_back(topBaseIndex + i + 2);
    }
}

void CPrimitives::generatePlane(float width, float height, int widthSegments, int heightSegments) {
    clearData();
    
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    
    float segmentWidth = width / widthSegments;
    float segmentHeight = height / heightSegments;
    
    // Generate vertices
    for (int i = 0; i <= heightSegments; ++i) {
        float z = -halfHeight + i * segmentHeight;
        float v = (float)i / heightSegments;
        
        for (int j = 0; j <= widthSegments; ++j) {
            float x = -halfWidth + j * segmentWidth;
            float u = (float)j / widthSegments;
            
            PrimVertex vertex;
            vertex.position[0] = x;
            vertex.position[1] = 0.0f;
            vertex.position[2] = z;
            
            vertex.texCoord[0] = u;
            vertex.texCoord[1] = v;
            
            vertex.normal[0] = 0.0f;
            vertex.normal[1] = 1.0f;
            vertex.normal[2] = 0.0f;
            
            vertices.push_back(vertex);
        }
    }
    
    // Generate indices
    for (int i = 0; i < heightSegments; ++i) {
        for (int j = 0; j < widthSegments; ++j) {
            int a = i * (widthSegments + 1) + j;
            int b = a + widthSegments + 1;
            
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(a + 1);
            
            indices.push_back(a + 1);
            indices.push_back(b);
            indices.push_back(b + 1);
        }
    }
}


void CPrimitives::generatePyramid(float baseSize, float height) {
    clearData();

    float h = baseSize / 2.0f;
    float top = height;

    // Base corners (Y=0)
    // BL, BR, FR, FL  (counter-clockwise from below)
    float base[4][3] = {
        {-h, 0.0f, -h},
        { h, 0.0f, -h},
        { h, 0.0f,  h},
        {-h, 0.0f,  h}
    };

    // ---- Base (facing down) ----
    {
        float n[3] = { 0.0f, 1.0f, 0.0f };
        int baseIdx = (int)vertices.size();
        vertices.push_back({ {base[0][0], base[0][1], base[0][2]}, {0.0f, 0.0f}, {n[0], n[1], n[2]} });
        vertices.push_back({ {base[1][0], base[1][1], base[1][2]}, {1.0f, 0.0f}, {n[0], n[1], n[2]} });
        vertices.push_back({ {base[2][0], base[2][1], base[2][2]}, {1.0f, 1.0f}, {n[0], n[1], n[2]} });
        vertices.push_back({ {base[3][0], base[3][1], base[3][2]}, {0.0f, 1.0f}, {n[0], n[1], n[2]} });
        // Two triangles (winding for downward face)
        indices.push_back(baseIdx + 0); indices.push_back(baseIdx + 1); indices.push_back(baseIdx + 2);
        indices.push_back(baseIdx + 0); indices.push_back(baseIdx + 2); indices.push_back(baseIdx + 3);
    }

    // ---- Four triangular side faces ----
    // Each side: two base corners + apex
    struct Side { int a; int b; };
    Side sides[4] = { {0,1},{1,2},{2,3},{3,0} };
    float apex[3] = { 0.0f, top, 0.0f };

    for (int s = 0; s < 4; ++s) {
        float* p0 = base[sides[s].a];
        float* p1 = base[sides[s].b];

        // Compute face normal via cross product
        float u[3] = { p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2] };
        float v[3] = { apex[0] - p0[0], apex[1] - p0[1], apex[2] - p0[2] };
        float n[3] = {
            -(u[1] * v[2] - u[2] * v[1]),
            -(u[2] * v[0] - u[0] * v[2]),
            -(u[0] * v[1] - u[1] * v[0])
        };
        normalizeVector(n);

        int baseIdx = (int)vertices.size();
        vertices.push_back({ {p0[0], p0[1], p0[2]}, {0.0f, 0.0f}, {n[0], n[1], n[2]} });
        vertices.push_back({ {p1[0], p1[1], p1[2]}, {1.0f, 0.0f}, {n[0], n[1], n[2]} });
        vertices.push_back({ {apex[0], apex[1], apex[2]}, {0.5f, 1.0f}, {n[0], n[1], n[2]} });

        indices.push_back(baseIdx + 0);
        indices.push_back(baseIdx + 2);
        indices.push_back(baseIdx + 1);
    }
}


void CPrimitives::generateCone(float radius, float height, int sectors) {
    clearData();

    float halfHeight = height / 2.0f;
    float sectorStep = 2 * M_PI / sectors;
    float slantY = radius / height; // controls how "tilted" the side normal is

    // Side: base ring + apex, duplicated per sector so each triangle gets a smoothly
    // varying normal (same trick used for the cylinder's caps).
    for (int j = 0; j <= sectors; ++j) {
        float sectorAngle = j * sectorStep;
        float cosA = cosf(sectorAngle);
        float sinA = sinf(sectorAngle);

        float n[3] = { cosA, slantY, sinA };
        normalizeVector(n);

        PrimVertex vBase;
        vBase.position[0] = radius * cosA;
        vBase.position[1] = -halfHeight;
        vBase.position[2] = radius * sinA;
        vBase.texCoord[0] = (float)j / sectors;
        vBase.texCoord[1] = 0.0f;
        vBase.normal[0] = n[0]; vBase.normal[1] = n[1]; vBase.normal[2] = n[2];
        vertices.push_back(vBase);

        PrimVertex vApex;
        vApex.position[0] = 0.0f;
        vApex.position[1] = halfHeight;
        vApex.position[2] = 0.0f;
        vApex.texCoord[0] = (float)j / sectors;
        vApex.texCoord[1] = 1.0f;
        vApex.normal[0] = n[0]; vApex.normal[1] = n[1]; vApex.normal[2] = n[2];
        vertices.push_back(vApex);
    }

    for (int j = 0; j < sectors; ++j) {
        int base0 = j * 2;
        int apex0 = base0 + 1;
        int base1 = base0 + 2;

        indices.push_back(base0);
        indices.push_back(base1);
        indices.push_back(apex0);
    }

    // Bottom cap
    int baseIndex = (int)vertices.size();
    vertices.push_back({ {0.0f, -halfHeight, 0.0f}, {0.5f, 0.5f}, {0.0f, -1.0f, 0.0f} });

    for (int j = 0; j <= sectors; ++j) {
        float sectorAngle = j * sectorStep;
        float x = radius * cosf(sectorAngle);
        float z = radius * sinf(sectorAngle);

        vertices.push_back({ {x, -halfHeight, z},
                            {0.5f + 0.5f * cosf(sectorAngle), 0.5f + 0.5f * sinf(sectorAngle)},
                            {0.0f, -1.0f, 0.0f} });
    }

    for (int j = 0; j < sectors; ++j) {
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + j + 2);
        indices.push_back(baseIndex + j + 1);
    }
}

void CPrimitives::generateTorus(float majorRadius, float minorRadius, int majorSegments, int minorSegments) {
    clearData();

    float majorStep = 2 * M_PI / majorSegments;
    float minorStep = 2 * M_PI / minorSegments;

    // Generate vertices
    for (int i = 0; i <= majorSegments; ++i) {
        float majorAngle = i * majorStep;
        float cosMajor = cosf(majorAngle);
        float sinMajor = sinf(majorAngle);

        for (int j = 0; j <= minorSegments; ++j) {
            float minorAngle = j * minorStep;
            float cosMinor = cosf(minorAngle);
            float sinMinor = sinf(minorAngle);

            PrimVertex v;
            v.position[0] = (majorRadius + minorRadius * cosMinor) * cosMajor;
            v.position[1] = minorRadius * sinMinor;
            v.position[2] = (majorRadius + minorRadius * cosMinor) * sinMajor;

            v.texCoord[0] = (float)i / majorSegments;
            v.texCoord[1] = (float)j / minorSegments;

            // Points from the tube's center line outward - already unit length
            v.normal[0] = cosMinor * cosMajor;
            v.normal[1] = sinMinor;
            v.normal[2] = cosMinor * sinMajor;

            vertices.push_back(v);
        }
    }

    // Generate indices
    for (int i = 0; i < majorSegments; ++i) {
        int k1 = i * (minorSegments + 1);
        int k2 = k1 + minorSegments + 1;

        for (int j = 0; j < minorSegments; ++j, ++k1, ++k2) {
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }
    }
}



void CPrimitives::setupBuffers() {
    if (buffersInitialized) {
        cleanup();
    }
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PrimVertex), vertices.data(), GL_STATIC_DRAW);
    
    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PrimVertex), (void*)offsetof(PrimVertex, position));
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PrimVertex), (void*)offsetof(PrimVertex, normal));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(PrimVertex), (void*)offsetof(PrimVertex, texCoord));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    buffersInitialized = true;
}

void CPrimitives::render() {
    if (!buffersInitialized) {
        return;
    }
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void CPrimitives::cleanup() {
    if (buffersInitialized) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        buffersInitialized = false;
    }
}
