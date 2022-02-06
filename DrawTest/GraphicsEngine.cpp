#include "Mesh.h"
#include "Matrices.h"

#define DarkGrey Color(45, 45, 45)


float theta, nearClip, farClip, fov;
Vector3D cameraPos;
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
        sword->Draw(nearClip, farClip, fov, cameraPos, lightDir, window);

        // End current frame
        window->display();
    }
}

void initialize()
{
    theta = 0;

    nearClip = 0.1; // Near Camera Clipping Plane Z
    farClip = 1000.0; // Far Camera Clipping Plane Z (View distance)
    fov = 90.0; // Field of View Angle (Deg)

    lightDir = Vector3D(0, 0, -1).Normalized();

    cameraPos = Vector3D(0, 0, 0);
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "Drawing3D");
    window.setVerticalSyncEnabled(true);
    WINDOWINFO wInfo;

    // Deactivate OpenGL context for multithreaded drawing
    window.setActive(false);

    // Initialize variables
    initialize();

    Clock clock;
    float dt;

    // Launch Draw thread
    // NOTE: MAIN THREAD LOOPS MUCH FASTER THAN DRAW THREAD
    Thread thread(&drawThread, &window);
    thread.launch();
    sword->Rotate(0, PI/2, 0, true);

    // Main Thread Update Loop
    while (window.isOpen())
    {
        dt = clock.restart().asSeconds();

        sword->Rotate(0, dt, 0, true);
        //cout << sword->GetTris()[3].Normal().getX() << endl;

        // check all the window's events that were triggered since the last iteration of the loop
        Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == Event::Closed)
                window.close();
        }
    }
    return 0;
}
