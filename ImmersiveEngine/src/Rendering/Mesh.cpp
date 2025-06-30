#include"Mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector <GLuint>& indices) :
	m_vertices(vertices), m_indices(indices), m_texture(nullptr),
    m_VBO(vertices), m_EBO(indices)
{
    buildMesh();
}

Mesh::~Mesh()
{
    dump();
}

void Mesh::buildMesh()
{
    m_VAO.Bind();

    m_EBO.Bind();
    /// Assign buffer objects to layout locations in the VAO (buffer object, layout index, number of elements).
    m_VAO.LinkAttrib(m_VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0); // Vertex positions
    m_VAO.LinkAttrib(m_VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float))); // Normal direction
    m_VAO.LinkAttrib(m_VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float))); // Vertex color (decimal RGB)
    m_VAO.LinkAttrib(m_VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float))); // Texture offset coordinates

    m_VAO.Unbind();
    m_VBO.Unbind();
}

/// Draw the object to the screen.
void Mesh::draw(Shader& shaderProgram)
{
    if (m_vertices.size() < 3)
    {
        std::cerr << "MESH_DRAW_ERROR not a polygon: mesh has less than 3 vertices.";
        return;
    }

    //shaderProgram.setFloat("scale", 1.0f);
    if (m_texture)
    {
        shaderProgram.setInt("tex0", 0);
        shaderProgram.setBool("textured", true);
        m_texture->Bind();
    }
    else
    {
        shaderProgram.setBool("textured", false);
    }
    m_VAO.Bind();
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);

    if (m_texture)
    {
        m_texture->Unbind();
    }
}


/// Set image texture on mesh.
void Mesh::setTexture(Texture* texture)
{
    if (m_texture) // Texture already applied, replace it.
    {
        //m_texture->Delete();
        m_texture = nullptr;
    }
    m_texture = texture;
}

ImmersiveEngine::Math::Vector3 defaultColor(255, 255, 255);

std::vector<GLuint> squareIndices =
{
    2, 0, 1,
    3, 0, 2
};

Mesh Mesh::generateSquare(float length)
{
    float offset = length / 2;
    ImmersiveEngine::Math::Vector3 normalDir(0, 0, 1.0f);
    std::vector<Vertex> vertices =
    {
        Vertex{ImmersiveEngine::Math::Vector3(offset, offset, 1.0f), normalDir, defaultColor, ImmersiveEngine::Math::Vector2(offset, offset)}, // 0
        Vertex{ImmersiveEngine::Math::Vector3(-offset, offset, 1.0f), normalDir, defaultColor, ImmersiveEngine::Math::Vector2(-offset, offset)}, // 1
        Vertex{ImmersiveEngine::Math::Vector3(-offset, -offset, 1.0f), normalDir, defaultColor, ImmersiveEngine::Math::Vector2(-offset, -offset)}, // 2
        Vertex{ImmersiveEngine::Math::Vector3(offset, -offset, 1.0f), normalDir, defaultColor, ImmersiveEngine::Math::Vector2(offset, -offset)}, // 3
    };
    /*
        1------0
        |      |
        |      |
        2------3
    */
    return Mesh(vertices, squareIndices);
}

void Mesh::dump()
{
    m_VAO.Delete();
    m_VBO.Delete();
    m_EBO.Delete();
    if (m_texture)
    {
        //m_texture->Delete();
        delete m_texture;
        m_texture = nullptr;
    };
}