
#include "OpenVolumeMesh/Core/GeometryKernel.hh"
#include "OpenVolumeMesh/Core/OpenVolumeMeshHandle.hh"
#include "glm/exponential.hpp"
#include "glm/fwd.hpp"
#include "volumeshOS.h"

typedef std::map<OpenVolumeMesh::FaceHandle, std::vector<volumeshOS::VSphere>> Face_CPs_Map;
typedef std::map<OpenVolumeMesh::FaceHandle, std::vector<volumeshOS::VCylinder>> Face_Cylinder_Map;

std::map<int, float> mesh_ids_normalization_factors;

int CP_2D_INDEX_TO_1D(int i0, int i1, int m) 
{
    return ((m)+1)*((m)+2)/2 - ((m)-(i0)+1)*((m)-(i0)+2)/2 + (i1);
}

void add_control_points_to_vector(std::vector<volumeshOS::VSphere>& cps, volumeshOS::VMesh& mesh, OpenVolumeMesh::FaceHandle face, float scale, glm::vec4 color = glm::vec4(0.9, 0.2, 0.2, 1.0))
{
    auto& ovm = *mesh.get_ovm();
    auto control_points_prop = 
        ovm.request_face_property<std::vector<double>>("BezierFaceControlPoints");

    // Calculate normalization factor used in the shape shader.
    // This is used here to properly scale the control points.
    // Cache this value in a map, so that cps can also be scaled afterwards.
    float normalization_factor;
    auto norm_f_it = mesh_ids_normalization_factors.find(mesh.get_id());
    if( norm_f_it == mesh_ids_normalization_factors.end())
    {
        OpenVolumeMesh::Vec3d v0 = ovm.vertex(*ovm.vertices().first);
        glm::vec3 min = glm::vec3(v0[0], v0[1], v0[2]);
        glm::vec3 max = glm::vec3(v0[0], v0[1], v0[2]);
        for (auto v_it: ovm.vertices())
        {
            OpenVolumeMesh::Vec3d v = ovm.vertex(v_it);
            for(int i = 0; i < 3; i++)
            {
                if(min[i] > v[i])
                    min[i] = v[i];
                else if (max[i] < v[i])
                    max[i] = v[i];
            }
        }
        glm::vec3 diameter = max - min;
        normalization_factor = 7.0f / std::max(std::max(diameter.x, diameter.y), diameter.z);
        mesh_ids_normalization_factors[mesh.get_id()] = normalization_factor;
    }
    else {
    {
        normalization_factor = norm_f_it->second;
    }
    }

    for(auto coord_it = control_points_prop[face].begin(); coord_it != control_points_prop[face].end(); coord_it += 3)
    {
        auto control_point = mesh.add_shape<volumeshOS::VSphere>();
        control_point.set_position(*coord_it, *(coord_it+1), *(coord_it+2));
        control_point.set_scale(scale, scale/normalization_factor, scale);
        control_point.set_color(color);
        cps.push_back(control_point);
    }
}

void add_control_points_to_face(Face_CPs_Map& faces_cps, volumeshOS::VMesh& mesh, OpenVolumeMesh::FaceHandle face, float scale, glm::vec4 color = glm::vec4(0.9, 0.2, 0.2, 1.0))
{
    std::vector<volumeshOS::VSphere> cps;
    add_control_points_to_vector(cps, mesh, face, scale, color);
    faces_cps.emplace(face, cps);
}

