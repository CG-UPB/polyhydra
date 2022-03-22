
#include "MeshMover.h"
#include "../input/Input.h"
#include "../Window.h"


namespace vOS
{

    void MeshMover::update()
    {
        // Update Position
        m_mouse_position.x = Input::get_mouse_X();
        m_mouse_position.y = Input::get_mouse_Y();

        // Update Scroll Position
        m_scroll_value += Input::get_scroll_offset_Y() * m_scroll_strength;
        m_mouse_position.z = m_scroll_value;

        bool mouse_down = Input::mouse_pressed();
        bool move_mode_desired = Input::controll_pressed();

        if (mouse_down && !m_mouse_press_flank && move_mode_desired)
        {

            // In case there is only one mesh in the scene, we need not consider which mesh is meant when moving
            int mesh_id;

            if (Window::instance().get_mesh_list().size() == 1)
                mesh_id = Window::instance().get_mesh_focus();
            else
                mesh_id = m_hover_pass->get_hovered_mesh_object();

            if (mesh_id >= 0)
            {
                // Get MeshObject that's to be moved
                Window::instance().rendering_mutex.unlock();
                m_target_mesh_object = Window::instance().get_mesh_obj(mesh_id);
                Window::instance().rendering_mutex.lock();

                // Stop dragging, if mesh object does not exist
                if (m_target_mesh_object == nullptr)
                {
                    m_dragging = false;
                }
                else
                {
                    // Enter Dragging Mode
                    m_anchor = {Input::get_mouse_X(), Input::get_mouse_Y(), 0};
                    m_dragging = true;

                    // Reset Scroll value
                    m_scroll_value = 0.0f;
                    m_mouse_position.z = 0.0f;

                    m_mesh_original_position = m_target_mesh_object->get_data().position;
                }
            }
            else
            {
                // User likely clicked on void, in this case don't drag
                m_dragging = false;
            }

        }
        else if (!move_mode_desired || (!mouse_down && m_mouse_press_flank))
        {
            // Leave Dragging Mode
            m_dragging = false;
        }
        else if (m_dragging)
        {
            // Stay in Dragging Mode
            move_mesh();
        }

        // Set Flank to previous mouse input
        m_mouse_press_flank = mouse_down;
    }

    void MeshMover::move_mesh()
    {
        // Cautious nullpointer check
        if (m_target_mesh_object != nullptr)
        {
            // Mouse Coordinate difference
            glm::vec3 screen_difference = m_mouse_position - m_anchor;

            // Flip y difference
            screen_difference.y *= -1;

            // Calculate Distance from Camera to Object
            float distance = glm::length(m_camera->position - m_mesh_original_position);

            distance = distance < 0.01f ? 0.01f : distance;

            // Normalize Mouse Coordinates by screen size and distance from camera to object
            screen_difference = screen_difference / (m_camera->get_viewport_size() / distance);

            // Convert Mouse Coordinate to Camera Space
            glm::vec3 cam_space =
                    screen_difference.x * m_camera->get_right() + screen_difference.y * m_camera->get_up() +
                    screen_difference.z * m_camera->get_front();

            // Convert Camera Space to new World Position
            glm::vec3 new_position = m_mesh_original_position + cam_space;

            // Set World Position
            m_target_mesh_object->get_data().position = new_position;
        }
    }
}