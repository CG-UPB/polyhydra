#include "volumeshOS.h"

using namespace volumeshOS;

int main(int argc, char* argv[])
{
    // check Input
    if(argc != 4)
    {
        std::cout << "Usage: ./main path resolution_width resolution_height"  << std::endl;
        return 1;
    }

    // some setup
    set_theme(volumeshOS::Theme::Dark);

    VMesh mesh;
    float steps = 1.0f;
    float max_steps = 0.0f;

    int frames = 60;
    float time = 20.0f;
    int rotations = 1;

    bool start = false;

    std::string path = argv[1];
    int width = atoi(argv[2]);
    int height = atoi(argv[3]);

    // this function gets called every frame
    on_gui_render([&](){
        ImGui::Begin("Video Renderer");

        // options
        ImGui::Text("Setup");
        ImGui::SliderInt("Frames",&frames, 1, 60);
        ImGui::SliderFloat("Time[s]", &time, 0.0f, 60.0f, "%.1f");
        ImGui::SliderInt("Rotations",&rotations, 1, 10);
        ImGui::Separator();

        // load mesh
        if (ImGui::Button("Load Mesh"))
        {
            mesh = load_from_dialog("Select OVM file");
            volumeshOS::set_camera_mode(CameraMode::ORBIT);
        }

        // start the application
        if(ImGui::Button("Start") && mesh.is_valid())
        {
            max_steps = (float)frames * time;
            volumeshOS::block_inputs(true);
            start = true;
        }

        if (start && mesh.is_valid())
        {
            // reset some values after max_steps is reached
            if(steps > max_steps)
            {
                steps = 0.0f;
                start = false;
                block_inputs(false);
                ImGui::End();

                // finally render video
                char command[200];
                sprintf(command, "yes | ffmpeg -r %i -f image2 -s %ix%i -i %simage%%d.png -vcodec libx264 -crf 15 -pix_fmt yuv420p video.mp4", frames, width, height, path.c_str());
                system(command);

                return;
            }

            // calculate next position to put the camera at
            auto mesh_pos = mesh.get_position<glm::vec3>();
            auto camera_pos = get_camera_position<glm::vec3>();
            auto distance = camera_pos - mesh_pos;
            auto d = glm::length(glm::vec2(distance.x, distance.z));

            float x = d * cos(steps * 2.0f * (float)rotations * PI / max_steps) + mesh_pos[0];
            float z = d * sin(steps * 2.0f * (float)rotations * PI / max_steps) + mesh_pos[2];

            set_camera_position(x, camera_pos.y, z);

            // specify export options for captured frame
            auto p = path + "image" + std::to_string((int)steps) + ".png";
            ExportOptions e = {};
            e.width = width;
            e.height = height;

            volumeshOS::export_image(p, e);
            log(p);

            steps += 1.0f;
        }
        ImGui::End();
    });

    open();

    return 0;
}
