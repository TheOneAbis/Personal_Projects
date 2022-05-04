//NOTE: SFML put y=0 at the top, so when y increments everything goes down
// This is a WIP build, so expect bugs

#include "Mesh.h"
#include "Torus.h"
#include "Matrices.h"
#include "GridXZ.h"
#include "Terrain.h"

#define SFML_STATIC
#include <SFML\Window.hpp>
#include <SFML\Graphics.hpp>
#include <thread>
#include <random>

//Detecting memory leaks
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define DarkGrey Color(65, 65, 65)
#define TickTime 0.15f

float theta, phi, nearClip, farClip, fov;
float camSpeed;
Camera mainCam(Vector3D(0, -3, 0), Vector3D(0, 0, 1));
Vector2i currentM, previousM, mouseVel;

// Triangle Buffers
vector<Triangle3D*> worldTriBuffer; // points to every triangle in the world
vector<Triangle3D*> subWorldBuffer1;
vector<Triangle3D*> subWorldBuffer2;
vector<Triangle3D*> subWorldBuffer3;
vector<Triangle3D*> subWorldBuffer4;

vector<SimpleTri3D> screenTriBuffer1;
vector<SimpleTri3D> screenTriBuffer2;
vector<SimpleTri3D>* drawPtr = &screenTriBuffer1;
vector<SimpleTri3D>* mainPtr = &screenTriBuffer2;

vector<SimpleTri3D> subBuffer1;
vector<SimpleTri3D> subBuffer2;
vector<SimpleTri3D> subBuffer3;
vector<SimpleTri3D> subBuffer4;

bool t1done = true, t2done = true, t3done = true, t4done = true;

// light direction vector
Vector3D lightDir;

bool closeWindow = false;

// meshes
Mesh* sword1;
Mesh* sword2;
Mesh* sword3;
Mesh* sword4;
Torus* ringSmall;
Torus* ringMed;
Torus* ringLarge;
Terrain* terrain;

SimpleTri3D* test;

vector<Mesh*> meshes;
GridXZ grid;

//  UI ELEMENTS

// Containers and Buttons
vector<RectangleShape> UIElems;
RectangleShape createShapesContainer;
RectangleShape createCubeButton;
RectangleShape createSphereButton;
RectangleShape createTorusButton;
RectangleShape createCylinderButton;
RectangleShape createConeButton;

// Text font
Font font;
Text instructions;
Text mainFPSCounter;
Text updateFPSCounter;
Text drawFPSCounter;

void SetTriViews(vector<Triangle3D*>& originalBuffer)
{
    float** cameraMat = Mat4x4::PointAt(mainCam.position, mainCam.Target, mainCam.GetUp());
    float** viewMat = Mat4x4::SimpleInverse(cameraMat);

    for (Triangle3D* tri : originalBuffer)
    {
        tri->SetNormal(Vector3D::Cross((*tri)[1] - (*tri)[0], (*tri)[2] - (*tri)[0]).Normalized());

        // Calculate triangle's viewed triangle points
        for (int i = 0; i < tri->Count(); i++)
            tri->viewed.p[i] = Mat4x4::MultiplyVectorByMatrix4((*tri)[i], viewMat, false);

        tri->SetCamNormal(Vector3D::Cross(tri->viewed.p[1] - tri->viewed.p[0], tri->viewed.p[2] - tri->viewed.p[0]).Normalized());
    }
    DeleteMatrix(cameraMat, 4); // Delete the camera PointAt matrix array
    DeleteMatrix(viewMat, 4); // Delete the Camera LookAt matrix array
}

