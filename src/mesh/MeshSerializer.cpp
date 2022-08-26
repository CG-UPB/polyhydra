
#include "MeshSerializer.h"
#include "nlohmann/json.hpp"

namespace volumeshOS::Internal
{
    void MeshSerializer::write_to_file(MeshObject& mesh, const std::string& file_name)
    {
        std::ofstream out_file{file_name};
        out_file << serialize(mesh);
        out_file.close();
    }

    void MeshSerializer::read_from_file(MeshObject& mesh, const std::string& file_name)
    {
        std::ifstream in_file(file_name);
        std::stringstream buffer;
        buffer << in_file.rdbuf();
        deserialize(mesh, buffer.str());
        in_file.close();
    }

    std::string MeshSerializer::serialize(MeshObject& mesh)
    {
        auto& data = mesh.get_data();
        nlohmann::json out;
        out["color"] = {
                {"r", data.color.r},
                {"g", data.color.g},
                {"b", data.color.b},
                {"a", data.color.a}
        };
        out["selection_color"] = {
                {"r", data.selection_color.r},
                {"g", data.selection_color.g},
                {"b", data.selection_color.b},
                {"a", data.selection_color.a}
        };
        out["ambient_strength"] = data.ambient_strength;
        out["diffuse_strength"] = data.diffuse_strength;
        out["specular_strength"] = data.specular_strength;
        out["specular_exponent"] = data.specular_exponent;

        out["peel_level"] = data.peel_level;
        out["max_peel_depth"] = data.max_peel_depth;
        out["slice_level"] = data.slice_level;
        out["slice_direction"] = {
                {"x", mesh.m_slice_dir.x},
                {"y", mesh.m_slice_dir.y},
                {"z", mesh.m_slice_dir.z}
        };
        out["slice_bb"] = {
                {"min", {
                        {"x", mesh.m_transformed_bb.first.x},
                        {"y", mesh.m_transformed_bb.first.y},
                        {"z", mesh.m_transformed_bb.first.z}
                }},
                {"max", {
                        {"x", mesh.m_transformed_bb.second.x},
                        {"y", mesh.m_transformed_bb.second.y},
                        {"z", mesh.m_transformed_bb.second.z}
                }}
        };
        out["cell_size"] = data.cell_size;
        out["slice_locked"] = data.slice_locked;
        out["rounding_active"] = data.rounding_active;
        out["rounding_size"] = data.rounding_size;

        out["position"] = {
                {"x", data.position.x},
                {"y", data.position.y},
                {"z", data.position.z}
        };
        out["scale"] = {
                {"x", data.scale.x},
                {"y", data.scale.y},
                {"z", data.scale.z}
        };
        out["rotation"] = {
                data.rotation[0][0], data.rotation[0][1], data.rotation[0][2], data.rotation[0][3],
                data.rotation[1][0], data.rotation[1][1], data.rotation[1][2], data.rotation[1][3],
                data.rotation[2][0], data.rotation[2][1], data.rotation[2][2], data.rotation[2][3],
                data.rotation[3][0], data.rotation[3][1], data.rotation[3][2], data.rotation[3][3]
        };
        out["transformation"] = {
                data.transformation[0][0], data.transformation[0][1], data.transformation[0][2], data.transformation[0][3],
                data.transformation[1][0], data.transformation[1][1], data.transformation[1][2], data.transformation[1][3],
                data.transformation[2][0], data.transformation[2][1], data.transformation[2][2], data.transformation[2][3],
                data.transformation[3][0], data.transformation[3][1], data.transformation[3][2], data.transformation[3][3]
        };

        out["visible"] = data.visible;
        return out.dump(2);
    }

    void MeshSerializer::deserialize(MeshObject& mesh, const std::string& json_string)
    {
        // we make a copy here, so we do not overwrite values which we don't serialize
        MeshData data = mesh.get_data();
        nlohmann::json in = nlohmann::json::parse(json_string);
        data.color = {
                in["color"]["r"],
                in["color"]["g"],
                in["color"]["b"],
                in["color"]["a"]
        };
        data.selection_color = {
                in["selection_color"]["r"],
                in["selection_color"]["g"],
                in["selection_color"]["b"],
                in["selection_color"]["a"]
        };
        data.ambient_strength = in["ambient_strength"];
        data.diffuse_strength = in["diffuse_strength"];
        data.specular_strength = in["specular_strength"];
        data.specular_exponent = in["specular_exponent"];

        data.peel_level = in["peel_level"];
        data.max_peel_depth = in["max_peel_depth"];
        data.slice_level = in["slice_level"];
        mesh.m_slice_dir = {
                in["slice_direction"]["x"],
                in["slice_direction"]["y"],
                in["slice_direction"]["z"],
        };
        mesh.m_transformed_bb = std::make_pair(
                glm::vec3{in["slice_bb"]["min"]["x"], in["slice_bb"]["min"]["y"], in["slice_bb"]["min"]["z"]},
                glm::vec3{in["slice_bb"]["max"]["x"], in["slice_bb"]["max"]["y"], in["slice_bb"]["max"]["z"]}
        );
        mesh.m_just_locked = false;
        data.cell_size = in["cell_size"];
        data.slice_locked = in["slice_locked"];
        data.rounding_active = in["rounding_active"];
        data.rounding_size = in["rounding_size"];
        data.visible = in["visible"];

        data.position = {
                in["position"]["x"],
                in["position"]["y"],
                in["position"]["z"]
        };
        data.scale = {
                in["scale"]["x"],
                in["scale"]["y"],
                in["scale"]["z"]
        };
        data.rotation = {
                {in["rotation"][0], in["rotation"][1], in["rotation"][2], in["rotation"][3]},
                {in["rotation"][4], in["rotation"][5], in["rotation"][6], in["rotation"][7]},
                {in["rotation"][8], in["rotation"][9], in["rotation"][10], in["rotation"][11]},
                {in["rotation"][12], in["rotation"][13], in["rotation"][14], in["rotation"][15]}
        };
        data.transformation = {
                {in["transformation"][0], in["transformation"][1], in["transformation"][2], in["transformation"][3]},
                {in["transformation"][4], in["transformation"][5], in["transformation"][6], in["transformation"][7]},
                {in["transformation"][8], in["transformation"][9], in["transformation"][10], in["transformation"][11]},
                {in["transformation"][12], in["transformation"][13], in["transformation"][14], in["transformation"][15]}
        };
        mesh.set_data(data);
    }
}