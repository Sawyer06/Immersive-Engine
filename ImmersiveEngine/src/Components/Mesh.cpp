#include"Mesh.h"

Mesh::Mesh(GLfloat* vertices, GLsizeiptr verticesSize, GLuint* indices, GLsizeiptr indicesSize) :
	m_VBO(vertices, verticesSize), m_EBO(indices, indicesSize),
	m_texture(nullptr), m_indexCount(indicesSize / sizeof(GLuint))
{
    m_VAO.Bind();
    m_EBO.Bind();
    /// Assign buffer objects to layout locations in the VAO (buffer object, layout index, number of elements).
    m_VAO.LinkAttrib(m_VBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0); // Vertex positions
    m_VAO.LinkAttrib(m_VBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Vertex color (decimal RGB)
    m_VAO.LinkAttrib(m_VBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture offset coordinates

    m_VAO.Unbind();
    m_VBO.Unbind();
}

Mesh::~Mesh()
{
    dump();
}

/// Draw the object to the screen.
void Mesh::draw(Shader& shaderProgram)
{
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
void Mesh::setTexture(Texture* texture)
{
    if (m_texture) // Texture already applied, replace it.
    {
        m_texture->Delete();
        m_texture = nullptr;
    }
    m_texture = texture;
}

void Mesh::dump()
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
}