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
    if (ImmersiveEngine::Settings::g_showWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
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
ImmersiveEngine::Math::Vector3 defaultColor(1.0f, 1.0f, 1.0f);
ImmersiveEngine::Math::Vector3 normalDir2D(0, 0, 1.0f);

std::vector<GLuint> squareIndices =
{
    2, 1, 0,
    2, 3, 0
};

Mesh Mesh::generateSquare(const float length)
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
        |======|
        |      | length
        |______|
         length
    */
    return Mesh(vertices, squareIndices);
}

Mesh Mesh::generateCircle(const float radius, uint32_t segments)
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
    /*
          ****
        ********
       **********  radius
        ********
          ****
        segments determine smoothness
    */
    return Mesh(vertices, indices);
}

Mesh Mesh::generatePlane(const float length, const float width)
{
    float offsetX = length / 2;
    float offsetZ = width / 2;

    ImmersiveEngine::Math::Vector3 normalDir(0, 1.0f, 0);

    std::vector<Vertex> vertices =
    {
        Vertex{ImmersiveEngine::Math::Vector3(offsetX, 0.0f, offsetZ), normalDir, defaultColor, ImmersiveEngine::Math::Vector2(offsetX, offsetZ)}, // 0
        Vertex{ImmersiveEngine::Math::Vector3(-offsetX, 0.0f, offsetZ), normalDir, defaultColor, ImmersiveEngine::Math::Vector2(-offsetX, offsetZ)}, // 1
        Vertex{ImmersiveEngine::Math::Vector3(-offsetX, 0.0f, -offsetZ), normalDir, defaultColor, ImmersiveEngine::Math::Vector2(-offsetX, -offsetZ)}, // 2
        Vertex{ImmersiveEngine::Math::Vector3(offsetX, 0.0f, -offsetZ), normalDir2D, defaultColor, ImmersiveEngine::Math::Vector2(offsetX, -offsetZ)}, // 3
    };
    /*
        |======|
        |      | width
        |______|
         length
    */
    return Mesh(vertices, squareIndices);
}

std::vector<GLuint> cubeIndices =
{
    0, 1, 2,
    3, 2, 1,

    4, 5, 6,
    7, 6, 5,

    8, 9, 10,
    11, 10, 9,

    12, 13, 14,
    15, 14, 13,

    16, 17, 18,
    19, 18, 17,

    20, 21, 22,
    23, 22, 21
};

