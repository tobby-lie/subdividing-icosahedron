/*
 Tobby Lie
 Intro to Graphics HW 4
 4/6/20
*/

#define GL_SILENCE_DEPRECATION // silence deprecation warnings

#include <stdlib.h>
#include <math.h>
#include <GLUT/glut.h>
#include <tuple>
#include <vector>

#include "trackball.h"

#include <iostream>

#define X 0.525731112119133696
#define Z 0.850650808352039932

std::vector<float> up_vector = {0, 1, 0}; // up vector for glulookat
static bool dragging = false; // flag for dragging mouse if left mouse button pressed down

GLfloat angle = -150;   /* in degrees */
GLboolean doubleBuffer = GL_TRUE, iconic = GL_FALSE, keepAspect = GL_FALSE;
int spinning = 0, moving = 0;
int beginx, beginy;
int W = 300, H = 300;
float curquat[4];
float lastquat[4];
GLdouble bodyWidth = 3.0;
int newModel = 1;
int scaling;
float scalefactor = 1.0;

/* vertex data array */
static GLfloat vdata[12][3] =
{
    {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
    {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
    {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};

/* triangle indices */
static int tindices[20][3] =
{
    {1,4,0}, {4,9,0}, {4,5,9}, {8,5,4}, {1,8,4},
    {1,10,8}, {10,3,8}, {8,3,5}, {3,2,5}, {3,7,2},
    {3,10,7}, {10,6,7}, {6,11,7}, {6,0,11}, {6,1,0},
    {10,1,6}, {11,0,9}, {2,11,9}, {5,2,9}, {11,2,7}
};

GLfloat mat_specular[] = {0.633, 0.633, 0.633}; // specular property of material
GLfloat mat_diffuse[] = {0.9, 0.9, 0.9, 1.0}; // diffuse property of material
GLfloat mat_ambient[] = {1.0, 0.1, 0.1, 1.0}; // ambient property of material
GLfloat mat_shininess = 100; // shininess of material

GLfloat light_ambient[] = {0.0, 0.05, 0.0, 1.0}; // ambient property of light 1
GLfloat light_diffuse[] = {0.0, 1.0, 0.0, 1.0}; // diffuse property of light 1
GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0}; // specular property of light 1

GLfloat light_ambient2[] = {0.0, 0.0, 0.5, 1.0}; // ambient property of light 2
GLfloat light_diffuse2[] = {0.0, 0.0, 1.0, 1.0}; // diffuse property of light 2
GLfloat light_specular2[] = {1.0, 1.0, 1.0, 1.0}; // diffuse property of light 2

GLfloat light_position[] = {3.5, 0.6, 1.3, 0.0}; // position of light 1
GLfloat light_position2[] = {-3.5, -0.6, -1.3, 0.0}; // position of light 2

int flat = 1; // 0 = smooth shading, 1 = flat shading
int subdiv = 0; // number of subdivisions */

/*
    Function: normalize
 
    Description: Normalize a vector of non-zero length.
 
    Parameters: GLfloat v[3]
 
    Pre-Conditions: Vector should be populated with elements.
 
    Post-Conditions: Vector should be normalized.
 
    Returns: Nothing
*/
void normalize(GLfloat v[3])
{
    GLfloat d = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]); // calculate d

    // divide all components by d
    v[0] /= d;
    v[1] /= d;
    v[2] /= d;
} // end normalize

/*
    Function: normalized_cross_product
 
    Description: Normalized the cross product.
 
    Parameters: GLfloat u[3], GLfloat v[3], GLfloat n[3]
 
    Pre-Conditions: Vectors should be populated with elements.
 
    Post-Conditions: Cross product is performed then normalized.
 
    Returns: Nothing
*/
void normalized_cross_product (GLfloat u[3], GLfloat v[3], GLfloat n[3])
{
    // cross product
    n[0] = u[1]*v[2] - u[2]*v[1];
    n[1] = u[2]*v[0] - u[0]*v[2];
    n[2] = u[0]*v[1] - u[1]*v[0];

    normalize(n); // normalize result
} // end normalized_cross_product

