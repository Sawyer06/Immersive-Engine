#ifndef FBO_CLASS_H
#define FBO_CLASS_H

#include<iostream>
#include<glad/glad.h>

class FBO
{
	public:
		GLuint ID;
		FBO();

		void Resize(GLuint width, GLuint height);
		void DrawScreen();
		void Bind();
		void Unbind();
		void Delete();
	private:
		GLuint m_width, m_height;
		GLuint m_RBO, m_texColorBuffer;
		GLuint m_screenVAO, m_screenVBO;

		void SetUp(GLuint width, GLuint height);
};
#endif
