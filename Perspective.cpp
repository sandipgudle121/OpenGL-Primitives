#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glew.h>    // This must be before <gl/gl.h>
#include <gl/GL.h>

#include "headers/Perspective.h"
#include "headers/vmath.h"
#include "headers/Sphere.h"
using namespace vmath;

#define STB_IMAGE_IMPLEMENTATION
#include "headers/stb_image.h"

// MACROS
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Check Image
#define CHECK_IMAGE_WIDTH 64
#define CHECK_IMAGE_HEIGHT 64
GLubyte checkImage[CHECK_IMAGE_HEIGHT][CHECK_IMAGE_WIDTH][4];

// Link with OpenGL Library
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "Sphere.lib")

// Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable Declaration
FILE *gpfile = NULL; // For File IO

// Global Variable Declaration for OpenGL
HDC ghdc = NULL;
HGLRC ghrc = NULL; // Create OpenGL context from device context

// Global Variable For Fullscreen of Window
HWND ghwnd = NULL;
DWORD dwStyle = 0;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};
BOOL gbFullScreen = FALSE;
int iResult = 0;
BOOL gbActive = FALSE;

HMENU hMenuBar;
HMENU hMenu;
HMENU sMenu;
HMENU perspectiveMenu;
HMENU singleShapeMenu;
HMENU colored2DShapeMenu;
HMENU coloredSingleShapeMenu;

HMENU animationMenu;
HMENU _2DAnimationMenu;
HMENU _3DAnimationMenu;

HMENU _2DWhiteAnimationMenu;
HMENU _2DColoredAnimationMenu;
HMENU _3DWhiteAnimationMenu;
HMENU _3DColoredAnimationMenu;

HMENU _2DWhiteSingleShapeAnimationMenu;
HMENU _2DColoredSingleShapeAnimationMenu;
HMENU _3DWhiteSingleShapeAnimationMenu;
HMENU _3DColoredSingleShapeAnimationMenu;

HMENU textureMenu;
HMENU _3DTextureMenu;
HMENU lightsMenu;
HMENU diffusedLightMenu;

// Menu Bar
#define ID_MENU_WHITE 1
#define ID_MENU_COLORED 2
#define ID_MENU_ANIMATION 3
#define ID_MENU_TEXTURES 4
#define ID_MENU_LIGHTS 5
#define ID_MENU_EXIT 6

// Sub Menu
#define ID_MENU_ORTHO_W_TRIANGLE 7
#define ID_MENU_PERSPECTIVE_WHITE_TRIANGLE 8
#define ID_MENU_PERSPECTIVE_WHITE_QUAD 9
#define ID_MENU_PERSPECTIVE_TWO_WHITE_SHAPES 10

#define ID_MENU_COLORED_TRIANGLE 11
#define ID_MENU_COLORED_QUAD 12
#define ID_MENU_COLORED_TWO_2D_SHAPES 13

#define ID_MENU_WHITE_2D_ANIMATING_TRIANGLE 14
#define ID_MENU_WHITE_2D_ANIMATING_QUAD 15
#define ID_MENU_WHITE_2D_ANIMATING_TRIANGLE_AND_QUAD 16

#define ID_MENU_COLORED_2D_ANIMATING_TRIANGLE 17
#define ID_MENU_COLORED_2D_ANIMATING_QUAD 18
#define ID_MENU_COLORED_2D_ANIMATING_TRIANGLE_AND_QUAD 19

#define ID_MENU_WHITE_PYRAMID 20
#define ID_MENU_WHITE_CUBE 21
#define ID_MENU_WHITE_PYRAMID_AND_CUBE 22

#define ID_MENU_COLORED_PYRAMID 23
#define ID_MENU_COLORED_CUBE 24
#define ID_MENU_COLORED_PYRAMID_AND_CUBE 25
#define ID_MENU_SPHERE 26

#define ID_MENU_SMILEY_TEXTURE 27
#define ID_MENU_PYRAMID_TEXTURE 28
#define ID_MENU_CUBE_TEXTURE 29
#define ID_MENU_CHECKERBOARD_TEXTURE 30
#define ID_MENU_PYRAMID_AND_CUBE_TEXTURE 31

// Lights Related
#define ID_MENU_DIFFUSE_LIGHT_SPHERE 32
#define ID_MENU_DIFFUSE_LIGHT_PYRAMID 33
#define ID_MENU_DIFFUSE_LIGHT_CUBE 34
#define ID_MENU_PER_VERTEX_LIGHT 35
#define ID_MENU_PER_FRAGMENT_LIGHT 36
#define ID_MENU_THREE_LIGHTS 37

// For Menu Toggling
BOOL IsMenuVisible = FALSE;

// Light Toggle
BOOL IsLightEnabled = FALSE;

// Animation Angle
GLfloat animationAngle = 0.0f;
GLfloat cubeAngle = 0.0f;
GLfloat ligthAnimationAngle = 0.0f;
BOOL bResult;

// OpenGL related variables
GLuint shaderProgramObject = 0;
GLuint shaderProgramObject_texturing = 0;
GLuint shaderProgramObject_diffusedLight = 0;
GLuint shaderProgramObject_perVertexLight = 0;
GLuint shaderProgramObject_perFragmentLight = 0;
GLuint shaderProgramObject_threeLights = 0;
GLuint shaderProgramObject_cubeMap = 0;

// Attribute Enum
enum
{
    SSG_ATTRIBUTE_POSITION = 0,
    SSG_ATTRIBUTE_COLOR,
    SSG_ATTRIBUTE_TEXCOORD,
    SSG_ATTRIBUTE_ELEMENT,
    SSG_ATTRIBUTE_NORMAL
};

// VAO and VBO for 2D Shapes
GLuint VAO_WHITE_TRIANGLE = 0;
GLuint VAO_ORTHO_WHITE_TRIANGLE = 0;
GLuint VAO_ORTHO_COLORED_TRIANGLE = 0;
GLuint VAO_COLOR_TRIANGLE = 0;

GLuint VBO_ORTHO_POS_TRIANGLE = 0;
GLuint VBO_POS_TRIANGLE = 0;

GLuint VBO_COLOR_TRIANGLE = 0;
GLuint VBO_WHITE_TRIANGLE = 0;

GLuint VAO_WHITE_QUAD = 0;
GLuint VAO_COLOR_QUAD = 0;

GLuint VBO_POS_QUAD = 0;

GLuint VBO_COLOR_QUAD = 0;
GLuint VBO_WHITE_QUAD = 0;

// Texture VAO and VBO
GLuint VAO_TEXTUREQUAD = 0;
GLuint VBO_TEXTUREQUAD_POS = 0;

// VAO and VBO for Cube
GLuint VAO_WHITE_CUBE = 0;
GLuint VAO_COLORED_CUBE = 0;
GLuint VBO_CUBE_POS = 0;
GLuint VBO_WHITE_CUBE = 0;
GLuint VBO_COLORED_CUBE = 0;

// VAO and VBO for Pyramid
GLuint VAO_WHITE_PYRAMID = 0;
GLuint VAO_COLORED_PYRAMID = 0;
GLuint VBO_PYRAMID_POS = 0;
GLuint VBO_WHITE_PYRAMID = 0;
GLuint VBO_COLORED_PYRAMID = 0;

// VBO TexCoord
GLuint VBO_QUAD_TEX = 0;
GLuint VBO_PYRAMID_TEX = 0;
GLuint VBO_CUBE_TEX = 0;

// Texture Variables
GLuint cubeMapTexture = 0;
GLuint TwoDTexture = 0;
GLuint stoneTexture = 0;
GLuint marbleTexture = 0;
GLuint checkerboardTexture = 0;

// Light VBOs
GLuint VBO_CUBE_NORMAL = 0;
GLuint VBO_PYRAMID_NORMAL = 0;

// Light Values
GLfloat materialDiffused_DifL[] = {0.5f, 0.5f, 0.5f, 1.0f}; // Material
GLfloat lightDiffused_DifL[] = {1.0f, 1.0f, 1.0f, 1.0f};    // white diffused light
GLfloat lightPosition_DifL[] = {0.0f, 0.0f, 2.0f, 1.0f};

// Sphere Variables
GLuint vao_sphere = 0;
GLuint vbo_sphere_position = 0;
GLuint vbo_sphere_normal = 0;
GLuint vbo_sphere_element = 0;

GLuint NumVertices = 0;
GLuint NumElements = 0;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

// Uniforms
GLuint modelViewMatrixUniform = 0;
GLuint projectionMatrixUniform_DifL = 0;
GLuint lightDiffusedUniform = 0;
GLuint materialDiffusedUniform = 0;
GLuint lightPositionUniform = 0;
GLuint keyPressedUniform_Difl = 0;

GLuint mvpMatrixUniform = 0;
GLuint mvpMatrixUniformCubeMap = 0;
GLuint textureSamplerUniform = 0;
GLuint textureSamplerUniformCubeMap = 0;
mat4 perspectiveProjectionMatrix;
mat4 orthographicProjectionMatrix;

// Per Vertex Light Variables
GLuint modelMatrixUniform = 0;
GLuint viewMatrixUniform = 0;
GLuint projectionMatrixUniform = 0;
GLuint keyPressedUniform = 0;

GLuint light_Diffused_Uniform = 0;
GLuint light_Ambient_Uniform = 0;
GLuint light_Position_Uniform = 0;
GLuint light_Specular_Uniform = 0;

GLuint material_Diffused_Uniform = 0;
GLuint material_Ambient_Uniform = 0;
GLuint material_Specular_Uniform = 0;
GLuint material_Shininess_Uniform = 0;

GLfloat lightPosition[] = {100.0f, 100.0f, 100.0f, 1.0f};
GLfloat lightAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f};
GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f};
GLfloat lightDiffused[] = {1.0f, 1.0f, 1.0f, 1.0f};

GLfloat materialDiffused[] = {0.5f, 0.2f, 0.7f, 1.0f};
GLfloat materialAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat materialSpecular[] = {0.7f, 0.7f, 0.7f, 1.0f};

GLfloat materialShininess = 128.0f;

// Three Lights
GLuint light_Diffused_Uniform_3L[3];
GLuint light_Ambient_Uniform_3L[3];
GLuint light_Position_Uniform_3L[3];
GLuint light_Specular_Uniform_3L[3];

GLuint modelMatrixUniform_3L = 0;
GLuint viewMatrixUniform_3L = 0;
GLuint projectionMatrixUniform_3L = 0;

GLuint material_Diffused_Uniform_3L = 0;
GLuint material_Ambient_Uniform_3L = 0;
GLuint material_Specular_Uniform_3L = 0;
GLuint material_Shininess_Uniform_3L = 0;

struct Light
{
    vec4 ambient;
    vec4 diffused;
    vec4 specular;
    vec4 position;
};

struct Light light[3];