void ProjectTris(RenderWindow* window, vector<Triangle3D*>& originalBuffer, vector<SimpleTri3D>& addToBuffer, bool* isDone)
{
    while (!closeWindow)
    {
        if (!*isDone)
        {
            float sceneWidth = (float)window->getSize().x, sceneHeight = (float)window->getSize().y;
            float aspectRatio = sceneHeight / sceneWidth;

            for (size_t i = 0; i < originalBuffer.size(); i++)
            {
                // Only project & draw visible triangles (if angle between normal and any pt on the tri relative to camera is <= 90deg)
                if (originalBuffer[i]->CamNormal().Dot(originalBuffer[i]->viewed.p[0].Normalized()) < 0.0)
                {
                    int shade = (int)Clamp(255 * (originalBuffer[i]->Normal() * lightDir), 30, 225);

                    SimpleTri3D clipped[2];
                    int clippedTrisAmt = ClipTriAgainstPlane(Vector3D(0, 0, nearClip), Vector3D(0, 0, 1), originalBuffer[i]->viewed, clipped[0], clipped[1]);

                    // Project the tri's clipped tris
                    for (int n = 0; n < clippedTrisAmt; n++)
                    {
                        VertexArray displayTri(Triangles, 3);
                        for (int i = 0; i < 3; i++)
                        {
                            // Project triangles to 2D screen using Projection
                            Vector3D projVector = Mat4x4::MultiplyVectorByMatrix4(clipped[n].p[i], Mat4x4::Projection(nearClip, farClip, fov, aspectRatio), true);

                            projVector.x += 1.0; // Move mesh to middle of screen
                            projVector.y += 1.0;
                            projVector.x *= 0.5f * sceneWidth; // scale it out from 1px
                            projVector.y *= 0.5f * sceneHeight;

                            // Create vertex with vector position and shade color, append it to triangle VertexArrays
                            Vertex projVector2D(Vector2f(projVector.x, projVector.y), Color(shade, shade, shade, 255));
                            displayTri[i] = projVector2D;
                        }
                        clipped[n].projected = displayTri;
                        addToBuffer.push_back(clipped[n]);
                    }
                }
            }
            *isDone = true;
        }
    }
}

void drawThread(RenderWindow* window)
{
    window->setActive(true);

    Clock time;
    float dt;
    float update = 0;

    while (!closeWindow)
    {
        dt = time.restart().asSeconds();
        update += dt;
        if (update >= TickTime)
        {
            drawFPSCounter.setString("Draw Thead FPS:  " + to_string((int)(1 / dt)));
            update = update - TickTime;
        }

        SetTriViews(worldTriBuffer);

        window->clear(DarkGrey);
        
        grid.Draw(window);

        // Draw the triangles
        for (size_t i = 0; i < drawPtr->size(); i++)
            window->draw((*drawPtr)[i].GetProjected());

        window->draw(instructions);
        window->draw(mainFPSCounter);
        window->draw(drawFPSCounter);
        window->draw(updateFPSCounter);
        for (RectangleShape& elem : UIElems)
            window->draw(elem);
        window->display();
    }
}

void updateThread(RenderWindow* window)
{
    Clock time;
    float dt;
    float update = 0;
    const float tickrate = 1.0f / 30.0f;

    while (!closeWindow)
    {
        dt = time.restart().asSeconds();

        update += dt;
        if (update >= TickTime)
        {
            updateFPSCounter.setString("Update Thread FPS:  " + to_string((int)(1 / dt)));
            update = update - TickTime;
        }

        lightDir = mainCam.Forward * -1;
    }
}

