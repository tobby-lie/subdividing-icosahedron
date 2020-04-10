*******************************************************
*  Name      :  Tobby Lie
*  Student ID:  103385744
*  Class     :  CSCI 4565
*  HW#       :  4
*  Due Date  :  April 13, 2020
*******************************************************
*  File organization:
*    graphics_hw4:
*       main.cpp - All functions for
*           program contained within this file.
*       trackball.h - Header file for trackball code
*       trackball.cpp - Code for trackball functionality
*    graphics_hw4.xcodeproj - Xcode project file
*           that can run program.
*  Functional organization:
*       frame_buffer_coordinates - Gets frame buffer
*           width and height in tuple form.
*       mouse - Handles logic mouse input.
*       motion - Handles logic for motion.
*       display - Handles logic for display callback.
*       reshape - Handles logic for reshaping window.
*       init - Handles initialization.
*       normalize - normalizes vectors.
*       normalized_cross_product - performs cross
*           product and normalizes result.
*       normalized face - creates normalized faces.
*       draw_flat_triangle - draws triangle in flat
*           shading mode.
*       draw_triangle_smooth - draws triangle in
*           smooth shading mode.
*       subdivide - subdivides triangles based on
*           provided depth.
*       animate - animates rotation based on quaternions.
*  Controls:
*       Mouse drag - Rotates object based on quaternions.
*           Can drag and hold as well as release and object
*           will continue spinning.
*  Drop down menu:
*       Smooth shading - Enables smooth shading.
*       Flat shading - Enables flat shading.
*       Additional faces:
*           - 20, 80, 320, 1280 faces to choose from.
*       Toggle right light - toggle right light on
*           and off.
*       Toggle left light - toggle left light on and
*           off.
*       Quit - Exit application.
*******************************************************
*  Status of program:
*       Compiles and runs. Works completely.
*******************************************************
*   Hardware platform:
*       Tested on Macbook Pro 2019 in Xcode.
*******************************************************
