#pragma once
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Defines.h"
#include "stb_image.h"

//struct Vertex
//{
//    glm::vec3 Position;
//    glm::vec3 Normal;
//    glm::vec2 TexCoords;
//    glm::vec3 Tangent;
//    glm::vec3 Bitangent;
//};
//
//struct Texture
//{
//    unsigned int id;
//    std::string type;
//    std::string path;
//};


class Mesh 
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(unsigned int shaderProgram);
    void DrawWithShadows(unsigned int shaderProgram);

    // Normais por triangulo (linhas)
    // Calcula a normal de cada triangulo (produto vetorial das arestas) e cria
    // um buffer de linhas partindo do centro do triangulo, com tamanho 'length'.
    void BuildFaceNormals(float length = 0.1f);
    // Renderiza as linhas. O shader deve ter a posicao em location 0 e,
    // opcionalmente, o uniform "uColor" (vec3).
    void DrawFaceNormals(unsigned int shaderProgram, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 0.0f));
    // Libera os buffers das linhas.
    void DeleteFaceNormals();
    bool normalsBuilt() const { return normalsVAO != 0; }
    float normalsLengthValue() const { return normalsLength; }
    

private:
    unsigned int VBO, EBO;
    void setupMesh();

    unsigned int normalsVAO = 0;
    unsigned int normalsVBO = 0;
    unsigned int normalsVertexCount = 0;
    float normalsLength = 0.0f;
};

class CAsset 
{
public:
    CAsset(const std::string& path, bool _flipTexture = false);
    ~CAsset();

    void Draw(unsigned int shaderProgram, bool shadows = false);

    // Renderiza uma linha para a normal de cada triangulo de todas as malhas.
    // Os buffers sao (re)criados automaticamente quando 'length' muda.
    // Configure antes os uniforms de matrizes (model/view/projection) do shader.
    void DrawFaceNormals(unsigned int shaderProgram, float length = 0.1f,
                         const glm::vec3& color = glm::vec3(1.0f, 1.0f, 0.0f));

    unsigned int numVertices;
    unsigned int numIndices;
    unsigned int numFaces;

	std::vector<Vertex> GetVertices() const { return meshes[0].vertices; }
	std::vector<unsigned int> GetIndices() const { return meshes[0].indices; }

private:
    std::vector<Mesh> meshes;
    std::string directory;
    bool flipTexture;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    unsigned int TextureFromFile(const char* path, const std::string& directory);
};