void ControlsThread(RenderWindow* window)
{
    Clock time;
    float dt;
    Vector3D forwardMovt, rightMovt, upMovt;
    bool shapesWindow = false;
    bool shapesKey = false;

    while (!closeWindow)
    {
        currentM = Mouse::getPosition();
        dt = time.restart().asSeconds();

        mouseVel = (currentM - previousM);

        forwardMovt = Vector3D(mainCam.Forward.x, 0, mainCam.Forward.z).Normalized() * (dt * camSpeed);
        rightMovt = mainCam.Right * (dt * camSpeed);
        upMovt = GlobalUp() * (dt * camSpeed);

        // Movement Inputs
        if (Keyboard::isKeyPressed(Keyboard::W))
            mainCam.SetPosition(mainCam.GetPosition() + forwardMovt);
        if (Keyboard::isKeyPressed(Keyboard::A))
            mainCam.SetPosition(mainCam.GetPosition() - rightMovt);
        if (Keyboard::isKeyPressed(Keyboard::S))
            mainCam.SetPosition(mainCam.GetPosition() - forwardMovt);
        if (Keyboard::isKeyPressed(Keyboard::D))
            mainCam.SetPosition(mainCam.GetPosition() + rightMovt);
        if (Keyboard::isKeyPressed(Keyboard::Space))
            mainCam.SetPosition(mainCam.GetPosition() - upMovt);
        if (Keyboard::isKeyPressed(Keyboard::LControl))
            mainCam.SetPosition(mainCam.GetPosition() + upMovt);
        if (Keyboard::isKeyPressed(Keyboard::LShift))
            camSpeed = 25;
        else camSpeed = 10;

        // Update Create Shape UI display depending on state
        UIElems[0].setFillColor(Color(35, 35, 35, shapesWindow ? 255 : 0));
        for (size_t i = 1; i < UIElems.size(); i++)
            UIElems[i].setFillColor(Color(30, 30, 30, shapesWindow ? 255 : 0));

        // UI Stuff
        if (Keyboard::isKeyPressed(Keyboard::E))
            shapesKey = true;
        else if (shapesKey)
        {
            shapesKey = false;
            shapesWindow = !shapesWindow;
        }

        // Create SHapes UI Interaction
        if (shapesWindow)
        {
            if (Mouse::isButtonPressed(Mouse::Left))
            {
                if (currentM.x > createTorusButton.getPosition().x && currentM.x < createTorusButton.getPosition().x + createTorusButton.getSize().x &&
                    currentM.y > createTorusButton.getPosition().y && currentM.y < createTorusButton.getPosition().y + createTorusButton.getSize().y)
                {
                    shapesWindow = false;
                    meshes.push_back(new Torus(2, 0.3f, 10, 10, Vector3D(0, 0, 0)));
                }
            }
        }
        // Mouse Look Input
        else
        {
            mainCam.RotateX((float)mouseVel.y / 200);
            mainCam.RotateY((float)-mouseVel.x / 200);

            // Keep look direction from going upside down
            if (mainCam.Forward * mainCam.forwardOnXZ <= 0)
            {
                mainCam.Forward = Vector3D(mainCam.forwardOnXZ.x * 0.01f, mainCam.Forward.y / abs(mainCam.Forward.y), mainCam.forwardOnXZ.z * 0.01f).Normalized();
                mainCam.Up = Vector3D::Cross(mainCam.Forward, mainCam.Right);
            }
        }
        previousM = currentM;
    }
}

// Switches buffer pointers, main buffer is now being displayed while draw buffer is being redrawn in main
void SwapBuffers(vector<SimpleTri3D>*& buffer1, vector<SimpleTri3D>*& buffer2)
{
    vector<SimpleTri3D>* temp = buffer1;
    buffer1 = buffer2;
    buffer2 = temp;
}

