#include <windows.h>
#include <GL/gl.h>
#include <map>
#include <chrono>

#include "Window.h"
#include "OpenGLContext.h"
#include "Shader.h"
#include "Math3D.h"
#include "Helpers.h"
#include "RubiksCube.h"
#include "Camera.h"


std::map<int, bool> keyPreviousState;
bool wireframeModeOn = false;
static bool isTimerRunning = false;
static std::chrono::steady_clock::time_point timerStart;
static bool drawTable = true;

static bool IsKeyPressedOnce(int vKey) {
    bool isPressedNow = (GetAsyncKeyState(vKey) & 0x8000) != 0;

    bool wasPressedLast = keyPreviousState[vKey];
    keyPreviousState[vKey] = isPressedNow;
    return isPressedNow && !wasPressedLast;
}

void PrintControls() {
    std::cout << "========================================================\n";
    std::cout << "                RUBIK'S CUBE CONTROLS                   \n";
    std::cout << "========================================================\n";
    std::cout << " [R] - Right Face               [L] - Left Face         \n";
    std::cout << " [U] - Up Face                  [D] - Down Face         \n";
    std::cout << " [F] - Front Face               [B] - Back Face         \n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << " [SHIFT] + Key - Rotate in opposite direction           \n";
    std::cout << " [Q] - Scramble Cube                                    \n";
    std::cout << " [P] - Undo All Moves                                   \n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << " [W] - Toggle Wireframe Mode                            \n";
    std::cout << " [X] - Toggle Table Visibility                          \n";
    std::cout << " [S] - Reload Shaders                                   \n";
    std::cout << " [O] - Reset Camera Position                            \n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << " MOUSE:\n";
    std::cout << " [LMB + Drag] - Orbit camera around the cube           \n";
    std::cout << " [Scroll Wheel] - Zoom In / Out                         \n";
    std::cout << "========================================================\n" << std::endl;
}