void add_cp_connections_to_vector(std::vector<volumeshOS::VCylinder>& connections, volumeshOS::VMesh& mesh, OpenVolumeMesh::FaceHandle face, float scale, glm::vec4 color = glm::vec4(0.2, 0.2, 0.9, 1.0))
{
    auto& ovm = *mesh.get_ovm();
    auto control_points_prop = 
        ovm.request_face_property<std::vector<double>>("BezierFaceControlPoints");
    int m = *ovm.request_mesh_property<int>("BezierDegree")->begin();

    auto coord_start = control_points_prop[face].begin();

    std::vector<std::vector<int>> delta_i;
    delta_i.push_back({-1,  1,  0});
    delta_i.push_back({ 0,  1, -1});
    delta_i.push_back({ 1,  0, -1});

    
    for (int i0 = 0; i0 <= m; i0++)
    {
        for(int i1 = 0; i1 <= m-i0; i1++)
        {
            int i2 = m-i0-i1;
            // std::cout << "from: " << i0 << ", " << i1 << ", " << i2 << std::endl;

            auto from_coord = coord_start+CP_2D_INDEX_TO_1D(i0, i1, m)*3;
            glm::vec3 from_cp(*from_coord, *(from_coord+1), *(from_coord+2));

            std::vector<glm::vec3> to_cps;

            // iterate over all neighbor cps to which a connection
            // should be made (currently, each connection is double)
            for(auto& d_i : delta_i)
            {
                int d_i0 = d_i[0];
                int d_i1 = d_i[1];
                int d_i2 = d_i[2];

                int to_i0 = i0+(d_i0);
                int to_i1 = i1+(d_i1);
                int to_i2 = i2+(d_i2);
                if(to_i0 >= 0 && to_i1 >= 0 && to_i2 >= 0
                    && to_i0 <= m && to_i1 <= m && to_i2 <= m)
                {
                    // std::cout << " to: " << to_i0 << ", " << to_i1 << ", " << to_i2 << std::endl;
                    auto to_coord = coord_start+CP_2D_INDEX_TO_1D(to_i0, to_i1, m)*3;
                    to_cps.push_back(glm::vec3(*to_coord, *(to_coord+1), *(to_coord+2)));
                }
            }

            // std::cout << std::endl;
            for (auto to_cp : to_cps)
            {
                volumeshOS::VCylinder connection = mesh.add_shape<volumeshOS::VCylinder>();
                connection.set_position(from_cp + (to_cp-from_cp)/2.f);
                connection.set_scale(scale, glm::length(to_cp - from_cp), scale);
                glm::vec3 direction = glm::normalize(to_cp - from_cp);
                // if the direction is only in the x direction, add a small
                // divergence or the connection won't be visible
                if(direction.y == 0) direction.y = 0.000001f;
                connection.set_direction(direction);
                connection.set_color(color);
                connections.push_back(connection);
            }
        }
    }
}

void add_cp_connections_to_face(Face_Cylinder_Map& faces_connections, volumeshOS::VMesh& mesh, OpenVolumeMesh::FaceHandle face, float scale, glm::vec4 color = glm::vec4(0.2, 0.2, 0.9, 1.0))
{
    std::vector<volumeshOS::VCylinder> connections;
    add_cp_connections_to_vector(connections, mesh, face, scale, color);
    faces_connections.emplace(face, connections);
}

