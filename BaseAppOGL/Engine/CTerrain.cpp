#include "CTerrain.h"

CTerrain::CTerrain(const char* filename)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data)
    {
        std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // Interleaved layout per vertex: [ pos(3), normal(3), texCoord(2) ] = 8 floats
    float yScale = 64.0f / 256.0f, yShift = 16.0f;
    int rez = 1;
    unsigned bytePerPixel = nrChannels;

    // Helper lambda: returns the scaled Y value for grid position (row i, col j)
    auto getY = [&](int i, int j) -> float {
        i = std::max(0, std::min(i, height - 1));
        j = std::max(0, std::min(j, width  - 1));
        unsigned char* p = data + (j + width * i) * bytePerPixel;
        return (int)p[0] * yScale - yShift;
    };

    std::vector<float> vertices;
    vertices.reserve(height * width * 8);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // --- Position ---
            float vx = -width  / 2.0f + width  * j / (float)width;
            float vy = getY(i, j);
            float vz = -height / 2.0f + height * i / (float)height;

            // --- Normal (central differences on the height field) ---
            // Sample the four cardinal neighbours to approximate the surface gradient.
            float hL = getY(i,     j - 1);   // left
            float hR = getY(i,     j + 1);   // right
            float hD = getY(i - 1, j    );   // down (prev row)
            float hU = getY(i + 1, j    );   // up   (next row)

            // Tangent vectors along X and Z axes, then cross-product gives the normal.
            // T_x = (2, hR-hL, 0),  T_z = (0, hU-hD, 2)
            // N   = T_x × T_z  =  (-(hR-hL), 4, -(hU-hD))  (already up-facing)
            float nx = -(hR - hL);
            float ny =  4.0f;                // constant step of 2 on each side → 2*2=4
            float nz = -(hU - hD);

            // Normalise
            float len = std::sqrt(nx*nx + ny*ny + nz*nz);
            nx /= len;  ny /= len;  nz /= len;

            // --- Texture coordinates (0..1 over the whole terrain) ---
            float u = j / (float)(width  - 1);
            float v = i / (float)(height - 1);

            // Push interleaved data
            vertices.push_back(vx);   // position
            vertices.push_back(vy);
            vertices.push_back(vz);
            vertices.push_back(nx);   // normal
            vertices.push_back(ny);
            vertices.push_back(nz);
            vertices.push_back(u);    // texCoord
            vertices.push_back(v);
        }
    }
    std::cout << "Loaded " << vertices.size() / 8 << " vertices" << std::endl;
    stbi_image_free(data);

    // Build indices (same triangle-strip logic as before)
    std::vector<unsigned> indices;
    for (unsigned i = 0; i < (unsigned)height - 1; i += rez)
    {
        for (unsigned j = 0; j < (unsigned)width; j += rez)
        {
            for (unsigned k = 0; k < 2; k++)
            {
                indices.push_back(j + width * (i + k * rez));
            }
        }
    }
    std::cout << "Loaded " << indices.size() << " indices" << std::endl;

    numStrips = (height - 1) / rez;
    numTrisPerStrip = (width / rez) * 2 - 2;
    std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each" << std::endl;
    std::cout << "Created " << numStrips * numTrisPerStrip << " triangles total" << std::endl;

    // Configure VAO / VBO / EBO
    // -------------------------
    constexpr int STRIDE = 8 * sizeof(float);

    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    // location 0 — position (vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE, (void*)0);
    glEnableVertexAttribArray(0);

    // location 1 — normal (vec3)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, STRIDE, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // location 2 — texCoord (vec2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, STRIDE, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // EBO
    glGenBuffers(1, &terrainEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

CTerrain::~CTerrain()
{
    glDeleteVertexArrays(1, &terrainVAO);
    glDeleteBuffers(1, &terrainVBO);
    glDeleteBuffers(1, &terrainEBO);
}

void CTerrain::DrawTerrain()
{
    glBindVertexArray(terrainVAO);
    for (unsigned strip = 0; strip < numStrips; strip++)
    {
        glDrawElements(GL_TRIANGLE_STRIP,
            numTrisPerStrip + 2,
            GL_UNSIGNED_INT,
            (void*)(sizeof(unsigned) * (numTrisPerStrip + 2) * strip));
    }
    glBindVertexArray(0);
}
