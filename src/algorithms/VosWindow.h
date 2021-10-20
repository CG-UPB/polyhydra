//
// Created by jan on 15.10.21.
//

#ifndef VOLUMESHOS_VOSWINDOW_H
#define VOLUMESHOS_VOSWINDOW_H

#include <iostream>
#include <OpenVolumeMesh/Mesh/HexahedralMesh.hh>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>
#include <list>
#include <map>
#include <string>

namespace vOS
{
/*
 * Used by the programmer to visualize OVM Meshes using volumeshos
 * Can be used to highlight specific elements, color specific parts of the mesh and apply other configerations
 * Provides Callback functions which communicate user inputs back to the algorithm
 *
 * TODO: Uses GeomtericPolyhedralMeshV3f exlusively at the moment, waiting for Wrapper Class
 * TODO: Allows a single Mesh Visualization at the moment
 */
class VosWindow {

    // Renamed Classes for convenience
    using v3f = OpenVolumeMesh::GeometricPolyhedralMeshV3f;
    typedef void(*void_callback)();
    typedef void(*button_callback)(int button_id, bool flanked);
    typedef void(*parameter_callback)(int double_id, double value);

public:
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////// Initialization ////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*
     * Constructor
     */
    VosWindow();
    /*
     * Deconstructor
     */
    ~VosWindow();

    /*
     * Opens a plain VolumeOs window. It will render any linked Meshes according to preset Preferences
     * Returns unique thread ID
     */
    int OpenWindow();

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////// Meshes /////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*
     * Links a single (v3f) OVM Mesh to be rendered by the Vos Window
     * Returns true on success, otherwise false (see Log for specific info)
     */
    bool LinkMesh(v3f* mesh);
    /*
     * Removes an already existing (v3f) OVM Mesh from the Vos Window. It will no longer be rendered or interacted with
     * Returns true on success, otherwise false  (see Log for specific info)
     */
    bool RemoveMesH(v3f* mesh);

    /*
     * Sets Callback Function which is called when the user presses the Pause Button inside Vos
     */
    void SetCallbackPauseActivated(void_callback vc);

    /*
     * Sets Callback Function which is called when the user releases the Pause Button inside Vos
     */
    void SetCallbackPauseDeactivated(void_callback vc);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////// Preset Interface ////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*
     * Asks whether Vos is ready for an algorithm side change to the linked meshes. Will be false if the user pressed the 'Pause' button, an internal
     * timer is not ready yet or some other underlying issue.
     */
    bool VosSideReady();

    /*
     * Asks whether Vos is paused
     */
    bool VosPauseActive();

    /*
     * Called when the User Pressed or Released the Pause Button
     */
    void PauseButtonFlank(bool is_pressed){if(is_pressed) OnPauseActivated(); else OnPauseDeactivated();};


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////// Custom Interface ////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AddCustomButton(std::string* button_name, bool default_value, bool stay_locked = false);
    void RemoveCustomButton(std::string* button_name);
    void AddCustomParameter(std::string* parameter_name, double default_value);
    void RemoveCustomParameter(std::string* parameter_name);



    /*
     * Called when a custom double parameter has been changed by the user
     */
    void OnParameterValueChanged(std::string* parameter_name, double value);
   // void ParameterValueChanged(int parameter_id, double value) { if(custom_parameter_callbacks.size() < parameter_id)  custom_button_callbacks[parameter_id](parameter_id, value);};
    /*
     * Called when a custom button has been changed by the user
     */
    //void ButtonValueChanged(int button_id, bool flanked) {ButtonCallback(button_id, flanked);};
    /*
     * Asks for the current state of a customly added button. True means the button id pressed, false means it is not pressed
     */
    bool AskButtonValue(int button_id){return false;}
    /*
     * Asks for the current value of a customly added double parameter.
     */
    double AskParameterValue(int parameter_id) {return 0;}

private:
    // Threading Variables
    static int static_thread_id;
    int thread_id = 0;

    v3f* m_mesh;

    // Callback functions
    static void default_callback_function() {std::cout << "Debug: Default Callback Function Called" << std::endl;};
    static void default_button_function(int button_id, bool flanked){};
    static void default_parameter_function(int parameter_id, double value){};

    /// Call Back Functions
    /// User Input Reactions
    // Called when Pause Button has been pressed and is active
    void_callback OnPauseActivated = default_callback_function;
    // Called when Pause Button has been released and is inactive
    void_callback OnPauseDeactivated = default_callback_function;
    // Called when Reset Button has been pressed
    void_callback OnResetPressed = default_callback_function;
    // Called when Step Button has been pressed
    void_callback OnStepPressed = default_callback_function;
    // Called when internal Step Timer has reached < 0
    void_callback OnStepTimerLap  = default_callback_function;

    // Custom GUI
    // The Amount of Custom Button added so far (includes deleted ones)
    int custom_button_total_amount = 0;
    // The Amount of Custom Parameters added so far (includes deleted ones)
    int custom_parameter_total_amount = 0;
    std::map<int, std::string*> custom_button_names;
    std::map<int, std::string*> custom_parameter_names;
    // List of Custom GUI Buttons. Called when flanked. True if Active, False otherwise
    std::list<button_callback>  custom_button_callbacks;
    // List of Custom GUI Parameters typed as doubles. Called when the value has been updated
    std::list<parameter_callback> custom_parameter_callbacks;


};

}


#endif //VOLUMESHOS_VOSWINDOW_H
