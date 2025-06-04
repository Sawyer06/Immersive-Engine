#include"Present.h"

GLuint u_scaleLoc;
GLuint u_texturedLoc;
GLuint u_transformLoc;

unsigned int Present::nextId = 0;

Present::Present() :
    m_VBO(nullptr, 0), m_EBO(nullptr, 0),
    m_texture(nullptr), m_indexCount(0),
    name("Object"), space()
{
    id = nextId;
    nextId++;
}

Present::Present(GLfloat* vertices, GLsizeiptr verticesSize, GLuint* indices, GLsizeiptr indicesSize) :
    m_VBO(vertices, verticesSize), m_EBO(indices, indicesSize),
    m_texture(nullptr), m_indexCount(indicesSize / sizeof(GLuint)),
    name("Object"), space()
{
    id = nextId;
    nextId++;

    m_VAO.Bind();
    m_EBO.Bind();
    /// Assign buffer objects to layout locations in the VAO (buffer object, layout index, number of elements).
    m_VAO.LinkAttrib(m_VBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0); // Vertex positions
    m_VAO.LinkAttrib(m_VBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Vertex color (decimal RGB)
    m_VAO.LinkAttrib(m_VBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture offset coordinates

    m_VAO.Unbind();
    m_VBO.Unbind();
}

Present::~Present()
{
    dump();
}

/// Reinitialize object mesh as if it was just created.
void Present::reInitialize(GLfloat* vertices, GLsizeiptr verticesSize, GLuint* indices, GLsizeiptr indicesSize)
{
    m_VBO = VBO(vertices, verticesSize);
    m_EBO = EBO(indices, indicesSize);
    m_texture = nullptr;
    m_indexCount = indicesSize / sizeof(GLuint);
    name = "Object";
    Space space();

    m_VAO.Bind();
    /// Assign buffer objects to layout locations in the VAO (buffer object, layout index, number of elements).
    m_VAO.LinkAttrib(m_VBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0); // Vertex positions
    m_VAO.LinkAttrib(m_VBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Vertex color (decimal RGB)
    m_VAO.LinkAttrib(m_VBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture offset coordinates

    m_VAO.Unbind();
    m_VBO.Unbind();
    m_EBO.Unbind();
}

/// Draw the object to the screen.
void Present::draw(Shader& shaderProgram)
{
    //glm::mat4 matrix = glm::mat4(1.0f);
    //shaderProgram.setMat4("transform", space.matrix);
    shaderProgram.setFloat("scale", 1.0f);
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
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

    if (m_texture)
    {
        m_texture->Unbind();
    }
}

/// Set image texture on mesh.
void Present::setTexture(const char* filePath, GLenum format)
{
    if (m_texture) // Texture already applied, replace it.
    {
        m_texture->Delete();
        m_texture = nullptr;
    }
    m_texture = new Texture(filePath, GL_TEXTURE_2D, GL_TEXTURE0, format, GL_UNSIGNED_BYTE);
}

/// Memory management. Delete all used objects.
void Present::dump()
{
    m_VAO.Delete();
    m_VBO.Delete();
    m_EBO.Delete();
    if (m_texture)
    {
        m_texture->Delete();
        delete m_texture;
        m_texture = nullptr;
    };
    std::cout << "Object " << id << " destroyed successfully\n";
}

std::string Present::toString()
{
    std::ostringstream oss;
    oss << name << " (" << id << "): " << space.toString() << std::endl;
    return oss.str();
}

Present::Space::Space() : position(0, 0, 0), orientation(0, 0, 0), scale(1, 1, 1), m_matrix(glm::mat4(1.0f)) { }

/// Update the position, orientation, and scale of object based on its corresponding values.
void Present::Space::updateTransforms(Shader shaderProgram)
{
    m_matrix = glm::mat4(1.0f);

    m_matrix = glm::translate(m_matrix, glm::vec3(position.x, position.y, position.z));
    
    m_matrix = glm::rotate(m_matrix, glm::radians(orientation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_matrix = glm::rotate(m_matrix, glm::radians(orientation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m_matrix = glm::rotate(m_matrix, glm::radians(orientation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    m_matrix = glm::scale(m_matrix, glm::vec3(scale.x, scale.y, scale.z));

    shaderProgram.setMat4("transform", m_matrix);
}

void Present::Space::translate(Engine::Math::Vector3 deltaPos)
{
    position += deltaPos;
}
void Present::Space::translate(Engine::Math::Vector2 deltaPos)
{
    Engine::Math::Vector3 pos(deltaPos.x, deltaPos.y, 0);
    translate(pos);
}

void Present::Space::rotate(Engine::Math::Vector3 deltaAngle)
{
    orientation += deltaAngle;
}
void Present::Space::rotate(float deltaAngle)
{
    Engine::Math::Vector3 angle(0, 0, deltaAngle);
    rotate(angle);
}

void Present::Space::dialate(float scaleFactor)
{
    scale *= scaleFactor;
}
void Present::Space::dialate(Engine::Math::Vector3 scaleFactor)
{
    scale.x *= scaleFactor.x;
    scale.y *= scaleFactor.y;
    scale.z *= scaleFactor.z;
}
void Present::Space::dialate(Engine::Math::Vector2 scaleFactor)
{
    Engine::Math::Vector3 scale(scaleFactor.x, scaleFactor.y, 0);
    dialate(scale);
}

std::string Present::Space::toString()
{
    std::ostringstream oss;
    oss << "\nposition: " << position.toString() << "\norientation: " << orientation.toString() << "\nscale: " << scale.toString();
    return oss.str();
}