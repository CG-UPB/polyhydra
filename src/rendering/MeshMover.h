#pragma once

#include "vospch.h"

#include "./passes/SelectionHoverPass.h"
#include "Camera.h"

namespace vOS
{
/**
 * With the Mover, the User can select a mesh and drag it around the screen. The XY coordinates correspond to the cameras perspective. Using the Scrollwheel, the object
 * can also be moved along the local Z-Axis.
 */
    class MeshMover
    {
    public:
        void set_references(Camera* cam, SelectionHoverPass* sph)
        {
            m_camera = cam;
            m_hover_pass = sph;
        }

        void update();

    private:

        // Called when dragging is active
        void move_mesh();

        // References
        Camera* m_camera;
        SelectionHoverPass* m_hover_pass;

        // Anchor Mouse Position
        glm::vec3 m_anchor;
        // Current Mouse Position
        glm::vec3 m_mouse_position;
        // Position of the Mesh before dragging occured
        glm::vec3 m_mesh_original_position;

        // Mesh Object that's targeted. Even if the mouse leaves the objects space, or another object obstructs its view, we want to keep moving the object we started off with
        std::shared_ptr<MeshObject> m_target_mesh_object;

        // For noticing flanks from pressed to unpressed and vice versa
        bool m_mouse_press_flank = false;
        // Remember whether or not we are in dragging mode
        bool m_dragging = false;
        // How many unity should an object be moved along the z-axis when scrolling
        float m_scroll_strength = 0.25f;
        // How far has the user scrolled so far
        float m_scroll_value = 0.0f;
    };

}
