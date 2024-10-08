#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

struct Vertex 
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture 
{
    GLuint id;
    string type;
    string path;
};

class Mesh
{
public:
    vector<Vertex>       Vertices;
    vector<unsigned int> Indices;
    vector<Texture>      Textures;
    GLuint VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    void Draw(Shader& shader);

private:
    GLuint VBO;
    GLuint EBO;

    void _SetupMesh();
};