/*
    Function: normalized_face
 
    Description: Normalize faces.
 
    Parameters: GLfloat v1[3], GLfloat v2[3], GLfloat v3[3]
 
    Pre-Conditions: Vectors should be populated with elements.
 
    Post-Conditions: Cross product is performed then normalized for faces.
 
    Returns: Nothing
*/
void normalize_face(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3])
{
    GLfloat d1[3], d2[3], n[3];
    int k;
    for (k = 0; k < 3; k++)
    {
        d1[k] = v1[k] - v2[k];
        d2[k] = v2[k] - v3[k];
    }
    normalized_cross_product(d1, d2, n);
    glNormal3fv(n);
} // end normalize_face

/*
    Function: draw_flat_triangle
 
    Description: Draw triangle based on vertices provided in flat shading mode.
 
    Parameters: GLfloat v1[3], GLfloat v2[3], GLfloat v3[3]
 
    Pre-Conditions: Vectors should be populated with elements.
 
    Post-Conditions: Triangle should be drawn based on vertices.
 
    Returns: Nothing
*/
void draw_flat_triangle(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3])
{
    glBegin(GL_TRIANGLES);
        normalize_face(v1, v2, v3); // normalize vertices first
        glVertex3fv(v1);
        glVertex3fv(v2);
        glVertex3fv(v3);
    glEnd();
} // end draw_flat triangle

/*
    Function: draw_triangle_smooth
 
    Description: Draw triangle based on normals.
 
    Parameters: GLfloat v1[3], GLfloat v2[3], GLfloat v3[3]
 
    Pre-Conditions: Vectors should be populated with elements.
 
    Post-Conditions: Triangle should be drawn based on vertices.
 
    Returns: Nothing
*/
void draw_triangle_smooth(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3])
{
    glBegin(GL_TRIANGLES);
        glNormal3fv(v1);
        glVertex3fv(v1);
        glNormal3fv(v2);
        glVertex3fv(v2);
        glNormal3fv(v3);
        glVertex3fv(v3);
    glEnd();
} // end draw_triangle_smooth

/*
    Function: subdivide
 
    Description: Recursively subdivides by depth times.
        Draw the triangles after.
 
    Parameters: GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int depth
 
    Pre-Conditions: Vectors should be populated with elements. Depth should have a value.
 
    Post-Conditions: Triangle should be drawn based on vertices and subdivisions.
 
    Returns: Nothing
*/
void subdivide(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int depth)
{
    GLfloat v12[3], v23[3], v31[3];
    int i;

    if(depth == 0)
    {
      if (flat == 1)
      {
          draw_flat_triangle(v1, v2, v3);
      }
      else
      {
          draw_triangle_smooth(v1, v2, v3);
      }
      return;
    }

    // calculate midpoints of each side
    for (i = 0; i < 3; i++)
    {
        v12[i] = (v1[i]+v2[i])/2.0;
        v23[i] = (v2[i]+v3[i])/2.0;
        v31[i] = (v3[i]+v1[i])/2.0;
    }
    // extrude midpoints to lie on unit sphere
    normalize(v12);
    normalize(v23);
    normalize(v31);

    // recursively subdivide new triangles
    subdivide(v1, v12, v31, depth-1);
    subdivide(v2, v23, v12, depth-1);
    subdivide(v3, v31, v23, depth-1);
    subdivide(v12, v23, v31, depth-1);
} // end subdivide

/*
    Function: mouse
 
    Description: Handle logic for mouse input.
 
    Parameters: int button, int state, int x, int y
 
    Pre-Conditions: None
 
    Post-Conditions: Update dragging flag and mouse positions.
 
    Returns: Nothing
*/
void mouse (int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) // if left mouse button held down, we are dragging
    {
        spinning = 0;
        glutIdleFunc(NULL);
        moving = 1;
        beginx = x;
        beginy = y;
        
        dragging = true;
    }
    else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) //  if left moues button released, we are not dragging
    {
        moving = 0;
        dragging = false;
    }
} // end mouse

