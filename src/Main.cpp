#include <windows.h>
#include <GL/gl.h>
#include <iostream>
#include <vector>
#include "GL/wglext.h"
#include "Window.h"
#include "OpenGLContext.h"
#include "Shader.h"
#include "GLLoader.h"
#include "Math3D.h"
#include "Helpers.h"
#include "Mesh.h"
#include "Loader.h"
#include "RubiksCube.h"
#include <map>

std::map<int, bool> keyPreviousState;

static bool IsKeyPressedOnce(int vKey) {
    bool isPressedNow = (GetAsyncKeyState(vKey) & 0x8000) != 0;

    bool wasPressedLast = keyPreviousState[vKey];
    keyPreviousState[vKey] = isPressedNow;
    return isPressedNow && !wasPressedLast;
}
static void ProcessInput(RubiksCube& cube, Shader& shader) {
    bool isShiftHeld = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool clockwise = !isShiftHeld;

    // R - Right (Prawa œcianka)
    if (IsKeyPressedOnce('R')) {
        cube.QueueRotation(Axis::X, Layer::Positive, clockwise);
    }
    // L - Left (Lewa œcianka)
    if (IsKeyPressedOnce('L')) {
        cube.QueueRotation(Axis::X, Layer::Negative, clockwise);
    }

    // U - Up (Górna œcianka)
    if (IsKeyPressedOnce('U')) {
        cube.QueueRotation(Axis::Y, Layer::Positive, clockwise);
    }
    // D - Down (Dolna œcianka)
    if (IsKeyPressedOnce('D')) {
        cube.QueueRotation(Axis::Y, Layer::Negative, clockwise);
    }

    // F - Front (Przednia œcianka)
    if (IsKeyPressedOnce('F')) {
        cube.QueueRotation(Axis::Z, Layer::Positive, clockwise);
    }
    // B - Back (Tylna œcianka)
    if (IsKeyPressedOnce('B')) {
        cube.QueueRotation(Axis::Z, Layer::Negative, clockwise);
    }

    if (IsKeyPressedOnce('S')) {
        shader.reload();
    }
}

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    Window win(800, 600, L"Rubic's Cube");
    OpenGLContext gl;
    
    if (!gl.Init(win.GetHDC())) {
        MessageBox(NULL, L"Failed to initialize OpenGL 4.6!", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    Shader myShader("Shaders/VertexShader.vert", "Shaders/FragmentShader.frag");
    myShader.use();

    RubiksCube myCube1;
    myCube1.Init("Resource/CubeTexture.png", "Resource/Cubes/");

    Mat4 projection = Mat4::Perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    Mat4 view = Mat4::Translation(Vec3(0.0f, 0.0f, -3.0f));


    //TODOJK - inputy(moga byc same klawisze)  - klasa kamera  - kostak rubika rendering  - jaka architektura jak usturktyryzwoac
    //TODO remove unused includes
    Mat4 scaleMatrix = Mat4::Scale(10.0f);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Wireframe
    while (win.IsOpen()) {
        float currentFrame = static_cast<float>(GetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        win.ProcessMessages();

        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Quat rot = Quat::FromAxisAngle(Vec3(1, 1, 0).Normalized(), currentFrame * 30.0f);
        Vec3 pos(sinf(currentFrame) * 0.0f, sinf(currentFrame)*0.0f, 0.0f);
        DualQuat dq = DualQuat::FromRotationTranslation(rot, pos);
        Mat4 modelFromDQ = dq.ToMat4();
        Mat4 model = modelFromDQ * scaleMatrix;


        myShader.use();
        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);
        //myShader.setMat4("model", model);
        
        win.ProcessMessages();
        ProcessInput(myCube1, myShader);
        myCube1.Update(deltaTime);
        myCube1.Draw(myShader, model);

        win.Swap();
    }

    return 0;
}