//NOTE: SFML put y=0 at the top, so when y increments everything goes down
// This is a WIP build, so expect bugs

#include "Mesh.h"
#include "Torus.h"
#include "Matrices.h"
#include "GridXZ.h"

#define SFML_STATIC
#include <SFML\Window.hpp>
#include <SFML\Graphics.hpp>
#include <thread>

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

vector<SimpleTri3D> screenTriBuffer;
vector<SimpleTri3D> subBuffer1;
vector<SimpleTri3D> subBuffer2;
vector<SimpleTri3D> subBuffer3;
vector<SimpleTri3D> subBuffer4;
vector<SimpleTri3D> tempScreenTriBuffer;

bool t1done = true, t2done = true, t3done = true, t4done = true;

// light direction vector
Vector3D lightDir;

bool closeWindow = false;

// meshes
Mesh sword1("Assets/SwordMesh.obj", Vector3D(0, 0, 8), &mainCam);
Mesh sword2("Assets/SwordMesh.obj", Vector3D(8, 0, 0), &mainCam);
Mesh sword3("Assets/SwordMesh.obj", Vector3D(0, 0, -8), &mainCam);
Mesh sword4("Assets/SwordMesh.obj", Vector3D(-8, 0, 0), &mainCam);
Torus ringSmall(2, 0.3f, 10, 10, Vector3D(0, 0, 0), &mainCam);
Torus ringMed(4, 0.3f, 15, 10, Vector3D(0, 0, 0), &mainCam);
Torus ringLarge(6, 0.3f, 21, 10, Vector3D(0, 0, 0), &mainCam);

SimpleTri3D* test;

vector<Mesh*> meshes;
GridXZ grid;

// Text font
Font font;
Text instructions;
Text mainFPSCounter;
Text drawFPSCounter;

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
                            Vertex projVector2D(Vector2f(projVector.x, projVector.y), Color(shade, 0, 0, 255));
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
            update = 0;
        }

        window->clear(DarkGrey);
        
        grid.Draw(window);

        for (Mesh* m : meshes)
            m->UpdateViewSpace();

        // Draw the triangles
        for (size_t i = 0; i < tempScreenTriBuffer.size(); i++)
            window->draw(tempScreenTriBuffer[i].GetProjected());

        window->draw(instructions);
        window->draw(mainFPSCounter);
        window->draw(drawFPSCounter);
        window->display();
    }
}

void initialize(RenderWindow& window)
{
    if (!font.loadFromFile("Assets/arial.ttf"))
        throw("Could not open font.");

    //Display how to Interact w/ application
    instructions.setFont(font);
    instructions.setString("WASD keys to move around \nSHIFT to move faster \nSPACE to go UP, LCtrl to go DOWN \nMove mouse to look around");
    instructions.setCharacterSize(20);
    instructions.setFillColor(Color::White);
    instructions.setPosition(0, 0);

    mainFPSCounter.setFont(font);
    mainFPSCounter.setCharacterSize(12);
    mainFPSCounter.setFillColor(Color::White);
    mainFPSCounter.setPosition(window.getSize().x * .8f, window.getSize().y * .02f);

    drawFPSCounter.setFont(font);
    drawFPSCounter.setCharacterSize(12);
    drawFPSCounter.setFillColor(Color::White);
    drawFPSCounter.setPosition(window.getSize().x * .8f, window.getSize().y * .05f);

    theta = 0;
    phi = 0;
    camSpeed = 15;

    currentM = Mouse::getPosition();
    previousM = Mouse::getPosition();

    nearClip = 0.1f; // Near Camera Clipping Plane Z
    farClip = 1000.0f; // Far Camera Clipping Plane Z (View distance)
    fov = 90.0f; // Field of View Angle (Deg)

    lightDir = Vector3D(0, -1, -1).Normalized();

    for (int i = -50; i < 50; i++)
        VertexArray gridLine(Lines, 2);

    meshes.push_back(&sword1);
    meshes.push_back(&sword2);
    meshes.push_back(&sword3);
    meshes.push_back(&sword4);
    meshes.push_back(&ringSmall);
    meshes.push_back(&ringMed);
    meshes.push_back(&ringLarge);

    ringLarge.Rotate(PI / 2.0f, 0, 0, false);
}