/*
    Function: frame_buffer_coordinates
 
    Description: From the viewport, gets the frame buffer
    width and height.
 
    Parameters: None
 
    Pre-Conditions: None
 
    Post-Conditions: None
 
    Returns: Tuple of GLint, GLint. The first value being
    frame buffer width, second being frame buffer height.
 */
std::tuple<GLint, GLint> frame_buffer_coordinates()
{
    GLint dims[4] = {0};
    glGetIntegerv(GL_VIEWPORT, dims);
    GLint fbWidth = dims[2];
    GLint fbHeight = dims[3];
    
    return std::make_tuple(fbWidth, fbHeight);
} // end frame_buffer_coordinates

void
animate(void)
{
  add_quats(lastquat, curquat, curquat);
  newModel = 1;
  glutPostRedisplay();
}

/*
    Function: motion
 
    Description: Handles logic for motion of mouse.
 
    Parameters: int mousex, int mousey
 
    Pre-Conditions: None
 
    Post-Conditions: Update swing and elevation based on mouse motion.
 
    Returns: Nothing
*/
void motion (int mousex, int mousey)
{
    if (dragging) // if mouse is dragging then update swing and elevation accordingly
    {
        GLint fbWidth;
        GLint fbHeight;
        std::tie(fbWidth, fbHeight) = frame_buffer_coordinates();
        
        trackball(lastquat,
        (2.0 * beginx - fbWidth) / fbWidth,
        (fbHeight - 2.0 * beginy) / fbHeight,
        (2.0 * mousex - fbWidth) / fbWidth,
        (fbHeight - 2.0 * mousey) / fbHeight
        );
        beginx = mousex;
        beginy = mousey;
        spinning = 1;
        glutIdleFunc(animate);
    }
} // end motion

/*
    Function: display
 
    Description: Handles logic for display callback.
 
    Parameters: None
 
    Pre-Conditions: None
 
    Post-Conditions: Updates display based on set states.
 
    Returns: Nothing
*/
void display(void)
{
    if(newModel)
    {
        GLfloat m[4][4];
        
        glPopMatrix();
        glPushMatrix();
        build_rotmatrix(m, curquat);
        
        glMultMatrixf(&m[0][0]);
        
        newModel = 0;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    // for 20 faces of icosphere
    for (int i = 0; i < 20; i++)
    {
        // subdivide
        subdivide(&vdata[tindices[i][0]][0],
                  &vdata[tindices[i][1]][0],
                  &vdata[tindices[i][2]][0],
                  subdiv);
    } // end for

    glutSwapBuffers();
} // end display



/*
    Function: reshape
 
    Description: Handles logic for reshaping window and objects in scene.
 
    Parameters: int w, int h
 
    Pre-Conditions: None
 
    Post-Conditions: objects in window are rescaled.
 
    Returns: Nothing
*/
void reshape(int w, int h)
{
    GLint fbWidth;
    GLint fbHeight;
    std::tie(fbWidth, fbHeight) = frame_buffer_coordinates();

    glViewport (0, 0, (GLsizei) fbWidth, (GLsizei) fbHeight);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(65.0, (GLfloat) fbWidth/(GLfloat) fbHeight, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef (0.0, 0.0, -5.0);
} // end reshape

/*
    Function: init
 
    Description: Initializes components of lights and material.
 
    Parameters: None
 
    Pre-Conditions: None
 
    Post-Conditions: Properties for lighting and material set.
 
    Returns: Nothing
*/
void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);

    /* Set components for first light */
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    /* Set components for second light */
    glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular2);

    /* Set properties for material */
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialf(GL_LIGHT1, GL_SHININESS, mat_shininess);
} // end init

/*
    Function: main_menu_select
 
    Description: Handles logic for drop-down menu selection
 
    Parameters: int value
 
    Pre-Conditions: None
 
    Post-Conditions: menu options are executed
 
    Returns: Nothing
*/
void main_menu_select(int value)
{
    if (value == 666) // exit option
    {
        exit(0);
    } // end if
    else if (value == 1) // flat shading off
    {
        flat = 0;
    } // end else if
    else if (value == 2) // flat shading on
    {
        flat = 1;
    } // end else if
    else if (value == 4) // 1 subdivisions
    {
        subdiv = 1;
    } // end else if
    else if (value == 5) // 2 subdivisions
    {
        subdiv = 2;
    } // end else if
    else if (value == 6) // 3 subdivisions
    {
        subdiv = 3;
    } // end else if
    glutPostRedisplay();
} // end main_menu_select

