#include "MeshTextureBuffer.h"
#include "mesh/MeshProperties.h"


namespace volumeshOS::Internal
{

    MeshTextureBuffer::MeshTextureBuffer(const std::shared_ptr<Mesh>& mesh)
    : m_mesh(mesh)
    {
        // Create Texture Buffer
        glGenBuffers(1, &m_bezier_control_points_texture_buffer);
        // Create Texture
        glGenTextures(1, &m_bezier_control_points_texture);

        glBindBuffer(GL_TEXTURE_BUFFER, m_bezier_control_points_texture_buffer);
        glBindBuffer(GL_TEXTURE_BUFFER, 0);

        glActiveTexture(GL_TEXTURE12);
        glBindTexture(GL_TEXTURE_BUFFER, m_bezier_control_points_texture);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, m_bezier_control_points_texture_buffer);
        glBindTexture(GL_TEXTURE_BUFFER, 0);

        update_buffer();    
    }

    MeshTextureBuffer::~MeshTextureBuffer()
    {
        glDeleteBuffers(1, &m_bezier_control_points_texture_buffer);
        glDeleteTextures(1, &m_bezier_control_points_texture);
    }

    void MeshTextureBuffer::update_buffer()
    {
        if(*m_mesh->request_mesh_property<bool>(MeshProperties::PROP_IS_BEZIER)->begin())
        {
            OpenVolumeMesh::FacePropertyT<std::vector<double>> controlPointProp =
            m_mesh->request_face_property<std::vector<double>>(MeshProperties::PROP_BEZIER_FACE_CONTROL_POINTS);

            float* bezier_control_points_array = new float[controlPointProp.size()*(controlPointProp.begin()->size())];
            // iterate over values and copy them into m_bezier_control_points_array
            int i = 0;
            for (OpenVolumeMesh::FaceIter f_it = m_mesh->faces_begin(); f_it != m_mesh->faces_end(); ++f_it)
            {
                for(double cp_coord : controlPointProp[*f_it]) {
                    bezier_control_points_array[i] = (float)cp_coord;
                    i++;
                }
            }

            glBindBuffer(GL_TEXTURE_BUFFER, m_bezier_control_points_texture_buffer);
            glBufferData(GL_TEXTURE_BUFFER, sizeof(float)*controlPointProp.size()*(controlPointProp.begin()->size()), bezier_control_points_array, GL_STATIC_DRAW);
            glBindBuffer(GL_TEXTURE_BUFFER, 0);

        }
    }

    void MeshTextureBuffer::bind() const
    {
        glActiveTexture(GL_TEXTURE12);
        glBindTexture(GL_TEXTURE_BUFFER, m_bezier_control_points_texture);
    }

}