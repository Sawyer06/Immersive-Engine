#include"FBO.h"

float screenVertices[] =
{
	 -1.0f,  1.0f,  0.0f, 1.0f,
	 -1.0f, -1.0f,  0.0f, 0.0f,
	  1.0f,  1.0f,  1.0f, 1.0f,

	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f
};

FBO::FBO() : m_width(1), m_height(1), m_RBO(0), m_texColorBuffer(0), m_screenVAO(0), m_screenVBO(0)
{
	glGenFramebuffers(1, &ID);

	glGenVertexArrays(1, &m_screenVAO);
	glGenBuffers(1, &m_screenVBO);
	glBindVertexArray(m_screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);

	SetUp(m_width, m_height);
}

void FBO::SetUp(GLuint width, GLuint height)
{
	Bind();

	glGenTextures(1, &m_texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, m_texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texColorBuffer, 0);

	glGenRenderbuffers(1, &m_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "FBO_ERROR framebuffer is not complete.\n";
	}
	//Unbind();
}

void FBO::Resize(GLuint width, GLuint height)
{
	if (width == m_width && height == m_height) return; // Return if this is already the size.
	m_width = width;
	m_height = height;

	SetUp(m_width, m_height);
}

void FBO::AttachExternalTexture(GLuint attachment, GLuint& image, GLuint width, GLuint height)
{
	//Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, image, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "FBO_ERROR Framebuffer is not complete." << std::endl;
	}
	//Unbind();
}

/// Draw frame buffer object to screen.
void FBO::DrawScreen()
{
	glBindTexture(GL_TEXTURE_2D, m_texColorBuffer);
	glBindVertexArray(m_screenVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

/// Bind custom framebuffer.
void FBO::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

/// Bind default framebuffer.
void FBO::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::Delete()
{
	glDeleteVertexArrays(1, &m_screenVAO);
	glDeleteBuffers(1, &m_screenVBO);
	glDeleteRenderbuffers(1, &m_RBO);
	glDeleteFramebuffers(1, &ID);
}