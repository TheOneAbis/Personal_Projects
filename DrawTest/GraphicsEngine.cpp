#include "Mesh.h"
#include "Matrices.h"
#include <SFML\Window\Mouse.hpp>

#define DarkGrey Color(45, 45, 45)


float theta, phi, nearClip, farClip, fov;

Camera* mainCam;
Vector2i currentM, previousM, mouseVel;

Vector3D lightDir;
Mesh* sword = new Mesh("Assets/SwordMesh.obj", Vector3D(0, 0, 4));

void drawThread(RenderWindow* window)
{
    // Activate window's context
    window->setActive(true);

    while (window->isOpen()) 
    {
        window->clear(DarkGrey);

        // draw in here
        sword->Draw(nearClip, farClip, fov, *mainCam, lightDir, window);
        //cout << mainCam->Right.x << " " << mainCam->Right.y << " " << mainCam->Right.z << endl;

        // End current frame
        window->display();
    }
}

void initialize()
{
    theta = 0;
    phi = 0;

    currentM = Mouse::getPosition();
    previousM = Mouse::getPosition();

    nearClip = 0.1; // Near Camera Clipping Plane Z
    farClip = 1000.0; // Far Camera Clipping Plane Z (View distance)
    fov = 90.0; // Field of View Angle (Deg)
    mainCam = new Camera(Vector3D(0, 0, 0), Vector3D(0, 0, 1));
    lightDir = (sword->GetPosition() - mainCam->position).Normalized();
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "Drawing3D");
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
        Vector3D forwardMovt = mainCam->Forward * (dt * 2);
        forwardMovt.y *= -1;
        Vector3D rightMovt = mainCam->Right * (dt * 2);
        Vector3D upMovt = Mesh::GlobalUp() * (dt * 2);

        cout << mainCam->position.y << endl;

        if (Keyboard::isKeyPressed(sf::Keyboard::W))
            mainCam->position += forwardMovt;
        if (Keyboard::isKeyPressed(sf::Keyboard::A))
            mainCam->position -= rightMovt;
        if (Keyboard::isKeyPressed(sf::Keyboard::S))
            mainCam->position -= forwardMovt;
        if (Keyboard::isKeyPressed(sf::Keyboard::D))
            mainCam->position += rightMovt;
        if (Keyboard::isKeyPressed(sf::Keyboard::Space))
            mainCam->position -= upMovt;
        if (Keyboard::isKeyPressed(sf::Keyboard::LControl))
            mainCam->position += upMovt;

        mainCam->RotateX(-dt * mouseVel.y);
        mainCam->RotateY(-dt * mouseVel.x);

        // Set camera's theta and elevation angles
        mainCam->WorldPhi = mainCam->Forward.y > 0 ? acos(mainCam->Forward.Dot(Vector3D(mainCam->Forward.x, 0, mainCam->Forward.z).Normalized())) :
            -acos(mainCam->Forward.Dot(Vector3D(mainCam->Forward.x, 0, mainCam->Forward.z).Normalized()));

        mainCam->WorldTheta = mainCam->Forward.x > 0 ? acos(Vector3D(mainCam->Forward.x, 0, mainCam->Forward.z).Normalized().Dot(Mesh::GlobalForward())) :
            -acos(Vector3D(mainCam->Forward.x, 0, mainCam->Forward.z).Normalized().Dot(Mesh::GlobalForward()));

        sword->Rotate(0, dt, 0, false);
        sword->Move(dt, 0, 0);
        lightDir = (mainCam->position - sword->GetPosition()).Normalized();

        // check all the window's events that were triggered since the last iteration of the loop
        Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape))
                window.close();
        }

        window.clear(DarkGrey);
        // draw in here
        sword->Draw(nearClip, farClip, fov, *mainCam, lightDir, &window);
        // End current frame
        window.display();

        previousM = currentM;
    }
    return 0;
}