int main()
{
    RenderWindow window(VideoMode(1000, 563), "My Graphics Engine");
    window.setVerticalSyncEnabled(true);

    // Deactivate OpenGL context for multithreaded drawing
    window.setActive(false);

    Mouse::setPosition(Vector2i(window.getSize().x / 2, window.getSize().y / 2), window);

    Clock time;
    float dt;
    float update = 0;

    // Initialize variables
    initialize(window);

    // Launch Threads
    thread drawthread(&drawThread, &window);

    thread project1(ProjectTris, &window, ref(subWorldBuffer1), ref(subBuffer1), &t1done);
    thread project2(ProjectTris, &window, ref(subWorldBuffer2), ref(subBuffer2), &t2done);
    //thread project3(ProjectTris, &window, ref(subWorldBuffer3), ref(subBuffer3), &t3done);
    //thread project4(ProjectTris, &window, ref(subWorldBuffer4), ref(subBuffer4), &t4done);
    
    Vector3D forwardMovt, rightMovt, upMovt;

    //              ----     MAIN THREAD UPDATE    ----

    while (window.isOpen())
    {
        dt = time.restart().asSeconds();

        update += dt;
        if (update >= TickTime)
        {
            mainFPSCounter.setString("Main Thread FPS:  " + to_string((int)(1 / dt)));
            update = 0;
        }

        currentM = Mouse::getPosition();
        mouseVel = currentM - previousM;

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
        else camSpeed = 5;
        
        // Mouse Look Input
        mainCam.RotateX((float)mouseVel.y / 100);
        mainCam.RotateY((float)-mouseVel.x / 100);

        // Keep look direction from going upside down
        if (mainCam.Forward * mainCam.forwardOnXZ <= 0)
        {
            mainCam.Forward = Vector3D(mainCam.forwardOnXZ.x * 0.01f, mainCam.Forward.y / abs(mainCam.Forward.y), mainCam.forwardOnXZ.z * 0.01f).Normalized();
            mainCam.Up = Vector3D::Cross(mainCam.Forward, mainCam.Right);
        }
        
        sword1.Rotate(0, dt, 0, false);
        sword2.Rotate(0, dt * 2, 0, false);
        sword3.Rotate(0, dt * 3, 0, false);
        sword4.Rotate(0, dt * 4, 0, false);

        ringSmall.Rotate(dt, 0, 0, false);
        ringSmall.Rotate(0, -dt * 0.5f, 0, true);

        ringMed.Rotate(0, 0, dt / 2, false);
        ringMed.Rotate(0, -dt, 0, true);

        ringLarge.Rotate(0, dt / 3, 0, false);
        ringLarge.Rotate(0, -dt, 0, true);

        // Clear the buffers for next frame
        worldTriBuffer.clear();
        screenTriBuffer.clear();

        // Fill the buffer containing every triangle in the world
        for (Mesh* m : meshes)
            m->FillBuffer(worldTriBuffer);

        //t1done = false;
        //ProjectTris(&window, worldTriBuffer, screenTriBuffer, &t1done);

        subWorldBuffer1.clear();
        subWorldBuffer2.clear();
        subWorldBuffer3.clear();
        subWorldBuffer4.clear();
        subBuffer1.clear();
        subBuffer2.clear();
        subBuffer3.clear();
        subBuffer4.clear();

        subWorldBuffer1.insert(subWorldBuffer1.end(), worldTriBuffer.begin(), worldTriBuffer.begin() + (worldTriBuffer.size() / 2));
        subWorldBuffer2.insert(subWorldBuffer2.end(), worldTriBuffer.begin() + (worldTriBuffer.size() / 2), worldTriBuffer.end());
        //subWorldBuffer3.insert(subWorldBuffer3.end(), worldTriBuffer.begin() + (2 * worldTriBuffer.size() / 4), worldTriBuffer.begin() + (3 * worldTriBuffer.size() / 4));
        //subWorldBuffer4.insert(subWorldBuffer4.end(), worldTriBuffer.begin() + (3 * worldTriBuffer.size() / 4), worldTriBuffer.end());
        t1done = false;
        t2done = false;
        //t3done = false;
        //t4done = false;
       
        while (!(t1done && t2done))
        {
            continue;
        }
        screenTriBuffer.insert(screenTriBuffer.end(), subBuffer1.begin(), subBuffer1.end());
        screenTriBuffer.insert(screenTriBuffer.end(), subBuffer2.begin(), subBuffer2.end());
        //screenTriBuffer.insert(screenTriBuffer.end(), subBuffer3.begin(), subBuffer3.end());
        //screenTriBuffer.insert(screenTriBuffer.end(), subBuffer4.begin(), subBuffer4.end());
        
        // Sort all triangles on the screen before drawing
        sort(screenTriBuffer.begin(), screenTriBuffer.end(), [](SimpleTri3D& t1, SimpleTri3D& t2)
            {
                float t1Z = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
                float t2Z = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
                return t1Z > t2Z;
            });
        tempScreenTriBuffer = screenTriBuffer;
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
                project1.join();
                project2.join();
                //project3.join();
                //project4.join();
                window.close();
            }
        }

        //drawThread(&window);
        previousM = currentM;
    }
    //_CrtDumpMemoryLeaks();
   
    return 0;
}
