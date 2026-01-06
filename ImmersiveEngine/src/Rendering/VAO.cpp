#include"VAO.h"

/// Vertex Array Object
/// Stores the state of a collection of buffer objects and how to use them.

namespace ImmersiveEngine::Rendering
{
	VAO::VAO()
	{
		glGenVertexArrays(1, &ID);
	}

	void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
	{
		VBO.Bind();
		glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
		glEnableVertexAttribArray(layout);
		VBO.Unbind();
	}

	void VAO::Bind()
	{
		glBindVertexArray(ID);
	}

	void VAO::Unbind()
	{
		glBindVertexArray(0);
	}

	void VAO::Delete()
	{
		glDeleteVertexArrays(1, &ID);
	}
}