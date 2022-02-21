//NOTE: SFML put y=0 at the top, so when y increments everything goes down

#include "Mesh.h"
#include "Matrices.h"
#include "GridXZ.h"
#include <SFML\Window\Mouse.hpp>
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
Mesh sword1("Assets/SwordMesh.obj", Vector3D(0, 0, 5), &mainCam);
Mesh sword2("Assets/SwordMesh.obj", Vector3D(5, 0, 0), &mainCam);
Mesh sword3("Assets/SwordMesh.obj", Vector3D(0, 0, -5), &mainCam);
Mesh sword4("Assets/SwordMesh.obj", Vector3D(-5, 0, 0), &mainCam);
vector<Mesh*> swords;
GridXZ grid;


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
        for (Mesh* m : swords)
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

        sort(swords.begin(), swords.end(), [](Mesh* m1, Mesh* m2)
            {
                return (m1->GetPosition() - mainCam.position).SqrLength() > (m2->GetPosition() - mainCam.position).SqrLength();
            });
        
        window->clear(DarkGrey);
        
        grid.Draw(window);
        
        for (Mesh* m : swords)
        {
            m->UpdateViewSpace();
            m->Draw(window);
        }

        window->display();
    }
}


void initialize()
{
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

    swords.push_back(&sword1);
    swords.push_back(&sword2);
    swords.push_back(&sword3);
    swords.push_back(&sword4);
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
    // NOTE: MAIN THREAD LOOPS MUCH FASTER THAN DRAW THREAD
    thread drawthread(&drawThread, &window);
    //thread viewthread(&viewThread, &window);

    sword1.Rotate(0, PI / 2.0f, 0, true);



    //              ----     MAIN THREAD UPDATE    ----

    while (window.isOpen())
    {
        dt = time.restart().asSeconds();
        cout << (int)(1 / dt) << " FPS" << endl;
        //cout << dt << endl;

        currentM = Mouse::getPosition();
        mouseVel = currentM - previousM;

        Vector3D forwardMovt = Vector3D(mainCam.Forward.x, 0, mainCam.Forward.z).Normalized() * (dt * camSpeed);
        Vector3D rightMovt = mainCam.Right * (dt * camSpeed);
        Vector3D upMovt = GlobalUp() * (dt * camSpeed);

        // Movement Inputs
        if (Keyboard::isKeyPressed(Keyboard::W))
            mainCam.position += forwardMovt;
        if (Keyboard::isKeyPressed(Keyboard::A))
            mainCam.position -= rightMovt;
        if (Keyboard::isKeyPressed(Keyboard::S))
            mainCam.position -= forwardMovt;
        if (Keyboard::isKeyPressed(Keyboard::D))
            mainCam.position += rightMovt;
        if (Keyboard::isKeyPressed(Keyboard::Space))
            mainCam.position -= upMovt;
        if (Keyboard::isKeyPressed(Keyboard::LControl))
            mainCam.position += upMovt;
        if (Keyboard::isKeyPressed(Keyboard::LShift))
            camSpeed = 25;
        else camSpeed = 15;
        
        // Mouse Look Input
        mainCam.RotateX((float)-mouseVel.y / 100);
        mainCam.RotateY((float)-mouseVel.x / 100);

        // Keep look direction from going upside down
        if (mainCam.Forward * mainCam.forwardOnXZ <= 0)
        {
            mainCam.Forward = Vector3D(mainCam.forwardOnXZ.x * 0.01f, mainCam.Forward.y / abs(mainCam.Forward.y), mainCam.forwardOnXZ.z * 0.01f).Normalized();
            mainCam.Up = Vector3D::Cross(mainCam.Forward, mainCam.Right);
        }
        
        // Set camera's theta and elevation angles
        mainCam.WorldPhi = mainCam.Forward.y > 0 ? acos(mainCam.Forward.Dot(mainCam.forwardOnXZ.Normalized())) :
            -acos(mainCam.Forward.Dot(mainCam.forwardOnXZ.Normalized()));

        mainCam.WorldTheta = mainCam.Forward.x > 0 ? acos(mainCam.forwardOnXZ.Normalized().Dot(GlobalForward())) :
            -acos(mainCam.forwardOnXZ.Normalized().Dot(GlobalForward()));

        sword1.Rotate(0, dt, 0, false);
        sword2.Rotate(0, dt * 2, 0, false);
        sword3.Rotate(0, dt * 3, 0, false);
        sword4.Rotate(0, dt * 4, 0, false);

        // Update the location of all tris on the screen for drawing on other threa
        //sword3.UpdateDisplay(nearClip, farClip, fov, lightDir, &window);
        for (Mesh* m : swords)
            m->UpdateDisplay(nearClip, farClip, fov, lightDir, &window);
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
        //viewThread(&window);
        previousM = currentM;
    }

    _CrtDumpMemoryLeaks();
    return 0;
}