GLfloat materialDiffused_3L[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialAmbient_3L[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat materialSpecular_3L[] = {1.0f, 1.0f, 1.0f, 1.0f};

// Reset the Rendering
typedef enum
{
    NONE,
    IsWhiteOrthoTriangle,
    IsColoredTriangle,
    IsWhiteTriangle,
    IsColoredQuad,
    IsWhiteQuad,
    IsColoredTriangleAndQuad,
    IsWhiteTriangleAndQuad,
    IsWhite2DAnimatingTriangle,
    IsWhite2DAnimatingQuad,
    IsWhite2DAnimatingTriangleAndQuad,
    IsColored2DAnimatingTriangle,
    IsColored2DAnimatingQuad,
    IsColored2DAnimatingTriangleAndQuad,
    IsWhitePyramid,
    IsWhiteCube,
    IsWhitePyramidAndCube,
    IsColoredPyramid,
    IsColoredCube,
    IsColoredPyramidAndCube,
    IsSphere,
    IsSmileyTexture,
    IsPyramidTexture,
    IsCubeTexture,
    IsPyraamidAndCubeTexture,
    IsCheckerboradTexture,

    IsDiffusedLightCube,
    IsDiffusedLightPyramid,
    IsDiffusedLightSphere,
    IsPerVertexLight,
    IsPerFragmentLight,
    IsThreeLights,
}   RenderMode;
RenderMode currentMode = NONE;

// Entry-Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
    // Function Declarations
    int initialize(void);
    void uninitialize(void);
    void display(void);
    void update(void);
    void ToggleFullscreen(void);

    // Local Variable Declarations
    WNDCLASSEX wndclass;
    HWND hwnd;
    MSG msg;
    TCHAR szAppName[] = TEXT("SG_Window");

    // For Avtive Window
    BOOL bDone = FALSE;

    // Code
    if (fopen_s(&gpfile, "Log.txt", "w") != 0)
    {
        MessageBox(NULL, TEXT("log file cannot be opened"),
                   TEXT("Error"), MB_OK | MB_ICONERROR);
        exit(0);
    }
    fprintf(gpfile, "Program Started Succesfully\n");

    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInstance;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(LEAF_ICON));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.lpszClassName = szAppName;
    wndclass.lpszMenuName = NULL;
    wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(LEAF_ICON));

    RegisterClassEx(&wndclass);

    // Create Menus
    hMenuBar = CreateMenu();
    hMenu = CreateMenu();
    sMenu = CreateMenu();
    perspectiveMenu = CreateMenu();
    singleShapeMenu = CreateMenu();
    colored2DShapeMenu = CreateMenu();
    coloredSingleShapeMenu = CreateMenu();
    animationMenu = CreateMenu();

    _2DAnimationMenu = CreateMenu();
    _2DWhiteAnimationMenu = CreateMenu();
    _2DColoredAnimationMenu = CreateMenu();
    _2DWhiteSingleShapeAnimationMenu = CreateMenu();
    _2DColoredSingleShapeAnimationMenu = CreateMenu();

    _3DAnimationMenu = CreateMenu();
    _3DWhiteAnimationMenu = CreateMenu();
    _3DColoredAnimationMenu = CreateMenu();
    _3DWhiteSingleShapeAnimationMenu = CreateMenu();
    _3DColoredSingleShapeAnimationMenu = CreateMenu();

    textureMenu = CreateMenu();
    _3DTextureMenu = CreateMenu();
    lightsMenu = CreateMenu();
    diffusedLightMenu = CreateMenu();

    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hMenu, L"&White");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)colored2DShapeMenu, L"&Colored");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)animationMenu, L"&Animation");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)textureMenu, L"&Textures");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)lightsMenu, L"&Lights");
    AppendMenuW(hMenuBar, MF_STRING, ID_MENU_EXIT, L"&Exit");

    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)sMenu, L"&Ortho");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)perspectiveMenu, L"&Perspective");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SPHERE, L"&Sphere");

    AppendMenuW(sMenu, MF_STRING, ID_MENU_ORTHO_W_TRIANGLE, L"&Ortho Triangle");
    AppendMenuW(perspectiveMenu, MF_POPUP, (UINT_PTR)singleShapeMenu, L"&One Shape");
    AppendMenuW(perspectiveMenu, MF_STRING, ID_MENU_PERSPECTIVE_TWO_WHITE_SHAPES, L"&Two Shapes");

    AppendMenuW(singleShapeMenu, MF_STRING, ID_MENU_PERSPECTIVE_WHITE_TRIANGLE, L"&Triangle");
    AppendMenuW(singleShapeMenu, MF_STRING, ID_MENU_PERSPECTIVE_WHITE_QUAD, L"&Quad");

    AppendMenuW(colored2DShapeMenu, MF_POPUP, (UINT_PTR)coloredSingleShapeMenu, L"&One Shape");
    AppendMenuW(colored2DShapeMenu, MF_STRING, ID_MENU_COLORED_TWO_2D_SHAPES, L"&Two Shapes");

    AppendMenuW(coloredSingleShapeMenu, MF_STRING, ID_MENU_COLORED_TRIANGLE, L"&Triangle");
    AppendMenuW(coloredSingleShapeMenu, MF_STRING, ID_MENU_COLORED_QUAD, L"&Quad");

    AppendMenuW(animationMenu, MF_POPUP, (UINT_PTR)_2DAnimationMenu, L"&2D");
    AppendMenuW(animationMenu, MF_POPUP, (UINT_PTR)_3DAnimationMenu, L"&3D");

    // 2D
    AppendMenuW(_2DAnimationMenu, MF_POPUP, (UINT_PTR)_2DWhiteAnimationMenu, L"&White");
    AppendMenuW(_2DAnimationMenu, MF_POPUP, (UINT_PTR)_2DColoredAnimationMenu, L"&Colored");

    AppendMenuW(_2DWhiteAnimationMenu, MF_POPUP, (UINT_PTR)_2DWhiteSingleShapeAnimationMenu, L"&One Shape");
    AppendMenuW(_2DWhiteAnimationMenu, MF_POPUP, ID_MENU_WHITE_2D_ANIMATING_TRIANGLE_AND_QUAD, L"&Two Shapes");

    AppendMenuW(_2DWhiteSingleShapeAnimationMenu, MF_STRING, ID_MENU_WHITE_2D_ANIMATING_TRIANGLE, L"&Triangle");
    AppendMenuW(_2DWhiteSingleShapeAnimationMenu, MF_STRING, ID_MENU_WHITE_2D_ANIMATING_QUAD, L"&Quad");

    AppendMenuW(_2DColoredAnimationMenu, MF_POPUP, (UINT_PTR)_2DColoredSingleShapeAnimationMenu, L"&One Shape");
    AppendMenuW(_2DColoredAnimationMenu, MF_POPUP, ID_MENU_COLORED_2D_ANIMATING_TRIANGLE_AND_QUAD, L"&Two Shapes");

    AppendMenuW(_2DColoredSingleShapeAnimationMenu, MF_STRING, ID_MENU_COLORED_2D_ANIMATING_TRIANGLE, L"&Triangle");
    AppendMenuW(_2DColoredSingleShapeAnimationMenu, MF_STRING, ID_MENU_COLORED_2D_ANIMATING_QUAD, L"&Quad");

    // 3D
    AppendMenuW(_3DAnimationMenu, MF_POPUP, (UINT_PTR)_3DWhiteAnimationMenu, L"&White");
    AppendMenuW(_3DAnimationMenu, MF_POPUP, (UINT_PTR)_3DColoredAnimationMenu, L"&Colored");

    AppendMenuW(_3DWhiteAnimationMenu, MF_POPUP, (UINT_PTR)_3DWhiteSingleShapeAnimationMenu, L"&One Shape");
    AppendMenuW(_3DWhiteAnimationMenu, MF_POPUP, ID_MENU_WHITE_PYRAMID_AND_CUBE, L"&Two Shapes");

    AppendMenuW(_3DWhiteSingleShapeAnimationMenu, MF_STRING, ID_MENU_WHITE_PYRAMID, L"&Pyramid");
    AppendMenuW(_3DWhiteSingleShapeAnimationMenu, MF_STRING, ID_MENU_WHITE_CUBE, L"&Cube");

    AppendMenuW(_3DColoredAnimationMenu, MF_POPUP, (UINT_PTR)_3DColoredSingleShapeAnimationMenu, L"&One Shape");
    AppendMenuW(_3DColoredAnimationMenu, MF_POPUP, ID_MENU_COLORED_PYRAMID_AND_CUBE, L"&Two Shapes");

    AppendMenuW(_3DColoredSingleShapeAnimationMenu, MF_STRING, ID_MENU_COLORED_PYRAMID, L"&Pyramid");
    AppendMenuW(_3DColoredSingleShapeAnimationMenu, MF_STRING, ID_MENU_COLORED_CUBE, L"&Cube");

    // Textures
    AppendMenuW(textureMenu, MF_STRING, ID_MENU_SMILEY_TEXTURE, L"&2D Texture");
    AppendMenuW(textureMenu, MF_POPUP, (UINT_PTR)_3DTextureMenu, L"&3D Texture");
    AppendMenuW(textureMenu, MF_STRING, ID_MENU_CHECKERBOARD_TEXTURE, L"&Checkerboard");

    AppendMenuW(_3DTextureMenu, MF_STRING, ID_MENU_PYRAMID_TEXTURE, L"&Pyramid");
    AppendMenuW(_3DTextureMenu, MF_STRING, ID_MENU_CUBE_TEXTURE, L"&Cube");
    AppendMenuW(_3DTextureMenu, MF_STRING, ID_MENU_PYRAMID_AND_CUBE_TEXTURE, L"&PyramidAndCube");

    // Lights
    AppendMenuW(lightsMenu, MF_POPUP, (UINT_PTR)diffusedLightMenu, L"&Diffused Light");
    AppendMenuW(lightsMenu, MF_STRING, ID_MENU_PER_VERTEX_LIGHT, L"&Per Vertex");
    AppendMenuW(lightsMenu, MF_STRING, ID_MENU_PER_FRAGMENT_LIGHT, L"&Per Fragment");
    AppendMenuW(lightsMenu, MF_STRING, ID_MENU_THREE_LIGHTS, L"&Three Lights");
    AppendMenuW(diffusedLightMenu, MF_STRING, ID_MENU_DIFFUSE_LIGHT_SPHERE, L"&Sphere");
    AppendMenuW(diffusedLightMenu, MF_STRING, ID_MENU_DIFFUSE_LIGHT_PYRAMID, L"&Pyramid");
    AppendMenuW(diffusedLightMenu, MF_STRING, ID_MENU_DIFFUSE_LIGHT_CUBE, L"&Cube");

    // Create Window
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);
    int xw = w / 2 - 400;
    int yh = h / 2 - 300;

    hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName,
                          TEXT("Sandip Sham Gudle"),
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
                          xw,
                          yh,
                          WIN_WIDTH,
                          WIN_HEIGHT,
                          NULL,
                          hMenuBar,
                          NULL,
                          NULL);
    ghwnd = hwnd;

    // Initialization
    iResult = initialize();
    if (iResult != 0)
    {
        MessageBox(hwnd, TEXT("initialization failed"),
                   TEXT("Error"), MB_OK | MB_ICONERROR);
        DestroyWindow(hwnd);
    }

    // Show Window
    ShowWindow(hwnd, iCmdShow);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    // Make fullscreen at start
    ToggleFullscreen();
    gbFullScreen = TRUE;

    // Message Loop
    // Game Loop
    while (bDone == FALSE)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                bDone = TRUE;
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            if (gbActive == TRUE)
            {
                // Render
                display();

                // Update
                update();
            }
        }
    }

    // Uninitialization
    uninitialize();
    return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    // Function Declaration
    void ToggleFullscreen(void);
    void resize(int, int);

    // Code
    switch (iMsg)
    {
    case WM_SETFOCUS:
        gbActive = TRUE;
        break;

    case WM_KILLFOCUS:
        gbActive = FALSE;
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_MENU_ORTHO_W_TRIANGLE:
            currentMode = IsWhiteOrthoTriangle;
            break;

        case ID_MENU_PERSPECTIVE_WHITE_TRIANGLE:
            currentMode = IsWhiteTriangle;
            break;

        case ID_MENU_PERSPECTIVE_WHITE_QUAD:
            currentMode = IsWhiteQuad;
            break;

        case ID_MENU_PERSPECTIVE_TWO_WHITE_SHAPES:
            currentMode = IsWhiteTriangleAndQuad;
            break;

        case ID_MENU_COLORED_TRIANGLE:
            currentMode = IsColoredTriangle;
            break;

        case ID_MENU_COLORED_QUAD:
            currentMode = IsColoredQuad;
            break;

        case ID_MENU_COLORED_TWO_2D_SHAPES:
            currentMode = IsColoredTriangleAndQuad;
            break;

            // -------------------- White -----------------------
        case ID_MENU_WHITE_2D_ANIMATING_TRIANGLE:
            currentMode = IsWhite2DAnimatingTriangle;
            break;

        case ID_MENU_WHITE_2D_ANIMATING_QUAD:
            currentMode = IsWhite2DAnimatingQuad;
            break;

        case ID_MENU_WHITE_2D_ANIMATING_TRIANGLE_AND_QUAD:
            currentMode = IsWhite2DAnimatingTriangleAndQuad;
            break;

            // ------------------ Colored -----------------------
        case ID_MENU_COLORED_2D_ANIMATING_TRIANGLE:
            currentMode = IsColored2DAnimatingTriangle;
            break;

        case ID_MENU_COLORED_2D_ANIMATING_QUAD:
            currentMode = IsColored2DAnimatingQuad;
            break;

        case ID_MENU_COLORED_2D_ANIMATING_TRIANGLE_AND_QUAD:
            currentMode = IsColored2DAnimatingTriangleAndQuad;
            break;

            // ---------------------- Pyramid ---------------------
        case ID_MENU_WHITE_PYRAMID:
            currentMode = IsWhitePyramid;
            break;

        case ID_MENU_COLORED_PYRAMID:
            currentMode = IsColoredPyramid;
            break;

            // ---------------------- Cube ---------------------
        case ID_MENU_WHITE_CUBE:
            currentMode = IsWhiteCube;
            break;

        case ID_MENU_COLORED_CUBE:
            currentMode = IsColoredCube;
            break;

            // --------------- Pyramid and Cube ----------------
        case ID_MENU_WHITE_PYRAMID_AND_CUBE:
            currentMode = IsWhitePyramidAndCube;
            break;

        case ID_MENU_COLORED_PYRAMID_AND_CUBE:
            currentMode = IsColoredPyramidAndCube;
            break;

        case ID_MENU_SPHERE:
            currentMode = IsSphere;
            break;

            // Textures
        case ID_MENU_SMILEY_TEXTURE:
            currentMode = IsSmileyTexture;
            break;

        case ID_MENU_PYRAMID_TEXTURE:
            currentMode = IsPyramidTexture;
            break;

        case ID_MENU_CUBE_TEXTURE:
            currentMode = IsCubeTexture;
            break;

        case ID_MENU_CHECKERBOARD_TEXTURE:
            currentMode = IsCheckerboradTexture;
            break;

        case ID_MENU_PYRAMID_AND_CUBE_TEXTURE:
            currentMode = IsPyraamidAndCubeTexture;
            break;

        // Lights
        case ID_MENU_DIFFUSE_LIGHT_CUBE:
            currentMode = IsDiffusedLightCube;
            break;

        case ID_MENU_DIFFUSE_LIGHT_PYRAMID:
            currentMode = IsDiffusedLightPyramid;
            break;

        case ID_MENU_DIFFUSE_LIGHT_SPHERE:
            currentMode = IsDiffusedLightSphere;
            break;

        case ID_MENU_PER_VERTEX_LIGHT:
            currentMode = IsPerVertexLight;
            break;

        case ID_MENU_PER_FRAGMENT_LIGHT:
            currentMode = IsPerFragmentLight;
            break;

        case ID_MENU_THREE_LIGHTS:
            currentMode = IsThreeLights;
            break;

        case ID_MENU_EXIT:
            DestroyWindow(hwnd);
            break;
        }
        break;

    case WM_SIZE:
        resize(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_ERASEBKGND:
        return (0);

    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        {
        case VK_ESCAPE:
            DestroyWindow(hwnd);
            break;
        }
        break;

    case WM_CHAR:
        switch (LOWORD(wParam))
        {
        case 'F':
        case 'f':
            if (gbFullScreen == FALSE)
            {
                ToggleFullscreen();
                gbFullScreen = TRUE;
            }
            else
            {
                ToggleFullscreen();
                gbFullScreen = FALSE;
            }
            break;

        case 'L':
        case 'l':
            if (IsLightEnabled == FALSE)
            {
                IsLightEnabled = TRUE;
            }
            else
            {
                IsLightEnabled = FALSE;
            }
            break;
        
        case 'm':
        case 'M':
            IsMenuVisible = !IsMenuVisible;
            SetMenu(hwnd, IsMenuVisible ? hMenuBar : NULL);
            DrawMenuBar(hwnd);
            break;
        } 
        break;

    case WM_DESTROY:
        if (gpfile)
        {
            fprintf(gpfile, "Program Ended Successfully\n");
            fclose(gpfile);
            gpfile = NULL;
        }
        PostQuitMessage(0);
        break;

    default:
        break;
    }
    return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
    // Local Variable Declarations
    MONITORINFO mi = {sizeof(MONITORINFO)};

    // Code
    if (gbFullScreen == FALSE)
    {
        dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
        if (dwStyle & WS_OVERLAPPEDWINDOW)
        {
            if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
            {
                SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
        ShowCursor(TRUE);
    }
    else
    {
        SetWindowPlacement(ghwnd, &wpPrev);
        SetWindowLong(ghwnd, GWL_STYLE,
                      dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
        ShowCursor(TRUE);
    }
}

int initialize(void)
{
    // Function Declarations
    void resize(int, int);
    void printGLInfo(void);
    void uninitialize(void);
    BOOL loadTexture(GLuint *, const char *);
    void loadProceduralTexture(void);
    BOOL LoadCubeMap(GLuint * texture, const char *filePaths[6]);

    PIXELFORMATDESCRIPTOR pfd;
    int iPixelFormatIndex = 0;
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

    // Get Device Context
    ghdc = GetDC(ghwnd);
    if (ghdc == NULL)
    {
        fprintf(gpfile, "GetDC failed\n");
        return (-1);
    }

    // Code
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = (PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER);
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cRedBits = 8;
    pfd.cBlueBits = 8;
    pfd.cGreenBits = 8;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32;
    iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

    if (iPixelFormatIndex == 0)
    {
        fprintf(gpfile, "ChoosePixelFormat fuction failed\n");
        return (-2);
    }

    if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
    {
        fprintf(gpfile, "SetPixelFormat function failed\n");
        return (-3);
    }

    // Tell WGL library to give OpenGL compatible context from this device context
    ghrc = wglCreateContext(ghdc);
    if (ghrc == NULL)
    {
        fprintf(gpfile, "wglContext failed\n");
        return (-4);
    }

    // Make Rendering Context to Current
    if (wglMakeCurrent(ghdc, ghrc) == FALSE)
    {
        fprintf(gpfile, "wgl MakeCurrent failed\n");
        return (-5);
    }

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        fprintf(gpfile, "failed to initialize GLEW \n");
        return (-6);
    }

    // PrintGLInfo
    printGLInfo();

    // Vertex Shaders
    const GLchar *vertexShaderSourceCode =

        "   #version 460 core                                       \n"
        "                                                           \n"
        "   in vec4 aPosition;                                      \n"
        "   in vec4 aColor;                                         \n"
        "   out vec4 oColor;                                        \n"
        "   in vec2 aTexCoord;                                      \n"
        "   out vec2 oTexCoord;                                     \n"
        "   uniform mat4 uMVPMatrix;                                \n"
        "   void main(void)                                         \n"
        "   {                                                       \n"
        "       gl_Position=uMVPMatrix * aPosition;                 \n"
        "       oColor = aColor;                                    \n"
        "       oTexCoord = aTexCoord;                              \n"
        "   }                                                       \n";

    GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);
    glCompileShader(vertexShaderObject);

    GLint status = NULL;
    GLint infoLoglength = NULL;
    char *szInfoLog = NULL;

    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(vertexShaderObject, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "Vertex shader complilation : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    // Fragment Shader
    const GLchar *fragmentShaderSourceCode =

        "   #version 460 core                                       \n"
        "   in vec4 oColor;                                         \n"
        "   out vec4 FragColor;                                     \n"
        "   void main(void)                                         \n"
        "   {                                                       \n"
        "       FragColor = oColor;                                 \n"
        "   }                                                       \n";

    GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);
    glCompileShader(fragmentShaderObject);

    status = 0;
    infoLoglength = 0;
    szInfoLog = 0;

    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(fragmentShaderObject, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "fragment shader compilation : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    // Shader Program
    shaderProgramObject = glCreateProgram();
    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);
    glBindAttribLocation(shaderProgramObject, SSG_ATTRIBUTE_POSITION, "aPosition");
    glBindAttribLocation(shaderProgramObject, SSG_ATTRIBUTE_COLOR, "aColor");
    glLinkProgram(shaderProgramObject);

    status = 0;
    infoLoglength = 0;
    szInfoLog = 0;

    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetProgramInfoLog(shaderProgramObject, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "shader program linking error log : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    // ========== Shaders For Texture ================
    // Vertex Shaders
    const GLchar *vertexShaderSourceCode_texture =

        "   #version 460 core                                       \n"
        "                                                           \n"
        "   in vec4 aPosition;                                      \n"
        "   in vec2 aTexCoord;                                      \n"
        "   out vec2 oTexCoord;                                     \n"
        "   uniform mat4 uMVPMatrix;                                \n"
        "   void main(void)                                         \n"
        "   {                                                       \n"
        "       gl_Position=uMVPMatrix * aPosition;                 \n"
        "       oTexCoord = aTexCoord;                              \n"
        "   }                                                       \n";

    GLuint vertexShaderObject_texture = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObject_texture, 1, (const GLchar **)&vertexShaderSourceCode_texture, NULL);
    glCompileShader(vertexShaderObject_texture);

    status = NULL;
    infoLoglength = NULL;
    szInfoLog = NULL;

    glGetShaderiv(vertexShaderObject_texture, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject_texture, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(vertexShaderObject_texture, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "Vertex shader complilation for texture : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    // Fragment Shader
    const GLchar *fragmentShaderSourceCode_texture =

        "   #version 460 core                                       \n"
        "   in vec2 oTexCoord;                                      \n"
        "   uniform sampler2D uTextureSampler;                      \n"
        "   out vec4 FragColor;                                     \n"
        "   void main(void)                                         \n"
        "   {                                                       \n"
        "       FragColor = texture(uTextureSampler, oTexCoord);    \n"
        "   }                                                       \n";

    GLuint fragmentShaderObject_texture = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject_texture, 1, (const GLchar **)&fragmentShaderSourceCode_texture, NULL);
    glCompileShader(fragmentShaderObject_texture);

    status = 0;
    infoLoglength = 0;
    szInfoLog = 0;

    glGetShaderiv(fragmentShaderObject_texture, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject_texture, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(fragmentShaderObject_texture, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "Fragment Shader Compilation for Texture: %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    // Shader Program
    shaderProgramObject_texturing = glCreateProgram();
    glAttachShader(shaderProgramObject_texturing, vertexShaderObject_texture);
    glAttachShader(shaderProgramObject_texturing, fragmentShaderObject_texture);
    glBindAttribLocation(shaderProgramObject_texturing, SSG_ATTRIBUTE_POSITION, "aPosition");
    glBindAttribLocation(shaderProgramObject_texturing, SSG_ATTRIBUTE_TEXCOORD, "aTexCoord");
    glLinkProgram(shaderProgramObject_texturing);

    status = 0;
    infoLoglength = 0;
    szInfoLog = 0;

    glGetProgramiv(shaderProgramObject_texturing, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject_texturing, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetProgramInfoLog(shaderProgramObject_texturing, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "Shader Program Linking Error log for Texture: %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    // Get Shader Uniform Locations
    mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix");
    textureSamplerUniform = glGetUniformLocation(shaderProgramObject_texturing, "uTextureSampler");

    //============================= DIFFUSED LIGHT SHADERS ====================================
    // Vertex Shader
    const GLchar *vertexShaderSourceCode_diffusedLight =
        "#version 460 core\n"
        "\n"
        "in vec4 aPosition;\n"
        "in vec3 aNormal;\n" /*"in vec4 aColor;\n" \
                             "out vec4 oColor;\n" \ */
        "uniform mat4 uModelViewMatrix;\n"
        "uniform mat4 uProjectionMatrix;\n"
        "uniform vec3 uLd;\n"
        "uniform vec3 uKd;\n"
        "uniform vec4 uLightPosition;\n"
        "uniform int uKeyPressed;\n"
        "out vec3 oDiffusedLight;\n"
        "void main(void)\n"
        "{\n"
        "if(uKeyPressed == 1)\n"
        "{\n"
        "vec4 iPosition = uModelViewMatrix * aPosition;\n"
        "mat3 normalMatrix = mat3(transpose(inverse(uModelViewMatrix)));\n"
        "vec3 n = normalize(normalMatrix * aNormal);\n"
        "vec3 s = normalize(vec3(uLightPosition - iPosition));\n"
        "oDiffusedLight = uLd * uKd * dot(s,n);\n"
        "}\n"
        "else\n"
        "{\n"
        "oDiffusedLight = vec3(0.0f, 0.0f, 0.0f);\n"
        "}\n" \
        "gl_Position = uProjectionMatrix * uModelViewMatrix * aPosition;\n"
        "}\n";

    GLuint vertexShaderObject_diffusedLight = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObject_diffusedLight, 1, (const GLchar **)&vertexShaderSourceCode_diffusedLight, NULL);
    glCompileShader(vertexShaderObject_diffusedLight);

    status = NULL;
    infoLoglength = NULL;
    szInfoLog = NULL;

    glGetShaderiv(vertexShaderObject_diffusedLight, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject_diffusedLight, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {

            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(vertexShaderObject_diffusedLight, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "Vertex shader complilation of Diffused Light : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    /* Fragment Shader */
    const GLchar *fragmentShaderSourceCode_diffusedLight =
        "#version 460 core\n"
        "\n"
        "in vec3 oDiffusedLight;\n"
        "uniform int uKeyPressed;\n"
        "out vec4 FragColor;\n"
        "void main(void)\n"
        "{\n"
        "if(uKeyPressed == 1)\n"
        "{\n"
        "FragColor = vec4(oDiffusedLight, 1.0);\n"
        "}\n"
        "else\n"
        "{\n"
        "FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "}\n"
        "}\n";

    GLuint fragmentShaderObject_diffusedLight = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject_diffusedLight, 1, (const GLchar **)&fragmentShaderSourceCode_diffusedLight, NULL);
    glCompileShader(fragmentShaderObject_diffusedLight);

    status = 0;
    infoLoglength = 0;
    szInfoLog = 0;

    glGetShaderiv(fragmentShaderObject_diffusedLight, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject_diffusedLight, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(fragmentShaderObject_diffusedLight, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "fragment shader compilation of Diffused Light : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    /* Shader Program */
    shaderProgramObject_diffusedLight = glCreateProgram();
    glAttachShader(shaderProgramObject_diffusedLight, vertexShaderObject_diffusedLight);
    glAttachShader(shaderProgramObject_diffusedLight, fragmentShaderObject_diffusedLight);
    glBindAttribLocation(shaderProgramObject_diffusedLight, SSG_ATTRIBUTE_POSITION, "aPosition");
    glBindAttribLocation(shaderProgramObject_diffusedLight, SSG_ATTRIBUTE_COLOR, "aColor");
    glBindAttribLocation(shaderProgramObject_diffusedLight, SSG_ATTRIBUTE_NORMAL, "aNormal");
    glLinkProgram(shaderProgramObject_diffusedLight);

    status = 0;
    infoLoglength = 0;
    szInfoLog = 0;

    glGetProgramiv(shaderProgramObject_diffusedLight, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject_diffusedLight, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetProgramInfoLog(shaderProgramObject_diffusedLight, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "shader program linking error log : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    /* Get Shader Uniform Locations */
    modelViewMatrixUniform = glGetUniformLocation(shaderProgramObject_diffusedLight, "uModelViewMatrix");
    projectionMatrixUniform_DifL = glGetUniformLocation(shaderProgramObject_diffusedLight, "uProjectionMatrix");
    lightDiffusedUniform = glGetUniformLocation(shaderProgramObject_diffusedLight, "uLd");
    materialDiffusedUniform = glGetUniformLocation(shaderProgramObject_diffusedLight, "uKd");
    lightPositionUniform = glGetUniformLocation(shaderProgramObject_diffusedLight, "uLightPosition");
    keyPressedUniform_Difl = glGetUniformLocation(shaderProgramObject_diffusedLight, "uKeyPressed");

    // =========================== Per Vertex Light Shaders ===========================
    /* Vertex Shaders */
    const GLchar *vertexShaderSourceCode_PVL =
        "#version 460 core\n"
        "\n"
        "in vec4 aPosition;\n"
        "in vec3 aNormal;\n"
        "in vec4 aColor;\n"
        "out vec4 oColor;\n"
        "\n"
        "uniform mat4 uViewMatrix;\n"
        "uniform mat4 uModelMatrix;\n"
        "uniform mat4 uProjectionMatrix;\n"
        "\n"
        "uniform vec3 uLight_Ambient;\n"
        "uniform vec3 uLight_Specular;\n"
        "uniform vec3 uLight_Diffused;\n"
        "uniform vec4 uLight_Position;\n"
        "\n"
        "uniform vec3 uMaterial_Diffused;\n"
        "uniform vec3 uMaterial_Ambient;\n"
        "uniform vec3 uMaterial_Specular;\n"
        "uniform float uMaterial_Shininess;\n"
        "\n"
        "uniform int uKeyPressed;\n"
        "out vec3 oPhong_ADS_Light;\n"
        "void main(void)\n"
        "{\n"
        "if(uKeyPressed == 1)\n"
        "{\n"
        "vec4 eye_Coordinates = uViewMatrix * uModelMatrix * aPosition;\n"
        "vec3 transformed_Normals = normalize(mat3(uViewMatrix * uModelMatrix) * aNormal);\n"
        "vec3 light_Direction = normalize(vec3(uLight_Position - eye_Coordinates));\n"
        "vec3 reflection_Vector = reflect(-light_Direction, transformed_Normals);\n"
        "vec3 viewer_Vector = normalize(-eye_Coordinates.xyz);\n"
        "vec3 lightAmbient = uLight_Ambient * uMaterial_Ambient;\n"
        "vec3 lightDiffused = uLight_Diffused * uMaterial_Diffused * max(dot(light_Direction, transformed_Normals), 0.0);\n"
        "vec3 lightSpecular = uLight_Specular * uMaterial_Specular * pow(max(dot(reflection_Vector, viewer_Vector), 0.0f), uMaterial_Shininess );\n"
        "oPhong_ADS_Light = lightAmbient + lightDiffused + lightSpecular;\n"
        "}\n"
        "else\n"
        "{\n"
        "oPhong_ADS_Light = vec3(0.0f, 0.0f, 0.0f);\n"
        "}\n"
        "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n"
        "}\n";

    GLuint vertexShaderObject_PVL = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObject_PVL, 1, (const GLchar **)&vertexShaderSourceCode_PVL, NULL);
    glCompileShader(vertexShaderObject_PVL);

    status = NULL;
    infoLoglength = NULL;
    szInfoLog = NULL;

    glGetShaderiv(vertexShaderObject_PVL, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject_PVL, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {

            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(vertexShaderObject_PVL, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "Vertex shader complilation of Per Vertex Light : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    /* Fragment Shader */
    const GLchar *fragmentShaderSourceCode_PVL =
        "#version 460 core\n"
        "\n"
        "in vec3 oPhong_ADS_Light;\n"
        "uniform int uKeyPressed;\n"
        "out vec4 FragColor;\n"
        "void main(void)\n"
        "{\n"
        "if(uKeyPressed == 1)\n"
        "{\n"
        "FragColor = vec4(oPhong_ADS_Light, 1.0);\n"
        "}\n"
        "else\n"
        "{\n"
        "FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "}\n"
        "}\n";

    GLuint fragmentShaderObject_PVL = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject_PVL, 1, (const GLchar **)&fragmentShaderSourceCode_PVL, NULL);
    glCompileShader(fragmentShaderObject_PVL);

    status = 0;
    infoLoglength = 0;
    szInfoLog = 0;

    glGetShaderiv(fragmentShaderObject_PVL, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject_PVL, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(fragmentShaderObject_PVL, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "fragment shader compilation of Per Vertex Light : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    /* Shader Program */
    shaderProgramObject_perVertexLight = glCreateProgram();
    glAttachShader(shaderProgramObject_perVertexLight, vertexShaderObject_PVL);
    glAttachShader(shaderProgramObject_perVertexLight, fragmentShaderObject_PVL);
    glBindAttribLocation(shaderProgramObject_perVertexLight, SSG_ATTRIBUTE_POSITION, "aPosition");
    glBindAttribLocation(shaderProgramObject_perVertexLight, SSG_ATTRIBUTE_COLOR, "aColor");
    glBindAttribLocation(shaderProgramObject_perVertexLight, SSG_ATTRIBUTE_NORMAL, "aNormal");
    glLinkProgram(shaderProgramObject_perVertexLight);

    status = 0;
    infoLoglength = 0;
    szInfoLog = 0;

    glGetProgramiv(shaderProgramObject_perVertexLight, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject_perVertexLight, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetProgramInfoLog(shaderProgramObject_perVertexLight, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "shader program linking error log of Per Vertex Light : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    /* Get Shader Uniform Locations */
    modelMatrixUniform = glGetUniformLocation(shaderProgramObject_perVertexLight, "uModelMatrix");
    viewMatrixUniform = glGetUniformLocation(shaderProgramObject_perVertexLight, "uViewMatrix");
    projectionMatrixUniform = glGetUniformLocation(shaderProgramObject_perVertexLight, "uProjectionMatrix");

    light_Diffused_Uniform = glGetUniformLocation(shaderProgramObject_perVertexLight, "uLight_Diffused");
    light_Ambient_Uniform = glGetUniformLocation(shaderProgramObject_perVertexLight, "uLight_Ambient");
    light_Specular_Uniform = glGetUniformLocation(shaderProgramObject_perVertexLight, "uLight_Specular");
    light_Position_Uniform = glGetUniformLocation(shaderProgramObject_perVertexLight, "uLight_Position");

    material_Diffused_Uniform = glGetUniformLocation(shaderProgramObject_perVertexLight, "uMaterial_Diffused");
    material_Ambient_Uniform = glGetUniformLocation(shaderProgramObject_perVertexLight, "uMaterial_Ambient");
    material_Specular_Uniform = glGetUniformLocation(shaderProgramObject_perVertexLight, "uMaterial_Specular");
    material_Shininess_Uniform = glGetUniformLocation(shaderProgramObject_perVertexLight, "uMaterial_Shininess");

    keyPressedUniform = glGetUniformLocation(shaderProgramObject_perVertexLight, "uKeyPressed");

    //============================ Per Fragment Light Shaders ===========================/* Vertex Shaders */
    // Vertex Shader
    const GLchar *vertexShaderSourceCode_PFL =

        "#version 460 core\n"
        "\n"
        "in vec4 aPosition;\n"
        "in vec3 aNormal;\n"
        "in vec4 aColor;\n"
        "out vec4 oColor;\n"
        "\n"
        "uniform mat4 uViewMatrix;\n"
        "uniform mat4 uModelMatrix;\n"
        "uniform mat4 uProjectionMatrix;\n"
        "uniform vec4 uLight_Position;\n"
        "\n"
        "uniform int uKeyPressed;\n"
        "out vec3 oTransformed_Normals;\n"
        "out vec3 oLight_Direction;\n"
        "out vec3 oViewer_Vector;\n"
        "void main(void)\n"
        "{\n"
        "if(uKeyPressed == 1)\n"
        "{\n"
        "vec4 eye_Coordinates = uViewMatrix * uModelMatrix * aPosition;\n"
        "oTransformed_Normals = mat3(uViewMatrix * uModelMatrix) * aNormal;\n"
        "oLight_Direction = vec3(uLight_Position - eye_Coordinates);\n"
        "oViewer_Vector = -eye_Coordinates.xyz;\n"
        "}\n"
        "else\n"
        "{\n"
        "oTransformed_Normals = vec3(0.0, 0.0, 0.0);\n"
        "oLight_Direction = vec3(0.0, 0.0, 0.0);\n"
        "oViewer_Vector = vec3(0.0, 0.0, 0.0);\n"
        "}\n"
        "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n"
        "}\n";

    GLuint vertexShaderObject_PFL = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObject_PFL, 1, (const GLchar **)&vertexShaderSourceCode_PFL, NULL);
    glCompileShader(vertexShaderObject_PFL);

    status = NULL;
    infoLoglength = NULL;
    szInfoLog = NULL;

    glGetShaderiv(vertexShaderObject_PFL, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject_PFL, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {

            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(vertexShaderObject_PFL, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "Vertex shader complilation of Per Fragment Light : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    /* Fragment Shader */
    const GLchar *fragmentShaderSourceCode_PFL =

        "#version 460 core\n"
        "\n"
        "in vec3 oTransformed_Normals;\n"
        "in vec3 oLight_Direction;\n"
        "in vec3 oViewer_Vector;\n"
        "\n"
        "uniform vec3 uLight_Ambient;\n"
        "uniform vec3 uLight_Specular;\n"
        "uniform vec3 uLight_Diffused;\n"
        "\n"
        "uniform vec3 uMaterial_Diffused;\n"
        "uniform vec3 uMaterial_Ambient;\n"
        "uniform vec3 uMaterial_Specular;\n"
        "uniform float uMaterial_Shininess;\n"
        "\n"
        "uniform int uKeyPressed;\n"
        "out vec4 FragColor;\n"
        "void main(void)\n"
        "{\n"
        "vec3 phong_ADS_Light;\n"
        "if(uKeyPressed == 1)\n"
        "{\n"
        "vec3 normalized_Transformed_Normals = normalize(oTransformed_Normals);\n"
        "vec3 normalized_Light_Direction = normalize(oLight_Direction);\n"
        "vec3 normalized_Viewer_Vector = normalize(oViewer_Vector);\n"
        "\n"
        "vec3 lightAmbient = uLight_Ambient * uMaterial_Ambient;\n"
        "vec3 lightDiffused = uLight_Diffused * uMaterial_Diffused * max(dot(normalized_Light_Direction, normalized_Transformed_Normals), 0.0);\n"
        "vec3 reflection_Vector = reflect(-normalized_Light_Direction, normalized_Transformed_Normals);\n"
        "vec3 lightSpecular = uLight_Specular * uMaterial_Specular * pow(max(dot(reflection_Vector, normalized_Viewer_Vector), 0.0f), uMaterial_Shininess );\n"
        "phong_ADS_Light = lightAmbient + lightDiffused + lightSpecular;\n"
        "}\n"
        "else\n"
        "{\n"
        "phong_ADS_Light = vec3(1.0, 1.0, 1.0);\n"
        "}\n"
        "FragColor = vec4(phong_ADS_Light, 1.0);\n"
        "}\n";

    GLuint fragmentShaderObject_PFL = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject_PFL, 1, (const GLchar **)&fragmentShaderSourceCode_PFL, NULL);
    glCompileShader(fragmentShaderObject_PFL);

    status = 0;
    infoLoglength = 0;
    szInfoLog = 0;

    glGetShaderiv(fragmentShaderObject_PFL, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject_PFL, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(fragmentShaderObject_PFL, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "fragment shader compilation of Per Fragment Light : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    /* Shader Program */
    shaderProgramObject_perFragmentLight = glCreateProgram();
    glAttachShader(shaderProgramObject_perFragmentLight, vertexShaderObject_PFL);
    glAttachShader(shaderProgramObject_perFragmentLight, fragmentShaderObject_PFL);
    glBindAttribLocation(shaderProgramObject_perFragmentLight, SSG_ATTRIBUTE_POSITION, "aPosition");
    glBindAttribLocation(shaderProgramObject_perFragmentLight, SSG_ATTRIBUTE_COLOR, "aColor");
    glBindAttribLocation(shaderProgramObject_perFragmentLight, SSG_ATTRIBUTE_NORMAL, "aNormal");
    glLinkProgram(shaderProgramObject_perFragmentLight);

    status = 0;
    infoLoglength = 0;
    szInfoLog = 0;

    glGetProgramiv(shaderProgramObject_perFragmentLight, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject_perFragmentLight, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetProgramInfoLog(shaderProgramObject_perFragmentLight, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "shader program linking error log of Per Fragment Light : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    /* Get Shader Uniform Locations */
    modelMatrixUniform = glGetUniformLocation(shaderProgramObject_perFragmentLight, "uModelMatrix");
    viewMatrixUniform = glGetUniformLocation(shaderProgramObject_perFragmentLight, "uViewMatrix");
    projectionMatrixUniform = glGetUniformLocation(shaderProgramObject_perFragmentLight, "uProjectionMatrix");

    light_Diffused_Uniform = glGetUniformLocation(shaderProgramObject_perFragmentLight, "uLight_Diffused");
    light_Ambient_Uniform = glGetUniformLocation(shaderProgramObject_perFragmentLight, "uLight_Ambient");
    light_Specular_Uniform = glGetUniformLocation(shaderProgramObject_perFragmentLight, "uLight_Specular");
    light_Position_Uniform = glGetUniformLocation(shaderProgramObject_perFragmentLight, "uLight_Position");

    material_Diffused_Uniform = glGetUniformLocation(shaderProgramObject_perFragmentLight, "uMaterial_Diffused");
    material_Ambient_Uniform = glGetUniformLocation(shaderProgramObject_perFragmentLight, "uMaterial_Ambient");
    material_Specular_Uniform = glGetUniformLocation(shaderProgramObject_perFragmentLight, "uMaterial_Specular");
    material_Shininess_Uniform = glGetUniformLocation(shaderProgramObject_perFragmentLight, "uMaterial_Shininess");

    keyPressedUniform = glGetUniformLocation(shaderProgramObject_perFragmentLight, "uKeyPressed");

    //=============================== CubeMap Shaders ======================
    // Vertex Shaders
    const GLchar *vertexShaderSourceCode_cubemap =

    "   #version 460 core                               \n"
    "                                                   \n"
    "   in vec4 aPosition;                              \n"
    "   in vec3 aTexCoord;                              \n"
    "   out vec3 oTexCoord;                             \n"
    "   uniform mat4 uMVPMatrix;                        \n"
    "   void main(void)                                 \n"
    "   {                                               \n"
    "       gl_Position = uMVPMatrix * aPosition;       \n"
    "       oTexCoord = aPosition.xyz;                  \n"
    "   }                                               \n";

    GLuint vertexShaderObject_cubemap = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObject_cubemap, 1, (const GLchar **)&vertexShaderSourceCode_cubemap, NULL);
    glCompileShader(vertexShaderObject_cubemap);

    status = NULL;
    infoLoglength = NULL;
    szInfoLog = NULL;

    glGetShaderiv(vertexShaderObject_cubemap, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject_cubemap, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {

            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(vertexShaderObject_cubemap, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "Vertex Shader Compilation for Cubemap : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    // Fragment Shader
    const GLchar *fragmentShaderSourceCode_cubemap =
        "   #version 460 core                               \n"
        "                                                   \n"
        "   in vec3 oTexCoord;                              \n"
        "   uniform samplerCube uCubeMap;                   \n"
        "   out vec4 FragColor;                             \n"
        "   void main(void)                                 \n"
        "   {                                               \n"
        "   FragColor = texture(uCubeMap, oTexCoord);       \n"
        "   }                                               \n";

    GLuint fragmentShaderObject_cubemap = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject_cubemap, 1, (const GLchar **)&fragmentShaderSourceCode_cubemap, NULL);
    glCompileShader(fragmentShaderObject_cubemap);

    status = 0;
    infoLoglength = 0;
    szInfoLog = 0;

    glGetShaderiv(fragmentShaderObject_cubemap, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject_cubemap, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(fragmentShaderObject_cubemap, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "Fragment Shader Compilation for CubeMap : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    // Shader Program
    shaderProgramObject_cubeMap = glCreateProgram();
    glAttachShader(shaderProgramObject_cubeMap, vertexShaderObject_cubemap);
    glAttachShader(shaderProgramObject_cubeMap, fragmentShaderObject_cubemap);
    glBindAttribLocation(shaderProgramObject_cubeMap, SSG_ATTRIBUTE_POSITION, "aPosition");
    glBindAttribLocation(shaderProgramObject_cubeMap, SSG_ATTRIBUTE_TEXCOORD, "aTexCoord");
    glLinkProgram(shaderProgramObject_cubeMap);

    status = 0;
    infoLoglength = 0;
    szInfoLog = 0;

    glGetProgramiv(shaderProgramObject_cubeMap, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject_cubeMap, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetProgramInfoLog(shaderProgramObject_cubeMap, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "Shader Program linking error log for CubeMap : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    /* Get Shader Uniform Locations */
    mvpMatrixUniformCubeMap = glGetUniformLocation(shaderProgramObject_cubeMap, "uMVPMatrix");
    textureSamplerUniformCubeMap = glGetUniformLocation(shaderProgramObject_cubeMap, "uCubeMap");

    // =========================== 3 Lights on Sphere Shaders ===================================
    // Per Fragment Light
    // Vertex Shaders
    const GLchar *vertexShaderSourceCode_PF =

        "#version 460 core\n"
        "\n"
        "in vec4 aPosition;\n"
        "in vec3 aNormal;\n"
        "\n"
        "uniform mat4 uViewMatrix;\n"
        "uniform mat4 uModelMatrix;\n"
        "uniform mat4 uProjectionMatrix;\n"
        "uniform vec4 uLight_Position[3];\n"
        "\n"
        "uniform int uKeyPressed;\n"
        "out vec3 oTransformed_Normals;\n"
        "out vec3 oLight_Direction[3];\n"
        "out vec3 oViewer_Vector;\n"
        "void main(void)\n"
        "{\n"
        "if(uKeyPressed == 1)\n"
        "{\n"
        "vec4 eye_Coordinates = uViewMatrix * uModelMatrix * aPosition;\n"
        "oTransformed_Normals = mat3(uViewMatrix * uModelMatrix) * aNormal;\n"
        "oLight_Direction[0] = vec3(uLight_Position[0] - eye_Coordinates);\n"
        "oLight_Direction[1] = vec3(uLight_Position[1] - eye_Coordinates);\n"
        "oLight_Direction[2] = vec3(uLight_Position[2] - eye_Coordinates);\n"
        "oViewer_Vector = -eye_Coordinates.xyz;\n"
        "}\n"
        "else\n"
        "{\n"
        "oTransformed_Normals = vec3(0.0, 0.0, 0.0);\n"
        "oLight_Direction[0] = vec3(0.0, 0.0, 0.0);\n"
        "oLight_Direction[1] = vec3(0.0, 0.0, 0.0);\n"
        "oLight_Direction[2] = vec3(0.0, 0.0, 0.0);\n"
        "oViewer_Vector = vec3(0.0, 0.0, 0.0);\n"
        "}\n"
        "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n"
        "}\n";

    GLuint vertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObject_PF, 1, (const GLchar **)&vertexShaderSourceCode_PF, NULL);
    glCompileShader(vertexShaderObject_PF);

    status = NULL;
    infoLoglength = NULL;
    szInfoLog = NULL;

    glGetShaderiv(vertexShaderObject_PF, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject_PF, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {

            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(vertexShaderObject_PF, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "Vertex shader complilation(per fragment) : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    /* Fragment Shader */
    const GLchar *fragmentShaderSourceCode_PF =

        "#version 460 core\n"
        "\n"
        "in vec3 oTransformed_Normals;\n"
        "in vec3 oLight_Direction[3];\n"
        "in vec3 oViewer_Vector;\n"
        "\n"
        "uniform vec3 uLight_Ambient[3];\n"
        "uniform vec3 uLight_Specular[3];\n"
        "uniform vec3 uLight_Diffused[3];\n"
        "\n"
        "uniform vec3 uMaterial_Diffused;\n"
        "uniform vec3 uMaterial_Ambient;\n"
        "uniform vec3 uMaterial_Specular;\n"
        "uniform float uMaterial_Shininess;\n"
        "\n"
        "uniform int uKeyPressed;\n"
        "out vec4 FragColor;\n"
        "void main(void)\n"
        "{\n"
        "vec3 phong_ADS_Light;\n"
        "if(uKeyPressed == 1)\n"
        "{\n"
        "vec3 normalized_Light_Direction[3];\n"
        "vec3 lightDiffused[3];\n"
        "vec3 lightAmbient[3];\n"
        "vec3 reflection_Vector[3];\n"
        "vec3 lightSpecular[3];\n"
        "vec3 normalized_Transformed_Normals = normalize(oTransformed_Normals);\n"
        "normalized_Light_Direction[0] = normalize(oLight_Direction[0]);\n"
        "normalized_Light_Direction[1] = normalize(oLight_Direction[1]);\n"
        "normalized_Light_Direction[2] = normalize(oLight_Direction[2]);\n"
        "vec3 normalized_Viewer_Vector = normalize(oViewer_Vector);\n"
        "\n"
        "lightAmbient[0] = uLight_Ambient[0] * uMaterial_Ambient;\n"
        "lightAmbient[1] = uLight_Ambient[1] * uMaterial_Ambient;\n"
        "lightAmbient[2] = uLight_Ambient[2] * uMaterial_Ambient;\n"
        "lightDiffused[0] = uLight_Diffused[0] * uMaterial_Diffused * max(dot(normalized_Light_Direction[0], normalized_Transformed_Normals), 0.0);\n"
        "lightDiffused[1] = uLight_Diffused[1] * uMaterial_Diffused * max(dot(normalized_Light_Direction[1], normalized_Transformed_Normals), 0.0);\n"
        "lightDiffused[2] = uLight_Diffused[2] * uMaterial_Diffused * max(dot(normalized_Light_Direction[2], normalized_Transformed_Normals), 0.0);\n"
        "reflection_Vector[0] = reflect(-normalized_Light_Direction[0], normalized_Transformed_Normals);\n"
        "reflection_Vector[1] = reflect(-normalized_Light_Direction[1], normalized_Transformed_Normals);\n"
        "reflection_Vector[2] = reflect(-normalized_Light_Direction[2], normalized_Transformed_Normals);\n"
        "lightSpecular[0] = uLight_Specular[0] * uMaterial_Specular * pow(max(dot(reflection_Vector[0], normalized_Viewer_Vector), 0.0f), uMaterial_Shininess );\n"
        "lightSpecular[1] = uLight_Specular[1] * uMaterial_Specular * pow(max(dot(reflection_Vector[1], normalized_Viewer_Vector), 0.0f), uMaterial_Shininess );\n"
        "lightSpecular[2] = uLight_Specular[2] * uMaterial_Specular * pow(max(dot(reflection_Vector[2], normalized_Viewer_Vector), 0.0f), uMaterial_Shininess );\n"
        "phong_ADS_Light = lightAmbient[0] + lightDiffused[0] + lightSpecular[0];\n"
        "phong_ADS_Light = phong_ADS_Light + lightAmbient[1] + lightDiffused[1] + lightSpecular[1];\n"
        "phong_ADS_Light = phong_ADS_Light + lightAmbient[2] + lightDiffused[2] + lightSpecular[2];\n"
        "}\n"
        "else\n"
        "{\n"
        "phong_ADS_Light = vec3(1.0, 1.0, 1.0);\n"
        "}\n"
        "FragColor = vec4(phong_ADS_Light, 1.0);\n"
        "}\n";

    GLuint fragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject_PF, 1, (const GLchar **)&fragmentShaderSourceCode_PF, NULL);
    glCompileShader(fragmentShaderObject_PF);

    status = 0;
    infoLoglength = 0;
    szInfoLog = 0;

    glGetShaderiv(fragmentShaderObject_PF, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject_PF, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(fragmentShaderObject_PF, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "fragment shader compilation(per fragment) : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    // Shader Program for Per Fragment
    shaderProgramObject_threeLights = glCreateProgram();
    glAttachShader(shaderProgramObject_threeLights, vertexShaderObject_PF);
    glAttachShader(shaderProgramObject_threeLights, fragmentShaderObject_PF);
    glBindAttribLocation(shaderProgramObject_threeLights, SSG_ATTRIBUTE_POSITION, "aPosition");
    glBindAttribLocation(shaderProgramObject_threeLights, SSG_ATTRIBUTE_COLOR, "aColor");
    glBindAttribLocation(shaderProgramObject_threeLights, SSG_ATTRIBUTE_NORMAL, "aNormal");
    glLinkProgram(shaderProgramObject_threeLights);

    status = 0;
    infoLoglength = 0;
    szInfoLog = 0;

    glGetProgramiv(shaderProgramObject_threeLights, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject_threeLights, GL_INFO_LOG_LENGTH, &infoLoglength);
        if (infoLoglength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLoglength);
            if (szInfoLog != NULL)
            {
                glGetProgramInfoLog(shaderProgramObject_threeLights, infoLoglength, NULL, szInfoLog);
                fprintf_s(gpfile, "Shader Program linking error log for Three Lights : %s \n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    /* Get Shader Uniform Locations for Per Fragment */
    modelMatrixUniform_3L = glGetUniformLocation(shaderProgramObject_threeLights, "uModelMatrix");
    viewMatrixUniform_3L = glGetUniformLocation(shaderProgramObject_threeLights, "uViewMatrix");
    projectionMatrixUniform_3L = glGetUniformLocation(shaderProgramObject_threeLights, "uProjectionMatrix");

    light_Diffused_Uniform_3L[0] = glGetUniformLocation(shaderProgramObject_threeLights, "uLight_Diffused[0]");
    light_Ambient_Uniform_3L[0] = glGetUniformLocation(shaderProgramObject_threeLights, "uLight_Ambient[0]");
    light_Specular_Uniform_3L[0] = glGetUniformLocation(shaderProgramObject_threeLights, "uLight_Specular[0]");
    light_Position_Uniform_3L[0] = glGetUniformLocation(shaderProgramObject_threeLights, "uLight_Position[0]");

    light_Diffused_Uniform_3L[1] = glGetUniformLocation(shaderProgramObject_threeLights, "uLight_Diffused[1]");
    light_Ambient_Uniform_3L[1] = glGetUniformLocation(shaderProgramObject_threeLights, "uLight_Ambient[1]");
    light_Specular_Uniform_3L[1] = glGetUniformLocation(shaderProgramObject_threeLights, "uLight_Specular[1]");
    light_Position_Uniform_3L[1] = glGetUniformLocation(shaderProgramObject_threeLights, "uLight_Position[1]");

    light_Diffused_Uniform_3L[2] = glGetUniformLocation(shaderProgramObject_threeLights, "uLight_Diffused[2]");
    light_Ambient_Uniform_3L[2] = glGetUniformLocation(shaderProgramObject_threeLights, "uLight_Ambient[2]");
    light_Specular_Uniform_3L[2] = glGetUniformLocation(shaderProgramObject_threeLights, "uLight_Specular[2]");
    light_Position_Uniform_3L[2] = glGetUniformLocation(shaderProgramObject_threeLights, "uLight_Position[2]");

    material_Diffused_Uniform_3L = glGetUniformLocation(shaderProgramObject_threeLights, "uMaterial_Diffused");
    material_Ambient_Uniform_3L = glGetUniformLocation(shaderProgramObject_threeLights, "uMaterial_Ambient");
    material_Specular_Uniform_3L = glGetUniformLocation(shaderProgramObject_threeLights, "uMaterial_Specular");
    material_Shininess_Uniform_3L = glGetUniformLocation(shaderProgramObject_threeLights, "uMaterial_Shininess");
    keyPressedUniform = glGetUniformLocation(shaderProgramObject_threeLights, "uKeyPressed");

    const GLfloat triangleVertices[] =
        {
            0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f};

    const GLfloat orthoTriangleVertices[] =
        {
            0.0f, 50.0f, 0.0f,
            -50.0f, -50.0f, 0.0f,
            50.0f, -50.0f, 0.0f};

    const GLfloat triangleColor[] =
        {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f};

    const GLfloat whiteTriangle[] =
        {
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f};

    // Quad
    const GLfloat quadVertices[] =
        {
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f};

    const GLfloat quadTexCoords[] =
        {
            1, 1,
            0, 1,
            0, 0,
            1, 0};

    const GLfloat whiteQuad[] =
        {
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f};

    const GLfloat colorQuad[] =
        {
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f};

    //------------ Pyramid ------------
    const GLfloat pyramidVertices[] =
        {
            // front
            0.0f, 1.0f, 0.0f,   // front-top
            -1.0f, -1.0f, 1.0f, // front-left
            1.0f, -1.0f, 1.0f,  // front-right

            // right
            0.0f, 1.0f, 0.0f,   // right-top
            1.0f, -1.0f, 1.0f,  // right-left
            1.0f, -1.0f, -1.0f, // right-right

            // back
            0.0f, 1.0f, 0.0f,    // back-top
            1.0f, -1.0f, -1.0f,  // back-left
            -1.0f, -1.0f, -1.0f, // back-right

            // left
            0.0f, 1.0f, 0.0f,    // left-top
            -1.0f, -1.0f, -1.0f, // left-left
            -1.0f, -1.0f, 1.0f,  // left-right
        };

    const GLfloat pyramidColor[] =
        {
            // front
            1.0f, 0.0f, 0.0f, // front-top
            0.0f, 1.0f, 0.0f, // front-lef
            0.0f, 0.0f, 1.0f, // front-right

            // right
            1.0f, 0.0f, 0.0f, // right-top
            0.0f, 0.0f, 1.0f, // right-left
            0.0f, 1.0f, 0.0f, // right-right

            // back
            1.0f, 0.0f, 0.0f, // back-top
            0.0f, 1.0f, 0.0f, // back-left
            0.0f, 0.0f, 1.0f, // back-right

            // left
            1.0f, 0.0f, 0.0f, // left-top
            0.0f, 0.0f, 1.0f, // left-left
            0.0f, 1.0f, 0.0f, // left-right
        };

    const GLfloat pyramidTexCoords[] =
        {
            // front
            0.5, 1.0, // front-top
            0.0, 0.0, // front-left
            1.0, 0.0, // front-right

            // right
            0.5, 1.0, // right-top
            1.0, 0.0, // right-left
            0.0, 0.0, // right-right

            // back
            0.5, 1.0, // back-top
            0.0, 0.0, // back-left
            1.0, 0.0, // back-right

            // left
            0.5, 1.0, // left-top
            1.0, 0.0, // left-left
            0.0, 0.0, // left-right
        };

    const GLfloat pyramidNormals[] =
        {
            // front
            0.000000f, 0.447214f, 0.894427f, // front-top
            0.000000f, 0.447214f, 0.894427f, // front-left
            0.000000f, 0.447214f, 0.894427f, // front-right

            // right
            0.894427f, 0.447214f, 0.000000f, // right-top
            0.894427f, 0.447214f, 0.000000f, // right-left
            0.894427f, 0.447214f, 0.000000f, // right-right

            // back
            0.000000f, 0.447214f, -0.894427f, // back-top
            0.000000f, 0.447214f, -0.894427f, // back-left
            0.000000f, 0.447214f, -0.894427f, // back-right

            // left
            -0.894427f, 0.447214f, 0.000000f, // left-top
            -0.894427f, 0.447214f, 0.000000f, // left-left
            -0.894427f, 0.447214f, 0.000000f, // left-right
        };

    // ------------- Cube ------------
    const GLfloat cubeVertices[] =
        {
            // front
            1.0f, 1.0f, 1.0f,   // top-right of front
            -1.0f, 1.0f, 1.0f,  // top-left of front
            -1.0f, -1.0f, 1.0f, // bottom-left of front
            1.0f, -1.0f, 1.0f,  // bottom-right of front

            // right
            1.0f, 1.0f, -1.0f,  // top-right of right
            1.0f, 1.0f, 1.0f,   // top-left of right
            1.0f, -1.0f, 1.0f,  // bottom-left of right
            1.0f, -1.0f, -1.0f, // bottom-right of right

            // back
            1.0f, 1.0f, -1.0f,   // top-right of back
            -1.0f, 1.0f, -1.0f,  // top-left of back
            -1.0f, -1.0f, -1.0f, // bottom-left of back
            1.0f, -1.0f, -1.0f,  // bottom-right of back

            // left
            -1.0f, 1.0f, 1.0f,   // top-right of left
            -1.0f, 1.0f, -1.0f,  // top-left of left
            -1.0f, -1.0f, -1.0f, // bottom-left of left
            -1.0f, -1.0f, 1.0f,  // bottom-right of left

            // top
            1.0f, 1.0f, -1.0f,  // top-right of top
            -1.0f, 1.0f, -1.0f, // top-left of top
            -1.0f, 1.0f, 1.0f,  // bottom-left of top
            1.0f, 1.0f, 1.0f,   // bottom-right of top

            // bottom
            1.0f, -1.0f, 1.0f,   // top-right of bottom
            -1.0f, -1.0f, 1.0f,  // top-left of bottom
            -1.0f, -1.0f, -1.0f, // bottom-left of bottom
            1.0f, -1.0f, -1.0f,  // bottom-right of bottom
        };

    const GLfloat cubeColor[] =
        {
            // front
            1.0f, 0.0f, 0.0f, // top-right of front
            1.0f, 0.0f, 0.0f, // top-left of front
            1.0f, 0.0f, 0.0f, // bottom-left of front
            1.0f, 0.0f, 0.0f, // bottom-right of front

            // right
            0.0f, 0.0f, 1.0f, // top-right of right
            0.0f, 0.0f, 1.0f, // top-left of right
            0.0f, 0.0f, 1.0f, // bottom-left of right
            0.0f, 0.0f, 1.0f, // bottom-right of right

            // back
            1.0f, 1.0f, 0.0f, // top-right of back
            1.0f, 1.0f, 0.0f, // top-left of back
            1.0f, 1.0f, 0.0f, // bottom-left of back
            1.0f, 1.0f, 0.0f, // bottom-right of back

            // left
            1.0f, 0.0f, 1.0f, // top-right of left
            1.0f, 0.0f, 1.0f, // top-left of left
            1.0f, 0.0f, 1.0f, // bottom-left of left
            1.0f, 0.0f, 1.0f, // bottom-right of left

            // top
            0.0f, 1.0f, 0.0f, // top-right of top
            0.0f, 1.0f, 0.0f, // top-left of top
            0.0f, 1.0f, 0.0f, // bottom-left of top
            0.0f, 1.0f, 0.0f, // bottom-right of top

            // bottom
            1.0f, 0.5f, 0.0f, // top-right of bottom
            1.0f, 0.5f, 0.0f, // top-left of bottom
            1.0f, 0.5f, 0.0f, // bottom-left of bottom
            1.0f, 0.5f, 0.0f, // bottom-right of bottom
        };

    const GLfloat cubeTexCoords[] =
        {
            // front
            1.0f, 1.0f, // top-right of front
            0.0f, 1.0f, // top-left of front
            0.0f, 0.0f, // bottom-left of front
            1.0f, 0.0f, // bottom-right of front

            // right
            1.0f, 1.0f, // top-right of right
            0.0f, 1.0f, // top-left of right
            0.0f, 0.0f, // bottom-left of right
            1.0f, 0.0f, // bottom-right of right

            // back
            1.0f, 1.0f, // top-right of back
            0.0f, 1.0f, // top-left of back
            0.0f, 0.0f, // bottom-left of back
            1.0f, 0.0f, // bottom-right of back

            // left
            1.0f, 1.0f, // top-right of left
            0.0f, 1.0f, // top-left of left
            0.0f, 0.0f, // bottom-left of left
            1.0f, 0.0f, // bottom-right of left

            // top
            1.0f, 1.0f, // top-right of top
            0.0f, 1.0f, // top-left of top
            0.0f, 0.0f, // bottom-left of top
            1.0f, 0.0f, // bottom-right of top

            // bottom
            1.0f, 1.0f, // top-right of bottom
            0.0f, 1.0f, // top-left of bottom
            0.0f, 0.0f, // bottom-left of bottom
            1.0f, 0.0f, // bottom-right of bottom
        };

    const GLfloat cubeNormals[] =
        {
            // front surface
            0.0f, 0.0f, 1.0f, // top-right of front
            0.0f, 0.0f, 1.0f, // top-left of front
            0.0f, 0.0f, 1.0f, // bottom-left of front
            0.0f, 0.0f, 1.0f, // bottom-right of front

            // right surface
            1.0f, 0.0f, 0.0f, // top-right of right
            1.0f, 0.0f, 0.0f, // top-left of right
            1.0f, 0.0f, 0.0f, // bottom-left of right
            1.0f, 0.0f, 0.0f, // bottom-right of right

            // back surface
            0.0f, 0.0f, -1.0f, // top-right of back
            0.0f, 0.0f, -1.0f, // top-left of back
            0.0f, 0.0f, -1.0f, // bottom-left of back
            0.0f, 0.0f, -1.0f, // bottom-right of back

            // left surface
            -1.0f, 0.0f, 0.0f, // top-right of left
            -1.0f, 0.0f, 0.0f, // top-left of left
            -1.0f, 0.0f, 0.0f, // bottom-left of left
            -1.0f, 0.0f, 0.0f, // bottom-right of left

            // top surface
            0.0f, 1.0f, 0.0f, // top-right of top
            0.0f, 1.0f, 0.0f, // top-left of top
            0.0f, 1.0f, 0.0f, // bottom-left of top
            0.0f, 1.0f, 0.0f, // bottom-right of top

            // bottom surface
            0.0f, -1.0f, 0.0f, // top-right of bottom
            0.0f, -1.0f, 0.0f, // top-left of bottom
            0.0f, -1.0f, 0.0f, // bottom-left of bottom
            0.0f, -1.0f, 0.0f, // bottom-right of bottom
        };

    // ============================ ORTHO TRIANGLE =======================
    // VAO
    glGenVertexArrays(1, &VAO_ORTHO_WHITE_TRIANGLE);
    glBindVertexArray(VAO_ORTHO_WHITE_TRIANGLE);

    // VBO for Position
    glGenBuffers(1, &VBO_ORTHO_POS_TRIANGLE);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ORTHO_POS_TRIANGLE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(orthoTriangleVertices), orthoTriangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VBO for White Triangle
    glGenBuffers(1, &VBO_WHITE_TRIANGLE);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_WHITE_TRIANGLE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(whiteTriangle), whiteTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind VAO

    // ========================= WHITE QUAD =============================
    // VAO
    glGenVertexArrays(1, &VAO_WHITE_QUAD);
    glBindVertexArray(VAO_WHITE_QUAD);

    // VBO for Position
    glGenBuffers(1, &VBO_POS_QUAD);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_POS_QUAD);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VBO for White
    glGenBuffers(1, &VBO_WHITE_QUAD);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_WHITE_QUAD);
    glBufferData(GL_ARRAY_BUFFER, sizeof(whiteQuad), whiteQuad, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind VAO

    // ============================ PERSPECTIVE WHITE TRIANGLE ===================================
    // VAO
    glGenVertexArrays(1, &VAO_WHITE_TRIANGLE);
    glBindVertexArray(VAO_WHITE_TRIANGLE);

    // VBO for Position
    glGenBuffers(1, &VBO_POS_TRIANGLE);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_POS_TRIANGLE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VBO for White Triangle
    glGenBuffers(1, &VBO_WHITE_TRIANGLE);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_WHITE_TRIANGLE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(whiteTriangle), whiteTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind VAO

    // ========================== PERSPECTIVE COLORED TRIANGLE =================================
    // VAO
    glGenVertexArrays(1, &VAO_COLOR_TRIANGLE);
    glBindVertexArray(VAO_COLOR_TRIANGLE);

    // VBO for Position
    glGenBuffers(1, &VBO_POS_TRIANGLE);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_POS_TRIANGLE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VBO for Color
    glGenBuffers(1, &VBO_COLOR_TRIANGLE);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_COLOR_TRIANGLE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColor), triangleColor, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind VAO

    // ============================ COLORED QUAD ==================================
    // VAO
    glGenVertexArrays(1, &VAO_COLOR_QUAD);
    glBindVertexArray(VAO_COLOR_QUAD);

    // VBO for Position
    glGenBuffers(1, &VBO_POS_QUAD);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_POS_QUAD);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VBO for Color
    glGenBuffers(1, &VBO_COLOR_QUAD);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_COLOR_QUAD);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorQuad), colorQuad, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind VAO

    // ========================= WHITE PYRAMID =============================
    // VAO
    glGenVertexArrays(1, &VAO_WHITE_PYRAMID);
    glBindVertexArray(VAO_WHITE_PYRAMID);

    // VBO for Position
    glGenBuffers(1, &VBO_PYRAMID_POS);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PYRAMID_POS);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VBO for White Color
    glVertexAttrib3f(SSG_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

    // VBO for TexCoord
    glGenBuffers(1, &VBO_PYRAMID_TEX);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PYRAMID_TEX);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexCoords), pyramidTexCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VBO for normals
    glGenBuffers(1, &VBO_PYRAMID_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PYRAMID_NORMAL);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0); // Unbind VAO

    // ========================= COLORED PYRAMID =============================
    // VAO
    glGenVertexArrays(1, &VAO_COLORED_PYRAMID);
    glBindVertexArray(VAO_COLORED_PYRAMID);

    // VBO for Position
    glGenBuffers(1, &VBO_PYRAMID_POS);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PYRAMID_POS);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VBO for White Color
    glGenBuffers(1, &VBO_COLORED_PYRAMID);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_COLORED_PYRAMID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidColor), pyramidColor, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind VAO

    // ========================= WHITE CUBE =============================
    // VAO
    glGenVertexArrays(1, &VAO_WHITE_CUBE);
    glBindVertexArray(VAO_WHITE_CUBE);

    // VBO for Position
    glGenBuffers(1, &VBO_CUBE_POS);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_CUBE_POS);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VBO for White Color
    glVertexAttrib3f(SSG_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

    // VBO for TexCoord
    glGenBuffers(1, &VBO_CUBE_TEX);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_CUBE_TEX);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexCoords), cubeTexCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VBO for normals
    glGenBuffers(1, &VBO_CUBE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_CUBE_NORMAL);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0); // Unbind VAO

    // ========================= COLORED CUBE =============================
    // VAO
    glGenVertexArrays(1, &VAO_COLORED_CUBE);
    glBindVertexArray(VAO_COLORED_CUBE);

    // VBO for Position
    glGenBuffers(1, &VBO_CUBE_POS);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_CUBE_POS);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VBO for White Color
    glGenBuffers(1, &VBO_COLORED_CUBE);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_COLORED_CUBE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColor), cubeColor, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind VAO

    // ======================== TEXTURE =============================
    // VAO
    glGenVertexArrays(1, &VAO_TEXTUREQUAD);
    glBindVertexArray(VAO_TEXTUREQUAD);

    // VBO for Position
    glGenBuffers(1, &VBO_TEXTUREQUAD_POS);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEXTUREQUAD_POS);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VBO for TexCoord
    glGenBuffers(1, &VBO_QUAD_TEX);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_QUAD_TEX);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadTexCoords), quadTexCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind VAO

    // ======================== SPHERE =============================
    getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
    NumVertices = getNumberOfSphereVertices();
    NumElements = getNumberOfSphereElements();

    // vao
    glGenVertexArrays(1, &vao_sphere);
    glBindVertexArray(vao_sphere);

    // vbo position
    glGenBuffers(1, &vbo_sphere_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VBO Color
    glVertexAttrib3f(SSG_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

    // vbo normal
    glGenBuffers(1, &vbo_sphere_normal);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
    glVertexAttribPointer(SSG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SSG_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // vbo element
    glGenBuffers(1, &vbo_sphere_element);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // vao unbind
    glBindVertexArray(0);

    // Enabling Depth
    glClearDepth(1.0f);      // Compulsory
    glEnable(GL_DEPTH_TEST); // Compulsory
    glDepthFunc(GL_LEQUAL);  // Compulsaory

    // Set the clear of window to blue
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    light[0].ambient = vec4(0.0f, 0.0f, 0.0f, 1.0);
    light[0].diffused = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    light[0].specular = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    light[0].position = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    light[1].ambient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    light[1].diffused = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    light[1].specular = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    light[1].position = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    light[2].ambient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    light[2].diffused = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    light[2].specular = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    light[2].position = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // Tell OpenGL to enable Texture
    glEnable(GL_TEXTURE_2D);

    // Create Texture
    loadTexture(&TwoDTexture, "./resources/textures/jollyRoger.png");
    loadTexture(&stoneTexture, "./resources/textures/Stone.png");
    loadTexture(&marbleTexture, "./resources/textures/marble.png");

    // Create Texture
    const char *cubeMapFiles[6] =
    {
        "./resources/Park/posx.jpg",
        "./resources/Park/negx.jpg",
        "./resources/Park/posy.jpg",
        "./resources/Park/negy.jpg",
        "./resources/Park/posz.jpg",
        "./resources/Park/negz.jpg"
    };

    if(!LoadCubeMap(&cubeMapTexture, cubeMapFiles))
    {
        fprintf(gpfile, "Cube map texture loading failed.\n");
        return (-8);
    }

    // Call Checkerboard Textureloading Function
    loadProceduralTexture();

    // Initialize Perspective Projection Matrix
    perspectiveProjectionMatrix = vmath::mat4::identity();
    orthographicProjectionMatrix = vmath::mat4::identity();
    resize(WIN_WIDTH, WIN_HEIGHT);
    return (0);
}

void printGLInfo(void)
{
    GLint numExtensions;
    GLint i;

    // Code
    fprintf(gpfile, "OpenGL Vender : %s\n", glGetString(GL_VENDOR));
    fprintf(gpfile, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
    fprintf(gpfile, "OpenGL Version : %s \n", glGetString(GL_VERSION));
    fprintf(gpfile, "OpenGL Shading language version(GLSL) : %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Listing of supported extensions
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    for (i = 0; i < numExtensions; i++)
    {
        fprintf(gpfile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
    }
}

void resize(int width, int height)
{
    // Code
    if (height <= 0)
        height = 1;

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    // Set Perspective Projection Matrix
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

    // Set Orthographic Projection Matrix
    if (width <= height)
    {
        orthographicProjectionMatrix = vmath::ortho(-100.0f, 100.0f, -100.0f * ((GLfloat)height / (GLfloat)width), 100.0f * ((GLfloat)height / (GLfloat)width), -100.0f, 100.0f);
    }
    else
    {
        orthographicProjectionMatrix = vmath::ortho(-100.0f * ((GLfloat)width / (GLfloat)height), 100.0f * ((GLfloat)width / (GLfloat)height), -100.0f, 100.0f, -100.0f, 100.0f);
    }
}

void display(void)
{
    // Function Declarations
    void cubeMap(void);
    void whiteOrthoTriangle(void);
    void whiteTriangle(void);
    void coloredTriangle(void);
    void whiteQuad(void);
    void coloredQuad(void);
    void coloredTriangleAndQuad(void);
    void whiteTriangleAndQuad(void);
    void whitePyramid(void);
    void whiteCube(void);
    void whitePyramidAndCube(void);
    void coloredPyramid(void);
    void coloredCube(void);
    void coloredPyramidAndCube(void);
    void sphere(void);

    // Texture Function Declarations
    void _2DTexture(void);
    void stoneTextureOnPyramid(void);
    void marbleTextureOnCube(void);
    void pyramidAndCubeTexture(void);
    void checkerboardTextureOnQuad(void);

    // Lighting Function Declarations
    void diffusedLightCube(void);
    void diffusedLightPyramid(void);
    void diffusedLightSphere(void);

    void perVertexLight(void);
    void perFragmentLight(void);
    void threeLightsOnSphere(void);

    // Code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cubeMap();

    // --------- Ortho Triangle --------
    if (currentMode == IsWhiteOrthoTriangle)
        whiteOrthoTriangle();

    //-------- White Shapes ----------
    if (currentMode == IsWhiteTriangle)
        whiteTriangle();

    if (currentMode == IsWhiteQuad)
        whiteQuad();

    if (currentMode == IsWhiteTriangleAndQuad)
        whiteTriangleAndQuad();

    //-------- Colored Shapes ----------
    if (currentMode == IsColoredTriangle)
        coloredTriangle();

    if (currentMode == IsColoredQuad)
        coloredQuad();

    if (currentMode == IsColoredTriangleAndQuad)
        coloredTriangleAndQuad();

    // ------ 2D White Animation --------
    if (currentMode == IsWhite2DAnimatingTriangle)
        whiteTriangle();

    if (currentMode == IsWhite2DAnimatingQuad)
        whiteQuad();

    if (currentMode == IsWhite2DAnimatingTriangleAndQuad)
        whiteTriangleAndQuad();

    // ------- 2D Colored Animation ---------
    if (currentMode == IsColored2DAnimatingTriangle)
        coloredTriangle();

    if (currentMode == IsColored2DAnimatingQuad)
        coloredQuad();

    if (currentMode == IsColored2DAnimatingTriangleAndQuad)
        coloredTriangleAndQuad();

    // --------------- Pyramid ---------------
    if (currentMode == IsWhitePyramid)
        whitePyramid();

    if (currentMode == IsColoredPyramid)
        coloredPyramid();

    // --------------- Cube ---------------
    if (currentMode == IsWhiteCube)
        whiteCube();

    if (currentMode == IsColoredCube)
        coloredCube();

    // ----------- Both Geometries -------------
    if (currentMode == IsWhitePyramidAndCube)
        whitePyramidAndCube();

    if (currentMode == IsColoredPyramidAndCube)
        coloredPyramidAndCube();

    // --------------- Sphere ---------------
    if (currentMode == IsSphere)
        sphere();

    // Texture Functions
    if (currentMode == IsSmileyTexture)
        _2DTexture();

    if (currentMode == IsPyramidTexture)
        stoneTextureOnPyramid();

    if (currentMode == IsCubeTexture)
        marbleTextureOnCube();

    if (currentMode == IsPyraamidAndCubeTexture)
        pyramidAndCubeTexture();

    if (currentMode == IsCheckerboradTexture)
        checkerboardTextureOnQuad();

    // Lighting Function
    if (currentMode == IsDiffusedLightCube)
        diffusedLightCube();

    if (currentMode == IsDiffusedLightPyramid)
        diffusedLightPyramid();

    if (currentMode == IsDiffusedLightSphere)
        diffusedLightSphere();

    if (currentMode == IsPerVertexLight)
        perVertexLight();

    if (currentMode == IsPerFragmentLight)
        perFragmentLight();

    if (currentMode == IsThreeLights)
        threeLightsOnSphere();

    SwapBuffers(ghdc);
}

void update(void)
{
    // Code
    //----------- Cube Map Rotation -----------
    cubeAngle = cubeAngle + 0.05f;
    if (cubeAngle >= 360.0f)
        cubeAngle = cubeAngle - 360.0f;

    // ------ 2D White Animation --------
    if (currentMode == IsWhite2DAnimatingTriangle)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    if (currentMode == IsWhite2DAnimatingQuad)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    if (currentMode == IsWhite2DAnimatingTriangleAndQuad)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    // ------- 2D Colored Animation ---------
    if (currentMode == IsColored2DAnimatingTriangle)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    if (currentMode == IsColored2DAnimatingQuad)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    if (currentMode == IsColored2DAnimatingTriangleAndQuad)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    // -------------------------------------------------

    if (currentMode == IsWhitePyramid)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    if (currentMode == IsWhiteCube)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    if (currentMode == IsWhitePyramidAndCube)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    if (currentMode == IsColoredPyramid)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    if (currentMode == IsColoredCube)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    if (currentMode == IsColoredPyramidAndCube)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    if (currentMode == IsPyramidTexture)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    if (currentMode == IsCubeTexture)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    if (currentMode == IsPyraamidAndCubeTexture)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    // --------------- Lighting Animation ---------------
    // Diffused Light Cube
    if (currentMode == IsDiffusedLightCube)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    // Diffused Light Pyramid
    if (currentMode == IsDiffusedLightPyramid)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    // Diffused Light Sphere
    if (currentMode == IsDiffusedLightSphere)
    {
        animationAngle = animationAngle + 1.0f;
        if (animationAngle >= 360.0f)
            animationAngle = animationAngle - 360.0f;
    }

    // Three Lights on Sphere
    if (currentMode == IsThreeLights && IsLightEnabled == TRUE)
    {
        light[0].position[0] = 0.0f;
        light[0].position[1] = 10.0f * (GLfloat)cos(ligthAnimationAngle * M_PI / 180.0f);
        light[0].position[2] = 10.0f * (GLfloat)sin(ligthAnimationAngle * M_PI / 180.0f);
        light[0].position[3] = 1.0f;

        ligthAnimationAngle = ligthAnimationAngle + 0.2f;
        if (ligthAnimationAngle > 360.0f)
            ligthAnimationAngle = ligthAnimationAngle - 360.0f;

        light[1].position[0] = 10.0f * (GLfloat)cos(ligthAnimationAngle * M_PI / 180.0f);
        light[1].position[1] = 0.0f;
        light[1].position[2] = 10.0f * (GLfloat)sin(ligthAnimationAngle * M_PI / 180.0f);
        light[1].position[3] = 1.0f;

        ligthAnimationAngle = ligthAnimationAngle + 0.2f;
        if (ligthAnimationAngle > 360.0f)
            ligthAnimationAngle = ligthAnimationAngle - 360.0f;

        light[2].position[0] = 10.0f * (GLfloat)sin(ligthAnimationAngle * M_PI / 180.0f);
        light[2].position[1] = 10.0f * (GLfloat)cos(ligthAnimationAngle * M_PI / 180.0f);
        light[2].position[2] = 0.0f;
        light[2].position[3] = 1.0f;

        ligthAnimationAngle = ligthAnimationAngle + 0.2f;
        if (ligthAnimationAngle > 360.0f)
            ligthAnimationAngle = ligthAnimationAngle - 360.0f;
    }
}

void uninitialize(void)
{
    // Function Declarations
    void ToggleFullscreen(void);

    // Code
    if (shaderProgramObject)
    {
        glUseProgram(shaderProgramObject);

        GLint numShaders = 0;
        glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);
        if (numShaders > 0)
        {
            GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
            if (pShaders != NULL)
            {
                glGetAttachedShaders(shaderProgramObject, numShaders, NULL, pShaders);
                for (GLint i = 0; i < numShaders; i++)
                {
                    glDetachShader(shaderProgramObject, pShaders[i]);
                    glDeleteShader(pShaders[i]);
                    pShaders[i] = 0;
                }
                free(pShaders);
                pShaders = NULL;
            }
        }
        glUseProgram(0);
        glDeleteProgram(shaderProgramObject);
        shaderProgramObject = 0;
    }

    if (shaderProgramObject_texturing)
    {
        glUseProgram(shaderProgramObject_texturing);

        GLint numShaders = 0;
        glGetProgramiv(shaderProgramObject_texturing, GL_ATTACHED_SHADERS, &numShaders);
        if (numShaders > 0)
        {
            GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
            if (pShaders != NULL)
            {
                glGetAttachedShaders(shaderProgramObject_texturing, numShaders, NULL, pShaders);
                for (GLint i = 0; i < numShaders; i++)
                {
                    glDetachShader(shaderProgramObject_texturing, pShaders[i]);
                    glDeleteShader(pShaders[i]);
                    pShaders[i] = 0;
                }
                free(pShaders);
                pShaders = NULL;
            }
        }
        glUseProgram(0);
        glDeleteProgram(shaderProgramObject_texturing);
        shaderProgramObject_texturing = 0;
    }

    // Diffused Light Shader Program
    if (shaderProgramObject_diffusedLight)
    {
        glUseProgram(shaderProgramObject_diffusedLight);

        GLint numShaders = 0;
        glGetProgramiv(shaderProgramObject_diffusedLight, GL_ATTACHED_SHADERS, &numShaders);
        if (numShaders > 0)
        {
            GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
            if (pShaders != NULL)
            {
                glGetAttachedShaders(shaderProgramObject_diffusedLight, numShaders, NULL, pShaders);
                for (GLint i = 0; i < numShaders; i++)
                {
                    glDetachShader(shaderProgramObject_diffusedLight, pShaders[i]);
                    glDeleteShader(pShaders[i]);
                    pShaders[i] = 0;
                }
                free(pShaders);
                pShaders = NULL;
            }
        }
        glUseProgram(0);
        glDeleteProgram(shaderProgramObject_diffusedLight);
        shaderProgramObject_diffusedLight = 0;
    }

    // Per Vertex Light Shader Program
    if (shaderProgramObject_perVertexLight)
    {
        glUseProgram(shaderProgramObject_perVertexLight);
        GLint numShaders = 0;
        glGetProgramiv(shaderProgramObject_perVertexLight, GL_ATTACHED_SHADERS, &numShaders);
        if (numShaders > 0)
        {
            GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
            if (pShaders != NULL)
            {
                glGetAttachedShaders(shaderProgramObject_perVertexLight, numShaders, NULL, pShaders);
                for (GLint i = 0; i < numShaders; i++)
                {
                    glDetachShader(shaderProgramObject_perVertexLight, pShaders[i]);
                    glDeleteShader(pShaders[i]);
                    pShaders[i] = 0;
                }

                free(pShaders);
                pShaders = NULL;
            }
        }

        glUseProgram(0);
        glDeleteProgram(shaderProgramObject_perVertexLight);
        shaderProgramObject_perVertexLight = 0;
    }

    // Per Fragment Light Shader Program
    if (shaderProgramObject_perFragmentLight)
    {
        glUseProgram(shaderProgramObject_perFragmentLight);
        GLint numShaders = 0;
        glGetProgramiv(shaderProgramObject_perFragmentLight, GL_ATTACHED_SHADERS, &numShaders);
        if (numShaders > 0)
        {
            GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
            if (pShaders != NULL)
            {
                glGetAttachedShaders(shaderProgramObject_perFragmentLight, numShaders, NULL, pShaders);
                for (GLint i = 0; i < numShaders; i++)
                {
                    glDetachShader(shaderProgramObject_perFragmentLight, pShaders[i]);
                    glDeleteShader(pShaders[i]);
                    pShaders[i] = 0;
                }

                free(pShaders);
                pShaders = NULL;
            }
        }

        glUseProgram(0);
        glDeleteProgram(shaderProgramObject_perFragmentLight);
        shaderProgramObject_perFragmentLight = 0;
    }

    // Three Lights on Sphere Shader Uninitialization
    if (shaderProgramObject_threeLights)
    {
        glUseProgram(shaderProgramObject_threeLights);
        GLint numShaders = 0;
        glGetProgramiv(shaderProgramObject_threeLights, GL_ATTACHED_SHADERS, &numShaders);
        if (numShaders > 0)
        {
            GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
            if (pShaders != NULL)
            {
                glGetAttachedShaders(shaderProgramObject_threeLights, numShaders, NULL, pShaders);
                for (GLint i = 0; i < numShaders; i++)
                {
                    glDetachShader(shaderProgramObject_threeLights, pShaders[i]);
                    glDeleteShader(pShaders[i]);
                    pShaders[i] = 0;
                }
                free(pShaders);
                pShaders = NULL;
            }
        }
        glUseProgram(0);
        glDeleteProgram(shaderProgramObject_threeLights);
        shaderProgramObject_threeLights = 0;
    }

    // Cube Map Shader Uninitialization
    if (shaderProgramObject_cubeMap)
    {
        glUseProgram(shaderProgramObject_cubeMap);
        GLint numShaders = 0;
        glGetProgramiv(shaderProgramObject_cubeMap, GL_ATTACHED_SHADERS, &numShaders);
        if (numShaders > 0)
        {
            GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
            if (pShaders != NULL)
            {
                glGetAttachedShaders(shaderProgramObject_cubeMap, numShaders, NULL, pShaders);
                for (GLint i = 0; i < numShaders; i++)
                {
                    glDetachShader(shaderProgramObject_cubeMap, pShaders[i]);
                    glDeleteShader(pShaders[i]);
                    pShaders[i] = 0;
                }

                free(pShaders);
                pShaders = NULL;
            }
        }

        glUseProgram(0);
        glDeleteProgram(shaderProgramObject_cubeMap);
        shaderProgramObject_cubeMap = 0;
    }

    // Sphere
    // Delete VBO element
    if (vbo_sphere_element)
    {
        glDeleteBuffers(1, &vbo_sphere_element);
        vbo_sphere_element = 0;
    }

    // Delete VBO normal
    if (vbo_sphere_normal)
    {
        glDeleteBuffers(1, &vbo_sphere_normal);
        vbo_sphere_normal = 0;
    }

    // Delete VBO of Position
    if (vbo_sphere_position)
    {
        glDeleteBuffers(1, &vbo_sphere_position);
        vbo_sphere_position = 0;
    }

    // Delete VAO
    if (vao_sphere)
    {
        glDeleteVertexArrays(1, &vao_sphere);
        vao_sphere = 0;
    }

    // Texture
    if (VBO_QUAD_TEX)
    {
        glDeleteBuffers(1, &VBO_QUAD_TEX);
        VBO_QUAD_TEX = 0;
    }

    if (VBO_PYRAMID_TEX)
    {
        glDeleteBuffers(1, &VBO_PYRAMID_TEX);
        VBO_PYRAMID_TEX = 0;
    }

    if (VBO_CUBE_TEX)
    {
        glDeleteBuffers(1, &VBO_CUBE_TEX);
        VBO_CUBE_TEX = 0;
    }

    if (checkerboardTexture)
    {
        glDeleteTextures(1, &checkerboardTexture);
        checkerboardTexture = 0;
    }
    if (TwoDTexture)
    {
        glDeleteTextures(1, &TwoDTexture);
        TwoDTexture = 0;
    }
    if (stoneTexture)
    {
        glDeleteTextures(1, &stoneTexture);
        stoneTexture = 0;
    }
    if (marbleTexture)
    {
        glDeleteTextures(1, &marbleTexture);
        marbleTexture = 0;
    }
    if (cubeMapTexture)
    {
        glDeleteTextures(1, &cubeMapTexture);
        cubeMapTexture = 0;
    }

    // Cube
    if (VBO_CUBE_POS)
    {
        glDeleteBuffers(1, &VBO_CUBE_POS);
        VBO_CUBE_POS = 0;
    }

    if (VBO_WHITE_CUBE)
    {
        glDeleteBuffers(1, &VBO_WHITE_CUBE);
        VBO_WHITE_CUBE = 0;
    }

    if (VBO_COLORED_CUBE)
    {
        glDeleteBuffers(1, &VBO_COLORED_CUBE);
        VBO_COLORED_CUBE = 0;
    }

    if (VAO_COLORED_CUBE)
    {
        glDeleteVertexArrays(1, &VAO_COLORED_CUBE);
        VAO_COLORED_CUBE = 0;
    }

    if (VAO_WHITE_CUBE)
    {
        glDeleteVertexArrays(1, &VAO_WHITE_CUBE);
        VAO_WHITE_CUBE = 0;
    }

    // Pyramid
    if (VBO_PYRAMID_POS)
    {
        glDeleteBuffers(1, &VBO_PYRAMID_POS);
        VBO_PYRAMID_POS = 0;
    }

    if (VBO_WHITE_PYRAMID)
    {
        glDeleteBuffers(1, &VBO_WHITE_PYRAMID);
        VBO_WHITE_PYRAMID = 0;
    }

    if (VBO_COLORED_PYRAMID)
    {
        glDeleteBuffers(1, &VBO_COLORED_PYRAMID);
        VBO_COLORED_PYRAMID = 0;
    }

    if (VAO_COLORED_PYRAMID)
    {
        glDeleteVertexArrays(1, &VAO_COLORED_PYRAMID);
        VAO_COLORED_PYRAMID = 0;
    }

    if (VAO_WHITE_PYRAMID)
    {
        glDeleteVertexArrays(1, &VAO_WHITE_PYRAMID);
        VAO_WHITE_PYRAMID = 0;
    }

    //-----------------------------
    if (VBO_COLOR_TRIANGLE)
    {
        glDeleteBuffers(1, &VBO_COLOR_TRIANGLE);
        VBO_COLOR_TRIANGLE = 0;
    }
    if (VBO_WHITE_TRIANGLE)
    {
        glDeleteBuffers(1, &VBO_WHITE_TRIANGLE);
        VBO_WHITE_TRIANGLE = 0;
    }

    if (VBO_COLOR_QUAD)
    {
        glDeleteBuffers(1, &VBO_COLOR_QUAD);
        VBO_COLOR_QUAD = 0;
    }
    if (VBO_WHITE_QUAD)
    {
        glDeleteBuffers(1, &VBO_WHITE_QUAD);
        VBO_WHITE_QUAD = 0;
    }

    // Delete VBO of Position
    if (VBO_POS_TRIANGLE)
    {
        glDeleteBuffers(1, &VBO_POS_TRIANGLE);
        VBO_POS_TRIANGLE = 0;
    }
    if (VBO_POS_QUAD)
    {
        glDeleteBuffers(1, &VBO_POS_QUAD);
        VBO_POS_QUAD = 0;
    }

    if (VAO_COLOR_TRIANGLE)
    {
        glDeleteVertexArrays(1, &VAO_COLOR_TRIANGLE);
        VAO_COLOR_TRIANGLE = 0;
    }
    if (VAO_WHITE_TRIANGLE)
    {
        glDeleteVertexArrays(1, &VAO_WHITE_TRIANGLE);
        VAO_WHITE_TRIANGLE = 0;
    }

    if (VAO_COLOR_QUAD)
    {
        glDeleteVertexArrays(1, &VAO_COLOR_QUAD);
        VAO_COLOR_QUAD = 0;
    }
    if (VAO_WHITE_QUAD)
    {
        glDeleteVertexArrays(1, &VAO_WHITE_QUAD);
        VAO_WHITE_QUAD = 0;
    }

    // Light VBOs
    if (VBO_CUBE_NORMAL)
    {
        glDeleteBuffers(1, &VBO_CUBE_NORMAL);
        VBO_CUBE_NORMAL = 0;
    }

    if (VBO_PYRAMID_NORMAL)
    {
        glDeleteBuffers(1, &VBO_PYRAMID_NORMAL);
        VBO_PYRAMID_NORMAL = 0;
    }

    if (gbFullScreen == TRUE)
    {
        ToggleFullscreen();
        gbFullScreen = FALSE;
    }

    // Make the hdc as current context
    if (wglGetCurrentContext() == ghrc)
    {
        wglMakeCurrent(NULL, NULL);
    }

    // Destroy Rendering Context
    if (ghrc)
    {
        wglDeleteContext(ghrc);
        ghrc = NULL;
    }

    // Release the Handle to Device Context
    if (ghdc)
    {
        ReleaseDC(ghwnd, ghdc);
        ghdc = NULL;
    }

    // Get Rid of Window Handle
    if (ghwnd)
    {
        DestroyWindow(ghwnd);
        ghwnd = NULL;
    }

    // Close the Log File
    if (gpfile)
    {
        fprintf(gpfile, "Program Ended Successfully\n");
        fclose(gpfile);
        gpfile = NULL;
    }
}

void whiteTriangle(void)
{
    glUseProgram(shaderProgramObject);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix = rotationMatrix_Y;
    modelMatrix = translationMatrix * rotationMatrix;
    mat4 modelViewProjectionmatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    // Push above MVP into Vertex Shaders's MVP Uniform
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionmatrix);

    glBindVertexArray(VAO_WHITE_TRIANGLE);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glUseProgram(0);
}

void whiteQuad(void)
{
    glUseProgram(shaderProgramObject);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
    rotationMatrix_X = vmath::rotate(animationAngle, 1.0f, 0.0f, 0.0f);
    rotationMatrix = rotationMatrix_X;
    modelMatrix = translationMatrix * rotationMatrix;
    mat4 modelViewProjectionmatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    // Push above MVP into Vertex Shaders's MVP Uniform
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionmatrix);

    glBindVertexArray(VAO_WHITE_QUAD);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glUseProgram(0);
}

void coloredTriangle(void)
{
    glUseProgram(shaderProgramObject);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix = rotationMatrix_Y;
    modelMatrix = translationMatrix * rotationMatrix;
    mat4 modelViewProjectionmatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    // Push above MVP into Vertex Shaders's MVP Uniform
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionmatrix);

    glBindVertexArray(VAO_COLOR_TRIANGLE);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glUseProgram(0);
}

void coloredQuad(void)
{
    glUseProgram(shaderProgramObject);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
    rotationMatrix_X = vmath::rotate(animationAngle, 1.0f, 0.0f, 0.0f);
    rotationMatrix = rotationMatrix_X;
    modelMatrix = translationMatrix * rotationMatrix;
    mat4 modelViewProjectionmatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    // Push above MVP into Vertex Shaders's MVP Uniform
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionmatrix);

    glBindVertexArray(VAO_COLOR_QUAD);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glUseProgram(0);
}

void coloredTriangleAndQuad(void)
{
    // Code
    glUseProgram(shaderProgramObject);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Triangle
    // Transformation
    translationMatrix = vmath::translate(-2.0f, 0.0f, -6.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix = rotationMatrix_Y;
    modelMatrix = translationMatrix * rotationMatrix;
    mat4 modelViewProjectionmatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    // Push above MVP into Vertex Shaders's MVP Uniform
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionmatrix);

    glBindVertexArray(VAO_COLOR_TRIANGLE);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glUseProgram(0);

    glUseProgram(shaderProgramObject);

    // Quad
    // Transformation
    translationMatrix = vmath::translate(2.0f, 0.0f, -6.0f);
    rotationMatrix_X = vmath::rotate(animationAngle, 1.0f, 0.0f, 0.0f);
    rotationMatrix = rotationMatrix_X;
    modelMatrix = translationMatrix * rotationMatrix;
    modelViewProjectionmatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    // Push above MVP into Vertex Shaders's MVP Uniform
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionmatrix);

    glBindVertexArray(VAO_COLOR_QUAD);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glUseProgram(0);
}

void whiteTriangleAndQuad(void)
{
    // Use the shader program
    glUseProgram(shaderProgramObject);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Triangle Transformation
    translationMatrix = vmath::translate(-2.0f, 0.0f, -6.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix = rotationMatrix_Y; // Only rotation about Y-axis
    modelMatrix = translationMatrix * rotationMatrix;

    // Compute MVP matrix
    mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * viewMatrix * modelMatrix;

    // Push the MVP matrix into the vertex shader's uniform
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    // Draw the Triangle
    glBindVertexArray(VAO_WHITE_TRIANGLE);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    // Quad Transformation
    translationMatrix = vmath::translate(2.0f, 0.0f, -6.0f);
    rotationMatrix_X = vmath::rotate(animationAngle, 1.0f, 0.0f, 0.0f);
    rotationMatrix = rotationMatrix_X; // Only rotation about X-axis
    modelMatrix = translationMatrix * rotationMatrix;

    // Compute MVP matrix
    modelViewProjectionMatrix = perspectiveProjectionMatrix * viewMatrix * modelMatrix;

    // Push the MVP matrix into the vertex shader's uniform
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    // Draw the Quad
    glBindVertexArray(VAO_WHITE_QUAD);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    // Unbind the shader program
    glUseProgram(0);
}

void whiteOrthoTriangle(void)
{
    // Code
    glUseProgram(shaderProgramObject);

    // Transformation
    mat4 modelViewMatrix = vmath::mat4::identity();
    mat4 modelViewProjectionmatri = orthographicProjectionMatrix * modelViewMatrix; // order of multiplication is very importatnt

    /* Push above mvp into vertex shaders's mvp uniform */
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionmatri);
    glBindVertexArray(VAO_ORTHO_WHITE_TRIANGLE);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glUseProgram(0);
}

void whitePyramid(void)
{
    // Code
    glUseProgram(shaderProgramObject);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -7.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix = rotationMatrix_Y;
    modelMatrix = translationMatrix * rotationMatrix;
    mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(VAO_WHITE_PYRAMID);
    glDrawArrays(GL_TRIANGLES, 0, 12);
    glBindVertexArray(0);
    glUseProgram(0);
}

void whiteCube(void)
{
    // Code
    glUseProgram(shaderProgramObject);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -8.0f);
    rotationMatrix_X = vmath::rotate(animationAngle, 1.0f, 0.0f, 0.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix_Z = vmath::rotate(animationAngle, 0.0f, 0.0f, 1.0f);
    rotationMatrix = rotationMatrix_X * rotationMatrix_Y * rotationMatrix_Z;
    modelMatrix = translationMatrix * rotationMatrix;
    mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(VAO_WHITE_CUBE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
    glBindVertexArray(0);
    glUseProgram(0);
}

void whitePyramidAndCube(void)
{
    // Code

    // Pyramid
    glUseProgram(shaderProgramObject);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 scaleMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(-2.5f, 0.0f, -7.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix = rotationMatrix_Y;
    modelMatrix = translationMatrix * rotationMatrix;
    mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(VAO_WHITE_PYRAMID);
    glDrawArrays(GL_TRIANGLES, 0, 12);
    glBindVertexArray(0);
    glUseProgram(0);

    // Cube
    glUseProgram(shaderProgramObject);

    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();

    translationMatrix = mat4::identity();
    rotationMatrix = mat4::identity();

    rotationMatrix_X = mat4::identity();
    rotationMatrix_Y = mat4::identity();
    rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(2.5f, 0.0f, -8.0f);
    scaleMatrix = vmath::scale(0.9f, 0.9f, 0.9f);
    rotationMatrix_X = vmath::rotate(animationAngle, 1.0f, 0.0f, 0.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix_Z = vmath::rotate(animationAngle, 0.0f, 0.0f, 1.0f);
    rotationMatrix = rotationMatrix_X * rotationMatrix_Y * rotationMatrix_Z;
    modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(VAO_WHITE_CUBE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
    glBindVertexArray(0);
    glUseProgram(0);
}

void coloredPyramid(void)
{
    // Code
    glUseProgram(shaderProgramObject);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -7.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix = rotationMatrix_Y;
    modelMatrix = translationMatrix * rotationMatrix;
    mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(VAO_COLORED_PYRAMID);
    glDrawArrays(GL_TRIANGLES, 0, 12);
    glBindVertexArray(0);
    glUseProgram(0);
}

void coloredCube(void)
{
    // Code
    glUseProgram(shaderProgramObject);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -8.0f);
    rotationMatrix_X = vmath::rotate(animationAngle, 1.0f, 0.0f, 0.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix_Z = vmath::rotate(animationAngle, 0.0f, 0.0f, 1.0f);
    rotationMatrix = rotationMatrix_X * rotationMatrix_Y * rotationMatrix_Z;
    modelMatrix = translationMatrix * rotationMatrix;
    mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(VAO_COLORED_CUBE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
    glBindVertexArray(0);
    glUseProgram(0);
}

void coloredPyramidAndCube(void)
{
    // Cube
    glUseProgram(shaderProgramObject);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 scaleMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(-2.5f, 0.0f, -7.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix = rotationMatrix_Y;
    modelMatrix = translationMatrix * rotationMatrix;
    mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(VAO_COLORED_PYRAMID);
    glDrawArrays(GL_TRIANGLES, 0, 12);
    glBindVertexArray(0);
    glUseProgram(0);

    // Cube
    glUseProgram(shaderProgramObject);

    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();

    translationMatrix = mat4::identity();
    rotationMatrix = mat4::identity();

    rotationMatrix_X = mat4::identity();
    rotationMatrix_Y = mat4::identity();
    rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(2.5f, 0.0f, -8.0f);
    scaleMatrix = vmath::scale(0.9f, 0.9f, 0.9f);
    rotationMatrix_X = vmath::rotate(animationAngle, 1.0f, 0.0f, 0.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix_Z = vmath::rotate(animationAngle, 0.0f, 0.0f, 1.0f);
    rotationMatrix = rotationMatrix_X * rotationMatrix_Y * rotationMatrix_Z;
    modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(VAO_COLORED_CUBE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
    glBindVertexArray(0);
    glUseProgram(0);
}

void sphere(void)
{
    // Code
    glUseProgram(shaderProgramObject);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelMatrix = translationMatrix;
    mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(vao_sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, NumElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

BOOL loadTexture(GLuint *texture, const char *File)
{
    // Local Variable Declarations
    int width, height, channels;
    unsigned char *image = stbi_load(File, &width, &height, &channels, STBI_rgb_alpha);

    if (image)
    {
        bResult = TRUE;

        // Generate and bind texture
        glGenTextures(1, texture);
        glBindTexture(GL_TEXTURE_2D, *texture);

        // Set Texture Parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Horizontal wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Vertical wrapping

        // Upload the texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

        // Generate mipmaps for better quality
        glGenerateMipmap(GL_TEXTURE_2D);

        // Free the image data
        stbi_image_free(image);

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        fprintf(gpfile, "Failed to load texture: %s\n", stbi_failure_reason());
        bResult = FALSE;
    }
    return bResult;
}

void loadProceduralTexture(void)
{
    // Code
    // Local Function Declarations
    void makeCheckImage(void);

    makeCheckImage();

    glGenTextures(1, &checkerboardTexture);
    glBindTexture(GL_TEXTURE_2D, checkerboardTexture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Create multiple mipmap images
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECK_IMAGE_WIDTH, CHECK_IMAGE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

BOOL LoadCubeMap(GLuint *texture, const char *filePaths[6])
{
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *texture);

    for (GLuint i = 0; i < 6; i++)
    {
        int width, height, channels;
        unsigned char *image = stbi_load(filePaths[i], &width, &height, &channels, 0);
        if (image)
        {
            if (channels == 3)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            }
            else if (channels == 4)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            }
            stbi_image_free(image);
        }
        else
        {
            fprintf(gpfile, "Failed to load cube map texture.\n");
            return (FALSE);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return (TRUE);
}

void makeCheckImage(void)
{
    // Variable Declarations
    int i, j, c;

    // code
    for (i = 0; i < CHECK_IMAGE_HEIGHT; i++)
    {
        for (j = 0; j < CHECK_IMAGE_WIDTH; j++)
        {
            c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;

            checkImage[i][j][0] = (GLubyte)c;
            checkImage[i][j][1] = (GLubyte)c;
            checkImage[i][j][2] = (GLubyte)c;
            checkImage[i][j][3] = (GLubyte)255;
        }
    }
}

void _2DTexture(void)
{
    // Code
    glUseProgram(shaderProgramObject_texturing);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -5.0f);
    modelMatrix = translationMatrix;
    mat4 modelViewProjectionmatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TwoDTexture);
    glUniform1i(textureSamplerUniform, 0);

    // Push above MVP into Vertex Shaders's MVP Uniform
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionmatrix);
    glBindVertexArray(VAO_TEXTUREQUAD);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void stoneTextureOnPyramid(void)
{
    // Code
    glUseProgram(shaderProgramObject_texturing);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -7.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix = rotationMatrix_Y;
    modelMatrix = translationMatrix * rotationMatrix;
    mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, stoneTexture);
    glUniform1i(textureSamplerUniform, 0);

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(VAO_WHITE_PYRAMID);
    glDrawArrays(GL_TRIANGLES, 0, 12);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void marbleTextureOnCube(void)
{
    // Code
    glUseProgram(shaderProgramObject_texturing);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -8.0f);
    rotationMatrix_X = vmath::rotate(animationAngle, 1.0f, 0.0f, 0.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix_Z = vmath::rotate(animationAngle, 0.0f, 0.0f, 1.0f);
    rotationMatrix = rotationMatrix_X * rotationMatrix_Y * rotationMatrix_Z;
    modelMatrix = translationMatrix * rotationMatrix;
    mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, marbleTexture);
    glUniform1i(textureSamplerUniform, 0);

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(VAO_WHITE_CUBE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void checkerboardTextureOnQuad(void)
{
    GLfloat square_position[12];

    // Code
    glUseProgram(shaderProgramObject_texturing);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();
    mat4 modelViewProjectionMatrix = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -5.0f);
    modelMatrix = translationMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    // Push above MVP into Vertex Shaders's MVP Uniform
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(VAO_TEXTUREQUAD);

    square_position[0] = 0.0f;
    square_position[1] = 1.0f;
    square_position[2] = 0.0f;
    square_position[3] = -2.0f;
    square_position[4] = 1.0f;
    square_position[5] = 0.0f;
    square_position[6] = -2.0f;
    square_position[7] = -1.0f;
    square_position[8] = 0.0f;
    square_position[9] = 0.0f;
    square_position[10] = -1.0f;
    square_position[11] = 0.0f;

    glBindBuffer(GL_ARRAY_BUFFER, VBO_WHITE_QUAD);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_position), square_position, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, checkerboardTexture);
    glUniform1i(textureSamplerUniform, 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glUseProgram(0);
}

void pyramidAndCubeTexture(void)
{
    // Code
    glUseProgram(shaderProgramObject_texturing);

    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();
    mat4 scaleMatrix = mat4::identity();

    mat4 rotationMatrix_X = mat4::identity();
    mat4 rotationMatrix_Y = mat4::identity();
    mat4 rotationMatrix_Z = mat4::identity();

    // Pyramid
    // Transformation
    translationMatrix = vmath::translate(-2.0f, 0.0f, -6.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix = rotationMatrix_Y;
    modelMatrix = translationMatrix * rotationMatrix;
    mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, stoneTexture);
    glUniform1i(textureSamplerUniform, 0);

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(VAO_WHITE_PYRAMID);
    glDrawArrays(GL_TRIANGLES, 0, 12);
    glBindVertexArray(0);
    glUseProgram(0);

    // Cube
    glUseProgram(shaderProgramObject_texturing);

    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();

    translationMatrix = mat4::identity();
    rotationMatrix = mat4::identity();

    rotationMatrix_X = mat4::identity();
    rotationMatrix_Y = mat4::identity();
    rotationMatrix_Z = mat4::identity();

    // Transformation
    translationMatrix = vmath::translate(2.0f, 0.0f, -6.0f);
    scaleMatrix = vmath::scale(0.8f, 0.8f, 0.8f);
    rotationMatrix_X = vmath::rotate(animationAngle, 1.0f, 0.0f, 0.0f);
    rotationMatrix_Y = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix_Z = vmath::rotate(animationAngle, 0.0f, 0.0f, 1.0f);
    rotationMatrix = rotationMatrix_X * rotationMatrix_Y * rotationMatrix_Z;
    modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelMatrix; // Order of multiplication is very importatnt

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, marbleTexture);
    glUniform1i(textureSamplerUniform, 0);

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(VAO_WHITE_CUBE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void cubeMap(void)
{
    // Code
    glUseProgram(shaderProgramObject_cubeMap);

    mat4 modelViewMatrix = mat4::identity();
    mat4 translationMatrix = mat4::identity();
    mat4 scaleMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();
    mat4 modelViewProjectionMatrix = mat4::identity();

    translationMatrix = vmath::translate(0.0f, 0.0f, 0.0f);
    scaleMatrix = vmath::scale(10.0f, 10.0f, 10.0f);
    rotationMatrix = vmath::rotate(cubeAngle, 0.0f, 1.0f, 0.0f);

    modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    glUniform1i(textureSamplerUniformCubeMap, 0);

    /* Push above mvp into vertex shaders's mvp uniform */
    glUniformMatrix4fv(mvpMatrixUniformCubeMap, 1, GL_FALSE, modelViewProjectionMatrix);
    glBindVertexArray(VAO_COLORED_CUBE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
    glBindVertexArray(0);
    glUseProgram(0);
}

void diffusedLightCube(void)
{
    // Code
    glUseProgram(shaderProgramObject_diffusedLight);

    mat4 modelViewMatrix = mat4::identity();
    mat4 translationMatrix = mat4::identity();
    mat4 scaleMatrix = mat4::identity();
    mat4 rotationMatrix1 = mat4::identity();
    mat4 rotationMatrix2 = mat4::identity();
    mat4 rotationMatrix3 = mat4::identity();

    translationMatrix = vmath::translate(0.0f, 0.0f, -8.0f);
    scaleMatrix = vmath::scale(1.0f, 1.0f, 1.0f);
    rotationMatrix1 = vmath::rotate(animationAngle, 1.0f, 0.0f, 0.0f);
    rotationMatrix2 = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    rotationMatrix3 = vmath::rotate(animationAngle, 0.0f, 0.0f, 1.0f);
    mat4 rotationMatrix = rotationMatrix1 * rotationMatrix2 * rotationMatrix3;
    modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    
    glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_FALSE, modelViewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform_DifL, 1, GL_FALSE, perspectiveProjectionMatrix);

    if(IsLightEnabled == TRUE)
    {
        glUniform1i(keyPressedUniform_Difl, 1);
        glUniform3fv(lightDiffusedUniform, 1, lightDiffused_DifL);
        glUniform3fv(materialDiffusedUniform, 1, materialDiffused_DifL);
        glUniform4fv(lightPositionUniform, 1, lightPosition_DifL);
    }
    else
    {
        glUniform1i(keyPressedUniform_Difl, 0);
    }

    glBindVertexArray(VAO_WHITE_CUBE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
    glBindVertexArray(0);
    glUseProgram(0);
}

void diffusedLightPyramid(void)
{
    // Code
    glUseProgram(shaderProgramObject_diffusedLight);

    /* Pyramid */
    mat4 modelViewMatrix = mat4::identity();
    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();
    mat4 modelViewProjectionMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, -7.0f);
    rotationMatrix = vmath::rotate(animationAngle, 0.0f, 1.0f, 0.0f);
    modelViewMatrix = translationMatrix * rotationMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; // order of multiplication is very importatnt

    /* Push above mvp into vertex shaders's mvp uniform */
    glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_FALSE, modelViewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform_DifL, 1, GL_FALSE, perspectiveProjectionMatrix);

    if (IsLightEnabled == TRUE)
    {
        glUniform1i(keyPressedUniform_Difl, 1);
        glUniform3fv(lightDiffusedUniform, 1, lightDiffused_DifL);
        glUniform3fv(materialDiffusedUniform, 1, materialDiffused_DifL);
        glUniform4fv(lightPositionUniform, 1, lightPosition_DifL);
    }
    else
    {
        glUniform1i(keyPressedUniform_Difl, 0);
    }

    glBindVertexArray(VAO_WHITE_PYRAMID);
    glDrawArrays(GL_TRIANGLES, 0, 12);
    glBindVertexArray(0);
    glUseProgram(0);
}

void diffusedLightSphere(void)
{
    // Code
    glUseProgram(shaderProgramObject_diffusedLight);

    /* Cube */
    mat4 modelViewMatrix = mat4::identity();
    mat4 translationMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
    modelViewMatrix = translationMatrix;

    /* Push above mvp into vertex shaders's mvp uniform */
    glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_FALSE, modelViewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform_DifL, 1, GL_FALSE, perspectiveProjectionMatrix);

    if (IsLightEnabled == TRUE)
    {
        glUniform1i(keyPressedUniform_Difl, 1);
        glUniform3fv(lightDiffusedUniform, 1, lightDiffused_DifL);
        glUniform3fv(materialDiffusedUniform, 1, materialDiffused_DifL);
        glUniform4fv(lightPositionUniform, 1, lightPosition_DifL);
    }
    else
    {
        glUniform1i(keyPressedUniform_Difl, 0);
    }

    glBindVertexArray(vao_sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, NumElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void perVertexLight(void)
{
    // Code
    glUseProgram(shaderProgramObject_perVertexLight);

    // Sphere
    mat4 translationMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
    mat4 modelMatrix = translationMatrix;
    mat4 viewMatrix = mat4::identity();

    /* Push above mvp into vertex shaders's mvp uniform */
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    if (IsLightEnabled == TRUE)
    {
        glUniform1i(keyPressedUniform, 1);

        glUniform3fv(light_Diffused_Uniform, 1, lightDiffused);
        glUniform3fv(light_Ambient_Uniform, 1, lightAmbient);
        glUniform3fv(light_Specular_Uniform, 1, lightSpecular);
        glUniform4fv(light_Position_Uniform, 1, lightPosition);

        glUniform3fv(material_Diffused_Uniform, 1, materialDiffused);
        glUniform3fv(material_Ambient_Uniform, 1, materialAmbient);
        glUniform3fv(material_Specular_Uniform, 1, materialSpecular);
        glUniform1f(material_Shininess_Uniform, materialShininess);
    }
    else
    {
        glUniform1i(keyPressedUniform, 0);
    }

    glBindVertexArray(vao_sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, NumElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void perFragmentLight(void)
{
    // Code
    glUseProgram(shaderProgramObject_perFragmentLight);

    // Sphere
    mat4 translationMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
    mat4 modelMatrix = translationMatrix;
    mat4 viewMatrix = mat4::identity();

    /* Push above mvp into vertex shaders's mvp uniform */
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    if (IsLightEnabled == TRUE)
    {
        glUniform1i(keyPressedUniform, 1);

        glUniform3fv(light_Diffused_Uniform, 1, lightDiffused);
        glUniform3fv(light_Ambient_Uniform, 1, lightAmbient);
        glUniform3fv(light_Specular_Uniform, 1, lightSpecular);
        glUniform4fv(light_Position_Uniform, 1, lightPosition);

        glUniform3fv(material_Diffused_Uniform, 1, materialDiffused);
        glUniform3fv(material_Ambient_Uniform, 1, materialAmbient);
        glUniform3fv(material_Specular_Uniform, 1, materialSpecular);
        glUniform1f(material_Shininess_Uniform, materialShininess);
    }
    else
    {
        glUniform1i(keyPressedUniform, 0);
    }

    glBindVertexArray(vao_sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, NumElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void threeLightsOnSphere(void)
{
    // Code
    glUseProgram(shaderProgramObject_threeLights);

    // Sphere
    mat4 translationMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
    modelMatrix = translationMatrix;

    // Push above mvp into vertex shaders's mvp uniform
    glUniformMatrix4fv(modelMatrixUniform_3L, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform_3L, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform_3L, 1, GL_FALSE, perspectiveProjectionMatrix);

    if (IsLightEnabled == TRUE)
    {
        glUniform1i(keyPressedUniform, 1);

        glUniform3fv(light_Ambient_Uniform_3L[0], 1, light[0].ambient);
        glUniform3fv(light_Diffused_Uniform_3L[0], 1, light[0].diffused);
        glUniform3fv(light_Specular_Uniform_3L[0], 1, light[0].specular);
        glUniform4fv(light_Position_Uniform_3L[0], 1, light[0].position);

        glUniform3fv(light_Ambient_Uniform_3L[1], 1, light[1].ambient);
        glUniform3fv(light_Diffused_Uniform_3L[1], 1, light[1].diffused);
        glUniform3fv(light_Specular_Uniform_3L[1], 1, light[1].specular);
        glUniform4fv(light_Position_Uniform_3L[1], 1, light[1].position);

        glUniform3fv(light_Ambient_Uniform_3L[2], 1, light[2].ambient);
        glUniform3fv(light_Diffused_Uniform_3L[2], 1, light[2].diffused);
        glUniform3fv(light_Specular_Uniform_3L[2], 1, light[2].specular);
        glUniform4fv(light_Position_Uniform_3L[2], 1, light[2].position);

        glUniform3fv(material_Diffused_Uniform_3L, 1, materialDiffused_3L);
        glUniform3fv(material_Ambient_Uniform_3L, 1, materialAmbient_3L);
        glUniform3fv(material_Specular_Uniform_3L, 1, materialSpecular_3L);
        glUniform1f(material_Shininess_Uniform_3L, materialShininess);
    }
    else
    {
        glUniform1i(keyPressedUniform, 0);
    }

    glBindVertexArray(vao_sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, NumElements, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}