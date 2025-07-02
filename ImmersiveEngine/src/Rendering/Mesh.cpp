#include"Mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector <GLuint>& indices) :
	m_vertices(vertices), m_indices(indices), m_texture(nullptr)
{
    buildMesh();
}

Mesh::Mesh(const Mesh& mesh) :
    m_vertices(mesh.m_vertices), m_indices(mesh.m_indices), m_texture(nullptr)
{
    buildMesh();
}

Mesh::~Mesh()
{
    dump();
}

/// Update VAO linkage based on current vertices and indices data.
void Mesh::buildMesh()
{
    m_VAO.Bind();

    VBO VBO(m_vertices);
    EBO EBO(m_indices);
    EBO.Bind();
    /// Assign buffer objects to layout locations in the VAO (buffer object, layout index, number of elements).
    m_VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0); // Vertex positions
    m_VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float))); // Normal direction
    m_VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float))); // Vertex color (decimal RGB)
    m_VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float))); // Texture offset coordinates

    m_VAO.Unbind();
    VBO.Unbind();
}

/// Return direct reference to vertex at a specific index in range.
Vertex& Mesh::getVertexAtIndex(const uint32_t index)
{
    if (index >= m_vertices.size())
    {
        std::cerr << "INDEX_OUT_OF_BOUNDS_ERROR vertex at index " << index << " is not in range of mesh's vertices.";
        return m_vertices.front();
    }
    return m_vertices[index];
}

/// Draw the object to the screen.
void Mesh::draw(Shader& shaderProgram)
{
    if (m_vertices.size() < 3)
    {
        std::cerr << "MESH_DRAW_ERROR not a polygon: mesh has less than 3 vertices.\n";
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


/*================================================================
                Primitive Shape Mesh Generators
=================================================================*/
ImmersiveEngine::Math::Vector3 defaultColor(255, 255, 255);
ImmersiveEngine::Math::Vector3 normalDir2D(0, 0, 1.0f);

std::vector<GLuint> squareIndices =
{
    2, 1, 0,
    2, 3, 0
};

Mesh Mesh::generateSquare(float length)
{
    float offset = length / 2;

    std::vector<Vertex> vertices =
    {
        Vertex{ImmersiveEngine::Math::Vector3(offset, offset, 0.0f), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(offset, offset)}, // 0
        Vertex{ImmersiveEngine::Math::Vector3(-offset, offset, 0.0f), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(-offset, offset)}, // 1
        Vertex{ImmersiveEngine::Math::Vector3(-offset, -offset, 0.0f), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(-offset, -offset)}, // 2
        Vertex{ImmersiveEngine::Math::Vector3(offset, -offset, 0.0f), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(offset, -offset)}, // 3
    };
    /*
        1------0
        |      |
        |      |
        2------3
    */
    return Mesh(vertices, squareIndices);
}

Mesh Mesh::generateCircle(float radius, uint32_t segments)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    if (segments < 3) segments = 3; // Minimum number of segments.

    // Center point
    vertices.push_back(Vertex{ImmersiveEngine::Math::Vector3(0.0f, 0.0f, 0.0f), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(0.0f, 0.0f)}); // 0

    for (int i = 0; i < segments + 1; ++i)
    {
        double theta = (2 * ImmersiveEngine::Math::PI) / segments;
        float x = std::cos(theta * i) * radius;
        float y = std::sin(theta * i) * radius;
        vertices.push_back(Vertex{ImmersiveEngine::Math::Vector3(x, y, 0.0f), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(x, y)});
        
        indices.push_back(0); // Start at center of circle.
        indices.push_back(i);
        indices.push_back(i + 1); // Connect to the next index.
    }

    return Mesh(vertices, indices);
}

Mesh Mesh::generatePlane(float length, float width)
{
    float offsetX = length / 2;
    float offsetZ = width / 2;

    std::vector<Vertex> vertices =
    {
        Vertex{ImmersiveEngine::Math::Vector3(offsetX, 0.0f, offsetZ), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(offsetX, offsetZ)}, // 0
        Vertex{ImmersiveEngine::Math::Vector3(-offsetX, 0.0f, offsetZ), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(-offsetX, offsetZ)}, // 1
        Vertex{ImmersiveEngine::Math::Vector3(-offsetX, 0.0f, -offsetZ), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(-offsetX, -offsetZ)}, // 2
        Vertex{ImmersiveEngine::Math::Vector3(offsetX, 0.0f, -offsetZ), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(offsetX, -offsetZ)}, // 3
    };

    return Mesh(vertices, squareIndices);
}

std::vector<GLuint> pyramidIndices =
{
    2, 1, 0,
    2, 3, 0,
    0, 1, 4,
    1, 2, 4,
    2, 3, 4,
    3, 0, 4
};

Mesh Mesh::generateSquarePyramid(float length, float height)
{
    float offset = length / 2;

    std::vector<Vertex> vertices =
    {
        Vertex{ImmersiveEngine::Math::Vector3(offset, 0.0f, offset), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(offset, offset)}, // 0
        Vertex{ImmersiveEngine::Math::Vector3(-offset, 0.0f, offset), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(-offset, offset)}, // 1
        Vertex{ImmersiveEngine::Math::Vector3(-offset, 0.0f, -offset), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(-offset, -offset)}, // 2
        Vertex{ImmersiveEngine::Math::Vector3(offset, 0.0f, -offset), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(offset, -offset)}, // 3
        Vertex{ImmersiveEngine::Math::Vector3(0, height, 0), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(offset, offset)}, // 4
    };

    return Mesh(vertices, pyramidIndices);
}

void Mesh::dump()
{
    m_VAO.Delete();

    if (m_texture)
    {
        //m_texture->Delete();
        delete m_texture;
        m_texture = nullptr;
    };
}