Mesh Mesh::generateCube(const float length)
{
    float offset = length / 2;

    std::vector<Vertex> vertices =
    {
        // Bottom face
        Vertex{ImmersiveEngine::Math::Vector3(offset, -offset, offset), ImmersiveEngine::Math::Vector3(0.0f, -1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, offset)}, // 0
        Vertex{ImmersiveEngine::Math::Vector3(-offset, -offset, offset), ImmersiveEngine::Math::Vector3(0.0f, -1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(-offset, offset)}, // 1
        Vertex{ImmersiveEngine::Math::Vector3(offset, -offset, -offset), ImmersiveEngine::Math::Vector3(0.0f, -1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, -offset)}, // 2
        Vertex{ImmersiveEngine::Math::Vector3(-offset, -offset, -offset), ImmersiveEngine::Math::Vector3(0.0f, -1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(-offset, -offset)}, // 3

        // Top face
        Vertex{ImmersiveEngine::Math::Vector3(offset, offset, offset), ImmersiveEngine::Math::Vector3(0.0f, 1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, offset)}, // 4
        Vertex{ImmersiveEngine::Math::Vector3(-offset, offset, offset), ImmersiveEngine::Math::Vector3(0.0f, 1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(-offset, offset)}, // 5
        Vertex{ImmersiveEngine::Math::Vector3(offset, offset, -offset), ImmersiveEngine::Math::Vector3(0.0f, 1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, -offset)}, // 6
        Vertex{ImmersiveEngine::Math::Vector3(-offset, offset, -offset), ImmersiveEngine::Math::Vector3(0.0f, 1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(-offset, -offset)}, // 7

        // Front face
        Vertex{ImmersiveEngine::Math::Vector3(offset, -offset, offset), ImmersiveEngine::Math::Vector3(0.0f, 0.0f, 1.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, offset)}, // 8
        Vertex{ImmersiveEngine::Math::Vector3(-offset, -offset, offset), ImmersiveEngine::Math::Vector3(0.0f, 0.0f, 1.0f), defaultColor, ImmersiveEngine::Math::Vector2(-offset, offset)}, // 9
        Vertex{ImmersiveEngine::Math::Vector3(offset, offset, offset), ImmersiveEngine::Math::Vector3(0.0f, 0.0f, 1.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, -offset)}, // 10
        Vertex{ImmersiveEngine::Math::Vector3(-offset, offset, offset), ImmersiveEngine::Math::Vector3(0.0f, 0.0f, 1.0f), defaultColor, ImmersiveEngine::Math::Vector2(-offset, -offset)}, // 11

        // Back face
        Vertex{ImmersiveEngine::Math::Vector3(offset, -offset, -offset), ImmersiveEngine::Math::Vector3(0.0f, 0.0f, -1.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, offset)}, // 12
        Vertex{ImmersiveEngine::Math::Vector3(-offset, -offset, -offset), ImmersiveEngine::Math::Vector3(0.0f, 0.0f, -1.0f), defaultColor, ImmersiveEngine::Math::Vector2(-offset, offset)}, // 13
        Vertex{ImmersiveEngine::Math::Vector3(offset, offset, -offset), ImmersiveEngine::Math::Vector3(0.0f, 0.0f, -1.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, -offset)}, // 14
        Vertex{ImmersiveEngine::Math::Vector3(-offset, offset, -offset), ImmersiveEngine::Math::Vector3(0.0f, 0.0f, -1.0f), defaultColor, ImmersiveEngine::Math::Vector2(-offset, -offset)}, // 15

        // Right face
        Vertex{ImmersiveEngine::Math::Vector3(offset, -offset, -offset), ImmersiveEngine::Math::Vector3(1.0f, 0.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, -offset)}, // 16
        Vertex{ImmersiveEngine::Math::Vector3(offset, -offset, offset), ImmersiveEngine::Math::Vector3(1.0f, 0.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(-offset, -offset)}, // 18
        Vertex{ImmersiveEngine::Math::Vector3(offset, offset, -offset), ImmersiveEngine::Math::Vector3(1.0f, 0.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, offset)}, // 19
        Vertex{ImmersiveEngine::Math::Vector3(offset, offset, offset), ImmersiveEngine::Math::Vector3(1.0f, 0.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(-offset, offset)}, // 20

        // Left face
        Vertex{ImmersiveEngine::Math::Vector3(-offset, -offset, -offset), ImmersiveEngine::Math::Vector3(-1.0f, 0.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, -offset)}, // 20
        Vertex{ImmersiveEngine::Math::Vector3(-offset, -offset, offset), ImmersiveEngine::Math::Vector3(-1.0f, 0.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(-offset, -offset)}, // 21
        Vertex{ImmersiveEngine::Math::Vector3(-offset, offset, -offset), ImmersiveEngine::Math::Vector3(-1.0f, 0.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, offset)}, // 22
        Vertex{ImmersiveEngine::Math::Vector3(-offset, offset, offset), ImmersiveEngine::Math::Vector3(-1.0f, 0.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(-offset, offset)}, // 23
    };
    /*
         /|======/|
        /_|_____/ | length
        | |     | |
        | /=====| / 
        |/______|/  length
          length
    */
    return Mesh(vertices, cubeIndices);
}

std::vector<GLuint> pyramidIndices =
{
    2, 1, 0,
    2, 3, 0,
    
    4, 5, 6,
    7, 8, 9,
    10, 11, 12,
    13, 14, 15
};

Mesh Mesh::generateSquarePyramid(const float length, const float height)
{
    float offset = length / 2;

    std::vector<Vertex> vertices =
    {
        Vertex{ImmersiveEngine::Math::Vector3(offset, 0.0f, offset), ImmersiveEngine::Math::Vector3(0.0f, -1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, 0)}, // 0
        Vertex{ImmersiveEngine::Math::Vector3(-offset, 0.0f, offset), ImmersiveEngine::Math::Vector3(0.0f, -1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(0, 0)}, // 1
        Vertex{ImmersiveEngine::Math::Vector3(-offset, 0.0f, -offset), ImmersiveEngine::Math::Vector3(0.0f, -1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, 0)}, // 2
        Vertex{ImmersiveEngine::Math::Vector3(offset, 0.0f, -offset), ImmersiveEngine::Math::Vector3(0.0f, -1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(0, 0)}, // 3

        Vertex{ImmersiveEngine::Math::Vector3(offset, 0.0f, offset), ImmersiveEngine::Math::Vector3(0.0f, 1.0f, 1.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, 0)}, // 4
        Vertex{ImmersiveEngine::Math::Vector3(-offset, 0.0f, offset), ImmersiveEngine::Math::Vector3(0.0f, 1.0f, 1.0f), defaultColor, ImmersiveEngine::Math::Vector2(0, 0)}, // 5
        Vertex{ImmersiveEngine::Math::Vector3(0, height, 0), ImmersiveEngine::Math::Vector3(0.0f, 1.0f, 1.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset / 2, offset * height)}, // 6

        Vertex{ImmersiveEngine::Math::Vector3(-offset, 0.0f, -offset), ImmersiveEngine::Math::Vector3(0.0f, 1.0f, -1.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, 0)}, // 7
        Vertex{ImmersiveEngine::Math::Vector3(offset, 0.0f, -offset), ImmersiveEngine::Math::Vector3(0.0f, 1.0f, -1.0f), defaultColor, ImmersiveEngine::Math::Vector2(0, 0)}, // 8
        Vertex{ImmersiveEngine::Math::Vector3(0, height, 0), ImmersiveEngine::Math::Vector3(0.0f, 1.0f, -1.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset / 2, offset * height)}, // 9

        Vertex{ImmersiveEngine::Math::Vector3(offset, 0.0f, -offset), ImmersiveEngine::Math::Vector3(1.0f, 1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, 0)}, // 10
        Vertex{ImmersiveEngine::Math::Vector3(offset, 0.0f, offset), ImmersiveEngine::Math::Vector3(1.0f, 1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(0, 0)}, // 11
        Vertex{ImmersiveEngine::Math::Vector3(0, height, 0), ImmersiveEngine::Math::Vector3(1.0f, 1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset / 2, offset * height)}, // 12
        
        Vertex{ImmersiveEngine::Math::Vector3(-offset, 0.0f, -offset), ImmersiveEngine::Math::Vector3(-1.0f, 1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset, 0)}, // 13
        Vertex{ImmersiveEngine::Math::Vector3(-offset, 0.0f, offset), ImmersiveEngine::Math::Vector3(-1.0f, 1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(0, 0)}, // 14
        Vertex{ImmersiveEngine::Math::Vector3(0, height, 0), ImmersiveEngine::Math::Vector3(-1.0f, 1.0f, 0.0f), defaultColor, ImmersiveEngine::Math::Vector2(offset / 2, offset * height)}, // 15
    };
    /*   
         height
           /\\   
          /  \ \   
         /====\=|
        /______\|  length
         length
    */
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