#include"MeshRenderer.h"

namespace ImmersiveEngine::cbs
{
	ImmersiveEngine::cbs::MeshRenderer::MeshRenderer(Object* obj) :
		Component(obj)
	{

	}

	ImmersiveEngine::cbs::MeshRenderer::MeshRenderer(Object* obj, std::shared_ptr<Rendering::Model> model) :
		Component(obj), m_model(model)
	{

	}

    void MeshRenderer::onUpdate()
    {
        drawNode(m_model->getRoot());
    }
	
    /// Process through each node in the tree and draw recursively.
    void MeshRenderer::drawNode(Rendering::ModelNode root)
    {
        // Draw all meshes in this node.
        for (Rendering::MeshMatPair pair : root.pairs)
        {
            drawPair(pair.mesh, pair.material);
        }

        // Continue to draw child nodes.
        for (Rendering::ModelNode childNode : root.children)
        {
            drawNode(childNode);
        }
    }

    /// Draw the mesh to the screen.
    void MeshRenderer::drawPair(std::shared_ptr<Rendering::Mesh> mesh, std::shared_ptr<Rendering::Material> material)
    {
        if (mesh->getVerticesCount() < 3)
        {
            std::cerr << "MESH_DRAW_ERROR not a polygon: mesh has less than 3 vertices.\n";
            return;
        }

        //shaderProgram.setFloat("scale", 1.0f);
        if (material->diffuseTexture)
        {
            material->shader.setInt("tex0", 0);
            material->shader.setFloat("scale", material->textureScale);
            material->shader.setVec2("offset", glm::vec2(material->textureOffset.x, material->textureOffset.y));
            material->shader.setBool("textured", true);
            material->diffuseTexture->Bind();
        }
        else
        {
            material->shader.setBool("textured", false);
        }
        material->shader.setFloat("shininess", material->shininess);
        material->shader.setVec3("mainColor", glm::vec3(material->mainColor.x, material->mainColor.y, material->mainColor.z));

        mesh->bind();

        if (ImmersiveEngine::Settings::g_showWireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        glDrawElements(GL_TRIANGLES, mesh->getIndicesCount(), GL_UNSIGNED_INT, 0);

        if (material->diffuseTexture)
        {
            material->diffuseTexture->Unbind();
        }
    }
}