int main()
{
    set_theme(volumeshOS::Theme::Dark);
    
    bool show_control_net_on_selection = false;
    float cp_scale = 0.2f;
    float cp_connection_scale = 0.04f;
    glm::vec4 cp_color = glm::vec4(0.9, 0.2, 0.2, 1.0);
    glm::vec4 connection_color = glm::vec4(0.2, 0.2, 0.9, 1.0);

    std::map<int, Face_CPs_Map> meshes_selected_faces_cps;
    std::map<int, Face_Cylinder_Map> meshes_selected_faces_connections;
    std::map<int, std::vector<OpenVolumeMesh::FaceHandle>> meshes_is_selected;

    volumeshOS::on_face_select([&](volumeshOS::VMesh mesh, OpenVolumeMesh::FaceHandle face)
    {
        if(!show_control_net_on_selection) return;
        
        auto& ovm = *mesh.get_ovm();
        bool is_bezier_mesh = *ovm.request_mesh_property<bool>("IsBezierMesh").begin();
        
        if(is_bezier_mesh)
        {
            int mesh_id = mesh.get_id();
            if(meshes_is_selected.find(mesh_id) == meshes_is_selected.end())
            {
                meshes_is_selected.emplace(mesh_id, std::vector<OpenVolumeMesh::FaceHandle>());
                meshes_selected_faces_cps.emplace(mesh_id, Face_CPs_Map());
                meshes_selected_faces_connections.emplace(mesh_id, Face_Cylinder_Map());
            }

            auto control_points_prop =
                ovm.request_face_property<std::vector<double>>("BezierFaceControlPoints");
            auto& is_selected = meshes_is_selected.at(mesh_id);
            auto face_is_selected_it = std::find(is_selected.begin(), is_selected.end(), face);

            if(face_is_selected_it == is_selected.end())
            {
                // Face not yet selected, add control points and connections
                add_control_points_to_face(meshes_selected_faces_cps.at(mesh_id), mesh, face, cp_scale, cp_color);
                add_cp_connections_to_face(meshes_selected_faces_connections.at(mesh_id), mesh, face, cp_connection_scale, connection_color);
                is_selected.push_back(face);
            }
            else 
            {
                // Face was already selected, remove control points
                for(auto cp : meshes_selected_faces_cps.at(mesh_id).at(face))
                {
                    volumeshOS::remove_shape(cp);
                }
                meshes_selected_faces_cps.at(mesh_id).erase(face);
                // Also, remove the connections
                for(auto connection : meshes_selected_faces_connections.at(mesh_id).at(face))
                {
                    volumeshOS::remove_shape(connection);
                }
                meshes_selected_faces_connections.at(mesh_id).erase(face);
                is_selected.erase(face_is_selected_it);
            }
        }
    });

    volumeshOS::on_gui_render([&](){
        ImGui::Begin("MyPanel");
        if (ImGui::Button("Load Mesh"))
        {
            auto temp = volumeshOS::load_from_dialog("Select OVM file");
        }

        ImGui::Checkbox("Show control polygon when selected", &show_control_net_on_selection);

        if(ImGui::DragFloat("Control point size", &cp_scale, 0.001f, 0.0f, 4.0f, "%.3f"))
        {
            for(auto& mesh_faces_cps : meshes_selected_faces_cps)
            {
                for(auto& face_cps : mesh_faces_cps.second)
                {
                    float normalization_factor = mesh_ids_normalization_factors[mesh_faces_cps.first];
                    for (auto& cp : face_cps.second)
                    {
                        cp.set_scale(cp_scale, cp_scale/normalization_factor, cp_scale);
                    }
                }
            }
        }

        float cp_color_f[4];
        cp_color_f[0] = cp_color.r;
        cp_color_f[1] = cp_color.g;
        cp_color_f[2] = cp_color.b;
        cp_color_f[3] = cp_color.a;
        if (ImGui::ColorEdit4("Control Point Color", cp_color_f))
        {
            cp_color.r = cp_color_f[0];
            cp_color.g = cp_color_f[1];
            cp_color.b = cp_color_f[2];
            cp_color.a = cp_color_f[3];
            for(auto& mesh_faces_cps : meshes_selected_faces_cps)
            {
                for(auto& face_cps : mesh_faces_cps.second)
                {
                    for (auto& cp : face_cps.second)
                    {
                        cp.set_color(cp_color);
                    }
                }
            }
        }
        
        if(ImGui::DragFloat("Connection size", &cp_connection_scale, 0.001f, 0.0f, 2.0f, "%.3f"))
        {
            for(auto& mesh_faces_connections : meshes_selected_faces_connections)
            {
                for(auto& face_connections : mesh_faces_connections.second)
                {
                    for (auto& connection : face_connections.second)
                    {
                        glm::vec3 scale = connection.get_scale<glm::vec3>();
                        connection.set_scale(cp_connection_scale, scale.y, cp_connection_scale);
                    }
                }
            }
        }

        float connection_color_f[4];
        connection_color_f[0] = connection_color.r;
        connection_color_f[1] = connection_color.g;
        connection_color_f[2] = connection_color.b;
        connection_color_f[3] = connection_color.a;
        if (ImGui::ColorEdit3("Connection Color", connection_color_f))
        {
            connection_color.r = connection_color_f[0];
            connection_color.g = connection_color_f[1];
            connection_color.b = connection_color_f[2];
            connection_color.a = connection_color_f[3];
            for(auto& mesh_faces_connections : meshes_selected_faces_connections)
            {
                for(auto& face_connections : mesh_faces_connections.second)
                {
                    for (auto& connection : face_connections.second)
                    {
                        glm::vec3 scale = connection.get_scale<glm::vec3>();
                        connection.set_color(connection_color);
                    }
                }
            }
        }

        if (ImGui::Button("Remove shapes"))
        {
            volumeshOS::remove_shapes();

            // clear control points
            for(auto& mesh_faces_cps : meshes_selected_faces_cps)
            {
                for(auto& face_cps : mesh_faces_cps.second)
                {
                    face_cps.second.clear();
                }
                
            }
            for(auto& mesh_faces_cps : meshes_selected_faces_cps)
            {
                mesh_faces_cps.second.clear();
            }
            meshes_selected_faces_cps.clear();

            // clear Connections
            for(auto& mesh_faces_connections : meshes_selected_faces_connections)
            {
                for(auto& face_connections : mesh_faces_connections.second)
                {
                    face_connections.second.clear();
                }
            }
            for(auto& mesh_faces_connections : meshes_selected_faces_connections)
            {
                mesh_faces_connections.second.clear();
            }
            meshes_selected_faces_connections.clear();

            for(auto& mesh_is_selected : meshes_is_selected)
            {
                mesh_is_selected.second.clear();
            }
            meshes_is_selected.clear();

            volumeshOS::warn("Removed Shapes");
        }


        ImGui::End();
    });
    volumeshOS::use_log_window(false);
    volumeshOS::open();
}
