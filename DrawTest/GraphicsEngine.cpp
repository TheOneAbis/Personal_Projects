//NOTE: SFML put y=0 at the top, so when y increments everything goes down

#include "Mesh.h"
#include "Matrices.h"
#include "GridXZ.h"
#include <SFML\Window\Mouse.hpp>

#define DarkGrey Color(65, 65, 65)


float theta, phi, nearClip, farClip, fov;
float camSpeed;
Camera* mainCam;
Vector2i currentM, previousM, mouseVel;

Vector3D lightDir;
Mesh* sword = new Mesh("Assets/SwordMesh.obj", Vector3D(0, 0, 5));
GridXZ* grid = new GridXZ();

// X-Z Plane Grid
std::vector<VertexArray> xzGrid;

void drawThread(RenderWindow* window)
{
    window->clear(DarkGrey);

    // draw in here
    grid->Draw(nearClip, farClip, fov, *mainCam, window);
    //cout << grid->GetLines()[0][0].x << endl;
    sword->Draw(nearClip, farClip, fov, *mainCam, lightDir, window);
    // End current frame
    window->display();
}

void initialize()
{
    theta = 0;
    phi = 0;
    camSpeed = 5;

    currentM = Mouse::getPosition();
    previousM = Mouse::getPosition();

    nearClip = 0.1; // Near Camera Clipping Plane Z
    farClip = 1000.0; // Far Camera Clipping Plane Z (View distance)
    fov = 90.0; // Field of View Angle (Deg)
    mainCam = new Camera(Vector3D(0, -3, 0), Vector3D(0, -PI/6, 1));
    lightDir = Vector3D(0, 0, -1);

    for (int i = -50; i < 50; i++)
    {
        VertexArray gridLine(Lines, 2);
    }
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "My Graphics Engine");
    window.setVerticalSyncEnabled(true);
    WINDOWINFO wInfo;

    // Deactivate OpenGL context for multithreaded drawing
    window.setActive(false);

    Mouse::setPosition(Vector2i(window.getSize().x / 2, window.getSize().y / 2), window);
    // Initialize variables
    initialize();

    Clock clock;
    float dt;

    // Launch Draw thread
    // NOTE: MAIN THREAD LOOPS MUCH FASTER THAN DRAW THREAD
    //Thread thread(&drawThread, &window);
    //thread.launch();

    sword->Rotate(0, PI/2, 0, true);

    // Main Thread Update Loop
    while (window.isOpen())
    {
        currentM = Mouse::getPosition();
        mouseVel = currentM - previousM;

        dt = clock.restart().asSeconds();

        Vector3D forwardMovt = Vector3D(mainCam->Forward.x, 0, mainCam->Forward.z).Normalized() * (dt * camSpeed);
        Vector3D rightMovt = mainCam->Right * (dt * camSpeed);
        Vector3D upMovt = GlobalUp() * (dt * camSpeed);

        //cout << mainCam->position.x << " " << mainCam->position.y << " " << mainCam->position.z << endl;

        // Movement Inputs
        if (Keyboard::isKeyPressed(Keyboard::W))
            mainCam->position += forwardMovt;
        if (Keyboard::isKeyPressed(Keyboard::A))
            mainCam->position -= rightMovt;
        if (Keyboard::isKeyPressed(Keyboard::S))
            mainCam->position -= forwardMovt;
        if (Keyboard::isKeyPressed(Keyboard::D))
            mainCam->position += rightMovt;
        if (Keyboard::isKeyPressed(Keyboard::Space))
            mainCam->position -= upMovt;
        if (Keyboard::isKeyPressed(Keyboard::LControl))
            mainCam->position += upMovt;
        if (Keyboard::isKeyPressed(Keyboard::LShift))
            camSpeed = 15;
        else camSpeed = 5;

        // Mouse Look Input
        mainCam->RotateX(-dt * mouseVel.y);
        mainCam->RotateY(-dt * mouseVel.x);

        // Set camera's theta and elevation angles
        mainCam->WorldPhi = mainCam->Forward.y > 0 ? acos(mainCam->Forward.Dot(Vector3D(mainCam->Forward.x, 0, mainCam->Forward.z).Normalized())) :
            -acos(mainCam->Forward.Dot(Vector3D(mainCam->Forward.x, 0, mainCam->Forward.z).Normalized()));

        mainCam->WorldTheta = mainCam->Forward.x > 0 ? acos(Vector3D(mainCam->Forward.x, 0, mainCam->Forward.z).Normalized().Dot(GlobalForward())) :
            -acos(Vector3D(mainCam->Forward.x, 0, mainCam->Forward.z).Normalized().Dot(GlobalForward()));

        sword->Rotate(0, dt, 0, false);

        // check all the window's events that were triggered since the last iteration of the loop
        Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape))
                window.close();
        }

        // DRAW
        drawThread(&window);

        previousM = currentM;
    }
    return 0;
}