void initialize(RenderWindow& window)
{
    if (!font.loadFromFile("Assets/arial.ttf"))
        throw("Could not open font.");

    // Set mouse pos to middle of window
    Mouse::setPosition(Vector2i(window.getSize().x / 2, window.getSize().y / 2), window);

    createShapesContainer = RectangleShape(Vector2f(window.getSize().x * 0.4f, window.getSize().y * 0.6f));
    createShapesContainer.setPosition(window.getSize().x / 2 - (createShapesContainer.getSize().x / 2), window.getSize().y * 0.1f);
    createShapesContainer.setFillColor(Color(35, 35, 35, 0));
    UIElems.push_back(createShapesContainer);

    createCubeButton = RectangleShape(Vector2f(createShapesContainer.getSize().x * 0.3f, createShapesContainer.getSize().y * 0.3f));
    createCubeButton.setPosition(createShapesContainer.getPosition().x + (createShapesContainer.getSize().x * 0.1f), 
        createShapesContainer.getPosition().y + (createShapesContainer.getSize().y * 0.1f));
    createCubeButton.setFillColor(Color(25, 25, 25, 0));
    UIElems.push_back(createCubeButton);

    createTorusButton = RectangleShape(Vector2f(createShapesContainer.getSize().x * 0.3f, createShapesContainer.getSize().y * 0.3f));
    createTorusButton.setPosition(createShapesContainer.getPosition().x + (createShapesContainer.getSize().x * 0.6f), 
        createShapesContainer.getPosition().y + (createShapesContainer.getSize().y * 0.1f));
    createTorusButton.setFillColor(Color(25, 25, 25, 0));
    UIElems.push_back(createTorusButton);

    mainFPSCounter.setFont(font);
    mainFPSCounter.setCharacterSize(12);
    mainFPSCounter.setFillColor(Color::White);
    mainFPSCounter.setPosition(window.getSize().x * .85f, window.getSize().y * .02f);

    drawFPSCounter.setFont(font);
    drawFPSCounter.setCharacterSize(12);
    drawFPSCounter.setFillColor(Color::White);
    drawFPSCounter.setPosition(window.getSize().x * .85f, window.getSize().y * .05f);

    updateFPSCounter.setFont(font);
    updateFPSCounter.setCharacterSize(12);
    updateFPSCounter.setFillColor(Color::White);
    updateFPSCounter.setPosition(window.getSize().x * .85f, window.getSize().y * .08f);

    theta = 0;
    phi = 0;
    camSpeed = 15;

    currentM = Mouse::getPosition();
    previousM = Mouse::getPosition();

    nearClip = 0.1f; // Near Camera Clipping Plane Z
    farClip = 1000.0f; // Far Camera Clipping Plane Z (View distance)
    fov = 90.0f; // Field of View Angle (Deg)

    lightDir = Vector3D(0, -1, -1).Normalized();

    sword1 = new Mesh("Assets/SwordMesh.obj", Vector3D(0, -10, 10));
    sword2 = new Mesh("Assets/SwordMesh.obj", Vector3D(8, -10, 0));
    sword3 = new Mesh("Assets/SwordMesh.obj", Vector3D(0, -10, -10));
    sword4 = new Mesh("Assets/SwordMesh.obj", Vector3D(-10, -10, 0));
    ringSmall = new Torus(2, 0.3f, 10, 10, Vector3D(0, -10, 0));
    ringMed = new Torus(4, 0.3f, 15, 10, Vector3D(0, -10, 0));
    ringLarge = new Torus(6, 0.3f, 21, 10, Vector3D(0, -10, 0));
    terrain = new Terrain(Vector3D(0, 0, 0), 25, 25, 2);

    // Set random terrain (this could really use some perlin noise)
    srand(time(NULL));
    for (Vector3D& v : terrain->GetVertices())
        v.y -= (float)(rand() % 350) / 100.0f;

    meshes.push_back(sword1);
    meshes.push_back(sword2);
    meshes.push_back(sword3);
    meshes.push_back(sword4);
    meshes.push_back(ringSmall);
    meshes.push_back(ringMed);
    meshes.push_back(ringLarge);

    ringLarge->Rotate(PI / 2.0f, 0, 0, false);
}