static void ProcessInput(RubiksCube& cube, Shader& shader, Camera& camera) {
    bool isShiftHeld = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool clockwise = !isShiftHeld;

    if (IsKeyPressedOnce('T')) {
        if (!isTimerRunning) {
            isTimerRunning = true;
            timerStart = std::chrono::steady_clock::now();
            std::cout << "\n>>> Timer started! <<<\n" << std::endl;
        }
        else {
            isTimerRunning = false;
            auto timerEnd = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsedSeconds = timerEnd - timerStart;

            std::cout << ">>> Timer stopped! <<<\n";
            std::cout << "Time elapsed: "
                << std::fixed << std::setprecision(3) << elapsedSeconds.count()
                << " s\n" << std::endl;
        }
    }

    if (IsKeyPressedOnce('R')) {
        cube.QueueRotation(Axis::X, Layer::Positive, clockwise);
    }
    if (IsKeyPressedOnce('L')) {
        cube.QueueRotation(Axis::X, Layer::Negative, clockwise);
    }
    if (IsKeyPressedOnce('U')) {
        cube.QueueRotation(Axis::Y, Layer::Positive, clockwise);
    }
    if (IsKeyPressedOnce('D')) {
        cube.QueueRotation(Axis::Y, Layer::Negative, clockwise);
    }
    if (IsKeyPressedOnce('B')) {
        cube.QueueRotation(Axis::Z, Layer::Positive, clockwise);
    }
    if (IsKeyPressedOnce('F')) {
        cube.QueueRotation(Axis::Z, Layer::Negative, clockwise);
    }

    if (IsKeyPressedOnce('S')) {
        shader.reload();
    }

    if (IsKeyPressedOnce('Q')) {
        cube.Scramble(20);
    }

    if (IsKeyPressedOnce('W'))
    {
        wireframeModeOn = !wireframeModeOn;
        if (wireframeModeOn) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Wireframe
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    if (IsKeyPressedOnce('X'))
    {
        drawTable = !drawTable;
    }

    if (IsKeyPressedOnce('O'))
    {
        camera.Reset();
    }
    if (IsKeyPressedOnce('P'))
    {
        cube.UndoAll();
    }

    if (IsKeyPressedOnce('H'))
    {
        PrintControls();
    }
}

float deltaTime = 0.0f;
float lastFrame = 0.0f;


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    PrintControls();
    Window win(800, 600, L"Rubic's Cube");
    OpenGLContext gl;
    
    if (!gl.Init(win.GetHDC())) {
        MessageBox(NULL, L"Failed to initialize OpenGL 4.6!", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    Shader myShader("Shaders/VertexShader.vert", "Shaders/FragmentShader.frag");
    myShader.use();

    RubiksCube Cube;
    Cube.Init("Resource/CubeTexture.png", "Resource/Cubes/");

    //Table
    std::vector<Vertex> tableVertices;
    Loader::LoadOBJ("Resource/table.obj", tableVertices);
    std::vector<unsigned int> objIndices;

    std::vector<Texture> textures;

    Texture diffMap;
    diffMap.id = Loader::LoadTexture("Resource/Table_BaseColor.png");
    diffMap.type = "diffuse";
    textures.push_back(diffMap);

    Texture specMap;
    specMap.id = Loader::LoadTexture("Resource/Table_MetallicRoughness.png");
    specMap.type = "specular";
    textures.push_back(specMap);

    Mesh tableMesh(tableVertices, objIndices, textures);
    //--------

    Camera camera(8.0f);

    bool isDragging = false;
    int lastMouseX = 0;
    int lastMouseY = 0;

    Mat4 projection = Mat4::Perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    //Mat4 view = Mat4::Translation(Vec3(0.0f, 0.0f, -3.0f));

    //TODO remove unused includes
    Mat4 scaleMatrix = Mat4::Scale(10.0f);

    //----------------------------------------------------------------
    // floor
    std::vector<Vertex> floorVertices = {
        { { 25.0f, -2.0f,  25.0f}, {0.0f, 1.0f, 0.0f}, {10.0f, 0.0f} },
        { {-25.0f, -2.0f,  25.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
        { {-25.0f, -2.0f, -25.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 10.0f} },
        { { 25.0f, -2.0f, -25.0f}, {0.0f, 1.0f, 0.0f}, {10.0f, 10.0f} }
    };
    std::vector<unsigned int> floorIndices = {
        0, 2, 1,
        0, 3, 2
    };
    std::vector<Texture> floorTextures;
    Texture texWood;
    texWood.id = Loader::LoadTexture("Resource/wall.jpg");
    texWood.type = "diffuse";
    floorTextures.push_back(texWood);

    Texture texDirt;
    texDirt.id = Loader::LoadTexture("Resource/Tiles.jpg");
    texDirt.type = "diffuse";
    floorTextures.push_back(texDirt);

    Mesh floorMesh(floorVertices, floorIndices, floorTextures);
    //----------------------------------------------------------------

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    while (win.IsOpen()) {
        float currentFrame = static_cast<float>(GetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        win.ProcessMessages();
        MouseInput mouse = win.GetAndResetMouseInput();
        if (mouse.isLeftBtnDown && (mouse.moveX != 0 || mouse.moveY != 0)) {
            camera.ProcessMouseMovement(mouse.moveX, mouse.moveY);
        }

        if (mouse.scroll != 0) {
            camera.ProcessMouseScroll(mouse.scroll);
        }
        ProcessInput(Cube, myShader, camera);

        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Quat rot = Quat::FromAxisAngle(Vec3(1, 1, 1).Normalized(), currentFrame * -30.0f);
        Quat rot = Quat::Identity();
        Vec3 pos(cosf(currentFrame) * 0.0f, cosf(currentFrame)*0.1f + 1.5f, 0.0f);
        DualQuat dq = DualQuat::FromRotationTranslation(rot, pos);
        Mat4 modelFromDQ = dq.ToMat4();
        Mat4 model = modelFromDQ * scaleMatrix;


        myShader.use();
        projection = Mat4::Perspective(45.0f, win.GetAspectRatio(), 0.1f, 100.0f);
        myShader.setMat4("projection", projection);
        
        Mat4 view = camera.GetViewMatrix();
        myShader.setMat4("view", view);
        //myShader.setMat4("model", model);

        //TODO cleanup
        //Vec3 ligthPos = Vec3(cosf(currentFrame * 0.5f)* 1.5f, -1.0f, cosf(currentFrame * 0.25f) *0.0f);
        myShader.setVec3("viewPos", camera.Position);

        myShader.setFloat("material.shininess", 64.0f);
        myShader.setFloat("specularStrength", 0.8f);

        //Ligths
        float x1 = cosf(currentFrame * 0.3f);
        float z1 = sinf(currentFrame * 0.3f);
        Vec3 dir1(x1, -0.3f, z1);
        Vec3 dir2(-x1, 0.3f, -z1);
        std::array<DirectionalLight, 4> dirLights;
        dirLights[0].direction = dir1;
        dirLights[0].color = Vec3(1.0f, 0.8f, 1.0f);
        dirLights[1].direction = dir2;
        dirLights[1].color = Vec3(1.0f, 1.0f, 0.8f);

        myShader.setInt("nrOfDirLights", (int)dirLights.size());
        
        for (size_t i = 0; i < dirLights.size(); i++)
        {
            std::string baseName = "dirLights[" + std::to_string(i) + "]";

            myShader.setVec3(baseName + ".direction", dirLights[i].direction);
            myShader.setVec3(baseName + ".color", dirLights[i].color);
        }

        myShader.setVec3("spotLight.position", Vec3(0.0f, 6.0f, 0.0f));
        myShader.setVec3("spotLight.direction", Vec3(0.0f, -1.0f, 0.0f));
        myShader.setVec3("spotLight.color", Vec3(1.0f, 1.0f, 1.0f));

        myShader.setFloat("spotLight.constant", 1.0f);
        myShader.setFloat("spotLight.linear", 0.09f);
        myShader.setFloat("spotLight.quadratic", 0.032f);

        myShader.setFloat("spotLight.cutOff", cosf(12.5f * 3.14159f / 180.0f));
        myShader.setFloat("spotLight.outerCutOff", cosf(17.5f * 3.14159f / 180.0f));
        
        myShader.setFloat("mixAlpha", 0.0f);
        Cube.Update(deltaTime);
        Cube.Draw(myShader, model);
        if (Cube.IsSolved() && isTimerRunning)
        {
            isTimerRunning = false;
            auto timerEnd = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsedSeconds = timerEnd - timerStart;

            std::cout << ">>> Timer stopped! <<<\n";
            std::cout << "Time elapsed: "
                << std::fixed << std::setprecision(3) << elapsedSeconds.count()
                << " s\n" << std::endl;
        }

        Mat4 floorModel = Mat4::Identity();
        myShader.setFloat("material.shininess", 256.0f);
        myShader.setFloat("specularStrength", 0.01f);
        myShader.setMat4("model", floorModel);
        myShader.setFloat("mixAlpha", 0.3f);
        floorMesh.Draw(myShader);

        if (drawTable) {
            Mat4 tableModel = Mat4::Identity();
            Mat4 tableScaleMatrix = Mat4::Scale(3.0f);
            Quat tableRot = Quat::FromAxisAngle(Vec3(0.0f, 1.0f, 0.0f), 3.14159f);
            DualQuat tabledq = DualQuat::FromRotationTranslation(tableRot, Vec3(0.0f, -2.0f, 0.0f));
            tableModel = tabledq.ToMat4() * tableScaleMatrix;

            myShader.setMat4("model", tableModel);
            myShader.setFloat("mixAlpha", 0.0f);
            tableMesh.Draw(myShader);
        }

        win.Swap();
    }

    return 0;
}