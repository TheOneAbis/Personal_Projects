//NOTE: SFML put y=0 at the top, so when y increments everything goes down
// This is a WIP build, so expect bugs

#include "Mesh.h"
#include "Torus.h"
#include "Matrices.h"
#include "GridXZ.h"
#include <SFML\Window.hpp>
#include <SFML\Graphics.hpp>
#include <thread>

//Detecting memory leaks
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define DarkGrey Color(65, 65, 65)


float theta, phi, nearClip, farClip, fov;
float camSpeed;
Camera mainCam(Vector3D(0, -3, 0), Vector3D(0, 0, 1));
Vector2i currentM, previousM, mouseVel;

Vector3D lightDir;
Mesh sword1("Assets/SwordMesh.obj", Vector3D(0, 0, 8), &mainCam);
Mesh sword2("Assets/SwordMesh.obj", Vector3D(8, 0, 0), &mainCam);
Mesh sword3("Assets/SwordMesh.obj", Vector3D(0, 0, -8), &mainCam);
Mesh sword4("Assets/SwordMesh.obj", Vector3D(-8, 0, 0), &mainCam);
Torus ringSmall(2, 0.3f, 10, 10, Vector3D(0, 0, 0), &mainCam);
Torus ringMed(4, 0.3f, 15, 10, Vector3D(0, 0, 0), &mainCam);
Torus ringLarge(6, 0.3f, 21, 10, Vector3D(0, 0, 0), &mainCam);

vector<Mesh*> meshes;
GridXZ grid;

// Text font
Font font;
Text instructions;

// X-Z Plane Grid
std::vector<VertexArray> xzGrid;

void viewThread(RenderWindow* window)
{
    Clock time;
    float dt;
    while (window->isOpen())
    {
        dt = time.restart().asSeconds();
        //cout << (int)(1/dt) << " FPS" << endl;
        for (Mesh* m : meshes)
            m->UpdateViewSpace();
    }
}

void drawThread(RenderWindow* window)
{
    window->setActive(true);

    Clock time;
    float dt;
    while (window->isOpen())
    {
        dt = time.restart().asSeconds();

        //cout << dt << " MS per Frame" << endl;
        //cout << (int)(1/dt) << " FPS" << endl;

        sort(meshes.begin(), meshes.end(), [](Mesh* m1, Mesh* m2)
            {
                return (m1->GetPosition() - mainCam.position).SqrLength() > (m2->GetPosition() - mainCam.position).SqrLength();
            });
        
        window->clear(DarkGrey);
        
        grid.Draw(window);
        
        for (Mesh* m : meshes)
        {
            //m->UpdateViewSpace();
            m->Draw(window);
        }
        window->draw(instructions);

        window->display();
    }
}


void initialize()
{
    if (!font.loadFromFile("Assets/arial.ttf"))
        throw("Could not open font.");

    //Display how to Interact w/ application
    instructions.setFont(font);
    instructions.setString("WASD keys to move around \nMove mouse to look around");
    instructions.setCharacterSize(20);
    instructions.setFillColor(Color::White);
    instructions.setPosition(0, 0);

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
}


int main()
{
    RenderWindow window(VideoMode(800, 600), "My Graphics Engine");
    window.setVerticalSyncEnabled(true);
    //WINDOWINFO wInfo;

    // Deactivate OpenGL context for multithreaded drawing
    window.setActive(false);

    Mouse::setPosition(Vector2i(window.getSize().x / 2, window.getSize().y / 2), window);

    Clock time;
    float dt;
    // Initialize variables
    initialize();

    // Launch Threads
    thread drawthread(&drawThread, &window);
    //thread viewthread(&viewThread, &window);

    ringLarge.Rotate(PI / 2.0f, 0, 0, false);

    Vector3D forwardMovt, rightMovt, upMovt;

    //              ----     MAIN THREAD UPDATE    ----

    while (window.isOpen())
    {
        dt = time.restart().asSeconds();
        //cout << (int)(1 / dt) << " FPS" << endl;
        //cout << dt << endl;

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

        // Update the location of all tris on the screen for drawing on other threa
        for (Mesh* m : meshes)
        {
            m->UpdateViewSpace();
            m->UpdateDisplay(nearClip, farClip, fov, lightDir, &window);
        }
        grid.UpdateDisplay(nearClip, farClip, fov, mainCam, &window);

        // check all the window's events that were triggered since the last iteration of the loop
        Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape))
                window.close();
        }

        //drawThread(&window);
        previousM = currentM;
    }
    Mesh sword("Assets/SwordMesh.obj", Vector3D(0, 0, 0), &mainCam);
    _CrtDumpMemoryLeaks();
    return 0;
}