int main()
{
    RenderWindow window(VideoMode::getFullscreenModes()[1], "My Graphics Engine");
    window.setVerticalSyncEnabled(true);

    // Deactivate OpenGL context for multithreaded drawing
    window.setActive(false);

    Clock time;
    float dt;
    float update = 0;

    // Initialize variables
    initialize(window);
    
    // Launch Threads
    thread updatethread(&updateThread, &window); // Updating the world. Next fram calculations are done in the main thread.
    thread drawthread(&drawThread, &window); // displaying completed frame
    thread controlsthread(&ControlsThread, &window); // For tracking user input
    
    // Multithreaded rendering
    thread project1(ProjectTris, &window, ref(subWorldBuffer1), ref(subBuffer1), &t1done);
    thread project2(ProjectTris, &window, ref(subWorldBuffer2), ref(subBuffer2), &t2done);
    thread project3(ProjectTris, &window, ref(subWorldBuffer3), ref(subBuffer3), &t3done);
    thread project4(ProjectTris, &window, ref(subWorldBuffer4), ref(subBuffer4), &t4done);

    //              ----     MAIN THREAD UPDATE    ----

    while (window.isOpen())
    {
        dt = time.restart().asSeconds();
        update += dt;
        if (update >= TickTime)
        {
            mainFPSCounter.setString("Main Thread FPS:  " + to_string((int)(1 / dt)));
            update = update - TickTime;
        }

        // Clear the buffers for next frame
        worldTriBuffer.clear();
        mainPtr->clear();

        // Fill the buffer containing every triangle in the world
        for (Mesh* m : meshes)
            m->FillBuffer(worldTriBuffer);
        terrain->FillBuffer(worldTriBuffer);

        subWorldBuffer1.clear();
        subWorldBuffer2.clear();
        subWorldBuffer3.clear();
        subWorldBuffer4.clear();
        subBuffer1.clear();
        subBuffer2.clear();
        subBuffer3.clear();
        subBuffer4.clear();
        
        subWorldBuffer1.insert(subWorldBuffer1.end(), worldTriBuffer.begin(), worldTriBuffer.begin() + (worldTriBuffer.size() / 4));
        subWorldBuffer2.insert(subWorldBuffer2.end(), worldTriBuffer.begin() + (worldTriBuffer.size() / 4), worldTriBuffer.begin() + (2 * worldTriBuffer.size() / 4));
        subWorldBuffer3.insert(subWorldBuffer3.end(), worldTriBuffer.begin() + (2 * worldTriBuffer.size() / 4), worldTriBuffer.begin() + (3 * worldTriBuffer.size() / 4));
        subWorldBuffer4.insert(subWorldBuffer4.end(), worldTriBuffer.begin() + (3 * worldTriBuffer.size() / 4), worldTriBuffer.end());
        t1done = false;
        t2done = false;
        t3done = false;
        t4done = false;

        while (!(t1done && t2done && t3done && t4done)) cout << ""; // empty loop that waits for threads to finish calculations

        mainPtr->insert(mainPtr->end(), subBuffer1.begin(), subBuffer1.end());
        mainPtr->insert(mainPtr->end(), subBuffer2.begin(), subBuffer2.end());
        mainPtr->insert(mainPtr->end(), subBuffer3.begin(), subBuffer3.end());
        mainPtr->insert(mainPtr->end(), subBuffer4.begin(), subBuffer4.end());

        // Sort all triangles on the screen before drawing
        sort(mainPtr->begin(), mainPtr->end(), [](SimpleTri3D& t1, SimpleTri3D& t2)
            {
                float t1Z = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
                float t2Z = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
                return t1Z > t2Z;
            });
        SwapBuffers(mainPtr, drawPtr); // Pointer swap of buffers

        grid.UpdateDisplay(nearClip, farClip, fov, mainCam, &window);
       
        // check all the window's events that were triggered since the last iteration of the loop
        Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape))
            {
                closeWindow = true;

                drawthread.join();
                updatethread.join();
                controlsthread.join();

                project1.join();
                project2.join();
                project3.join();
                project4.join();

                window.close();
            }
        }
    }
    // Clean up
    for (Mesh* m : meshes)
        delete m;
    delete terrain;
    //_CrtDumpMemoryLeaks();
   
    return 0;
}