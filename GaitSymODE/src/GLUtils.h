/*
 *  GLUtils.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 26/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

#ifndef GLUtils_h
#define GLUtils_h

#include <ode/ode.h>

#if !defined(_WIN32)&& !defined(WIN32)
#include <gl.h>
#else
#include <qgl.h>
#include <windows.h>

// missing windows typedefs
typedef ptrdiff_t GLsizeiptr;

// typedef the function pointers for the missing functions
typedef void (WINAPI *FNWINDOWPOS2I)(GLint, GLint);
typedef void (WINAPI *FNDELETEBUFFERS)(GLsizei, const GLuint *);
typedef void (WINAPI *FNBINDBUFFER)(GLenum, GLuint);
typedef void (WINAPI *FNGENBUFFERS)(GLsizei, const GLuint *);
typedef void (WINAPI *FNBUFFERDATA)(GLenum target, GLsizeiptr, const GLvoid *, GLenum);

// define globals function pointers for the missing functions
#ifndef DEFINE_GLUTILS_GLOBALS
extern FNWINDOWPOS2I glWindowPos2i;
extern FNDELETEBUFFERS glDeleteBuffers;
extern FNBINDBUFFER glBindBuffer;
extern FNGENBUFFERS glGenBuffers;
extern FNBUFFERDATA glBufferData;
#else
FNWINDOWPOS2I glWindowPos2i;
FNDELETEBUFFERS glDeleteBuffers;
FNBINDBUFFER glBindBuffer;
FNGENBUFFERS glGenBuffers;
FNBUFFERDATA glBufferData;
#endif

// missing enum defines
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STATIC_DRAW 0x88E4

// set globals function pointers for the missing functions
void InitWindowsMissingGLFunctions();

#endif

#include "PGDMath.h"

struct Colour
{
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat alpha;

    void SetColour(GLfloat rf, GLfloat gf, GLfloat bf, GLfloat af) { r = rf;  g = gf;  b = bf;  alpha = af; };
    void SetColour(Colour &c) { r = c.r;  g = c.g;  b = c.b;  alpha = c.alpha; };
};

enum ColourMap
{
    GreyColourMap,
    CoolColourMap,
    HotColourMap,
    HSVColourMap,
    JetColourMap,
    ParulaColourMap
};

class GLUtils
{
public:

    static void DrawAxes(GLfloat x, GLfloat y, GLfloat z, GLfloat ox = 0, GLfloat oy = 0, GLfloat oz = 0);
    static void OutputText(GLfloat x, GLfloat y, GLfloat z, char *text, double size, int plane);
    static void SetColourFromMap(GLfloat index, ColourMap m, Colour *mappedColour, bool invert = false);
#ifdef USE_DEPRECATED_DRAWING_ROUTINES
    static void DrawCylinder(dVector3 p1, dVector3 p2, GLfloat radius, Colour &colour);
    static void DrawCylinder(dVector3 position, dVector3 axis, GLfloat length, GLfloat radius, Colour &colour, bool useAxisLengthScaling = false);
    static void DrawCylinder(dVector3 p, dMatrix3 R, double length, double radius, Colour &colour, double ox, double oy, double oz);
    static void DrawPath(pgd::Vector *pathCoordinates, int numPathCoordinates, double radius, Colour &colour);
#endif
};

// these are local versions of the required glu functions so we can remove that dependency
void gluOrtho2D(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);
void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
void gluLookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx,
      GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy,
      GLdouble upz);
void gluPickMatrix(GLdouble x, GLdouble y, GLdouble deltax, GLdouble deltay,
          GLint viewport[4]);
GLint gluProject(GLdouble objx, GLdouble objy, GLdouble objz,
          const GLdouble modelMatrix[16],
          const GLdouble projMatrix[16],
              const GLint viewport[4],
          GLdouble *winx, GLdouble *winy, GLdouble *winz);
GLint gluUnProject(GLdouble winx, GLdouble winy, GLdouble winz,
        const GLdouble modelMatrix[16],
        const GLdouble projMatrix[16],
                const GLint viewport[4],
            GLdouble *objx, GLdouble *objy, GLdouble *objz);
GLint gluUnProject4(GLdouble winx, GLdouble winy, GLdouble winz, GLdouble clipw,
          const GLdouble modelMatrix[16],
          const GLdouble projMatrix[16],
          const GLint viewport[4],
          GLclampd nearVal, GLclampd farVal,
          GLdouble *objx, GLdouble *objy, GLdouble *objz,
          GLdouble *objw);

#endif