/*
    Function: faces_select
 
    Description: Handles logic for subdivision faces menu selection
 
    Parameters: int id
 
    Pre-Conditions: None
 
    Post-Conditions: menu options are executed
 
    Returns: Nothing
*/
void faces_select(int id)
{
    // adjust subdivisions based on choice selected
    switch(id)
    {
        case 1:
            subdiv = 1;
            break;
        case 2:
            subdiv = 2;
            break;
        case 3:
            subdiv = 3;
            break;
        case 0:
            subdiv = 0;
            break;
        default:
            break;
    } // end switch
    glutPostRedisplay();
} // end faces_select

/*
    Function: right_light_toggle
 
    Description: Handles logic for right light on and off
 
    Parameters: int id
 
    Pre-Conditions: None
 
    Post-Conditions: menu options are executed
 
    Returns: Nothing
*/
void right_light_toggle(int id)
{
    switch(id)
    {
        case 0:
            glDisable(GL_LIGHT0); // turn off light
            break;
        case 1:
            glEnable(GL_LIGHT0); // turn on light
            break;
        default:
            break;
    } // end switch
    glutPostRedisplay();
} // end right_light_toggle

/*
    Function: left_light_toggle
 
    Description: Handles logic for left light on and off
 
    Parameters: int id
 
    Pre-Conditions: None
 
    Post-Conditions: menu options are executed
 
    Returns: Nothing
*/
void left_light_toggle(int id)
{
    switch(id)
    {
        case 0:
            glDisable(GL_LIGHT1); // turn off light
            break;
        case 1:
            glEnable(GL_LIGHT1); // turn on light
            break;
        default:
            break;
    } // end switch
    glutPostRedisplay();
} // end left_light_toggle

void
vis(int visible)
{
  if (visible == GLUT_VISIBLE) {
    if (spinning)
      glutIdleFunc(animate);
  } else {
    if (spinning)
      glutIdleFunc(NULL);
  }
}

int main(int argc, char **argv)
{
    // create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); /* single buffering */
    trackball(curquat, 0.0, 0.0, 0.0, 0.0);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Subdivide");

    // set callbacks
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutVisibilityFunc(vis);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    // initialize GL
    init();

    // menu for face selection
    int faces_menu = glutCreateMenu(faces_select);
    glutAddMenuEntry("20 Faces", 0);
    glutAddMenuEntry("80 Faces", 1);
    glutAddMenuEntry("320 Faces", 2);
    glutAddMenuEntry("1280 Faces", 3);

    // menu for right light toggle
    int right_light_menu = glutCreateMenu(right_light_toggle);
    glutAddMenuEntry("Toggle On", 1);
    glutAddMenuEntry("Toggle Off", 0);

    // menu for left light toggle
    int left_light_menu = glutCreateMenu(left_light_toggle);
    glutAddMenuEntry("Toggle On", 1);
    glutAddMenuEntry("Toggle Off", 0);

    // menu for main menu
    glutCreateMenu(main_menu_select);
    glutAddMenuEntry("Smooth Shading", 1);
    glutAddMenuEntry("Flat Shading", 2);
    glutAddSubMenu("Additional Faces", faces_menu);
    glutAddSubMenu("Toggle Right Light", right_light_menu);
    glutAddSubMenu("Toggle Left Light", left_light_menu);
    glutAddMenuEntry("Quit", 666);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    gluLookAt(0.0, 0.0, 10.0, 0, 0, 0, 0, 1, 0);

    glEnable(GL_LIGHTING); // enable lighting
    glEnable(GL_LIGHT0); // enable light 0
    glEnable(GL_LIGHT1); // enable light 1

    glEnable(GL_DEPTH_TEST); // enable depth test

    glutMainLoop();
    return(0);
}
