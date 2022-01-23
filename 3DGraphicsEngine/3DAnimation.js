"use strict"

// Main Animation Calculations
let width = document.querySelector("#Animation3D").clientWidth;

let animation = new PIXI.Application({
    width: width,
    height: width * (9/16)
});
document.querySelector("#Animation3D").appendChild(animation.view);

let sceneWidth = animation.view.width;
let sceneHeight = animation.view.height;

// Variables
let theta = 0;

let meshContainer, UIContainer;
let instructions;
let useLocal = false;

let mouseCurrent, mousePrev, mouseVel;
let mouseDown = false;
animation.view.onmousedown = e => mouseDown = true;
animation.view.onmouseup = e => mouseDown = false;

let near, far, fov, fovRad, aspectRatio; // For projection

// Lighting direction
let lightDir = Vector3.Normalize(new Vector3(0, 0, -1)); 

let cameraPos = new Vector3();

let mesh;
let meshLoadPromise; // Promise where mesh is manipulated in seperately from rest of the program to account for file read time

animation.loader.onComplete.add(setup);
animation.loader.load();


// Called upon site initialization
function setup() {
    mouseCurrent = new Vector3();
    mousePrev = new Vector3();

    meshContainer = new PIXI.Container();
    meshContainer.visible = true;
    animation.stage.addChild(meshContainer);

    UIContainer = new PIXI.Container();
    UIContainer.visible = true;
    animation.stage.addChild(UIContainer);

    // Load the mesh from File
    meshLoadPromise = GenerateMeshFromFile("media/SwordMesh.obj").then(trisFromFile => {
        mesh = new Mesh(trisFromFile);
        // Add mesh triangle objects to the scene
        for (let tri of mesh.tris) {
            meshContainer.addChild(tri);
        }
    });

    near = 0.1; // Near Camera Clipping Plane
    far  = 1000.0; // Far Camera Clipping Plane (View distance)
    fov = 90.0; // Field of View Angle (Deg)
    aspectRatio = sceneHeight / sceneWidth; // Container Height/Width ratio
    
    meshLoadPromise.then(()=> {
        mesh.Rotate(0, Math.PI / 2, 0);
        mesh.Project(near, far, fov, aspectRatio);
    }); // Turn it around so it faces up

    // UI Elements
    instructions = new PIXI.Text("Click + Drag to \nRotate the Sword!");
    instructions.style = new PIXI.TextStyle({
        fill: 0xFFFFFF,
        fontSize: sceneWidth * 0.04,
        fontFamily: "Verdana",
        stroke: 0xFFFFFF,
        strokeThickness: 1
    });
    instructions.x = sceneWidth * 0.05;
    instructions.y = sceneWidth * 0.1;
    UIContainer.addChild(instructions);

    /*
    let rotateSpaceButton = new PIXI.Text("Click HERE to switch to LOCAL Rotation");
    rotateSpaceButton.style = new PIXI.TextStyle({
        fill: 0xFFFFFF,
        fontSize: 36,
        fontFamily: "Verdana",
        stroke: 0xFF0000,
        strokeThickness: 4
    });
    rotateSpaceButton.x = 50;
    rotateSpaceButton.y = sceneHeight - 50;
    rotateSpaceButton.interactive = true;
    rotateSpaceButton.on("pointerdown", (e) => {
        useLocal = !useLocal;
        rotateSpaceButton.text = useLocal ? "Click HERE to switch to GLOBAL Rotation" : "Click HERE to switch to LOCAL Rotation";
    });
    UIContainer.addChild(rotateSpaceButton);
    */

    animation.ticker.add(update);
}

// Called every frame
function update() {
    mouseCurrent = animation.renderer.plugins.interaction.mouse.global;
    mouseVel = Vector3.Divide(new Vector3(mouseCurrent.x - mousePrev.x, mouseCurrent.y - mousePrev.y, 0), 100);

    let dt = 1 / animation.ticker.FPS;
    if (dt > 1 / 12) dt = 1 / 12;
    theta = dt * 2;

    // Only alter the mesh after it's been loaded
    meshLoadPromise.then(() => {
        if (mouseDown) mesh.Rotate(-mouseVel.y, mouseVel.x, 0, false);
        mesh.Rotate(0, theta, 0, true);
        mesh.Project(near, far, fov, aspectRatio);
    });
    mousePrev = Vector3.Copy(mouseCurrent);
}

// Generates a 3D mesh from an OBJ file
function GenerateMeshFromFile(fileLoc) {
    return fetch(fileLoc).then((objResult)=>{
        return objResult.text().then((objData)=>{
            return CreateMesh(objData.replaceAll("\n", ""));
        })
    });
}

// Creates a 3D mesh from the given OBJ file
function CreateMesh(fileData) {
    let vertices = [];
    let tris = [];

    let vertexData = fileData.split("v "); // First half of the file (vertices)
    vertexData.splice(0, 1);
    let faceData = vertexData[vertexData.length - 1].split("f "); // Second half of the file (faces)
    faceData[0] = faceData[0].split("vt")[0]; // Ignore the vt's
    vertexData[vertexData.length - 1] = faceData[0]; // Faces still has the last vertex data
    faceData.splice(0, 1); // Cut off the last vertex data

    for (let vString of vertexData) {
        let points = vString.split(" ");
        vertices.push(new Vector3(parseFloat(points[0]), parseFloat(points[1]), parseFloat(points[2])));
    }
    for (let fString of faceData) {
        let faces = fString.split(" ");
        tris.push(new Triangle3D(
            Vector3.Copy(vertices[parseInt(faces[0].split("/")[0] - 1)]), 
            Vector3.Copy(vertices[parseInt(faces[1].split("/")[0] - 1)]), 
            Vector3.Copy(vertices[parseInt(faces[2].split("/")[0] - 1)])));
            
        //vertices[parseInt(faces[0].split("/")[0] - 1)].Log();
        //vertices[parseInt(faces[1].split("/")[0] - 1)].Log();
        //vertices[parseInt(faces[2].split("/")[0] - 1)].Log();
        //console.log("\n");
    }
    return tris;
}

// When window is resized, recalculate view width & height and everything that had to do with it
window.onresize = e => {
    animation.view.width = document.querySelector("canvas").clientWidth;
    animation.view.height = document.querySelector("canvas").clientWidth * (9/16);
    sceneWidth = animation.view.width;
    sceneHeight = animation.view.height;
    instructions.style.fontSize = sceneWidth * 0.04;
    instructions.x = sceneWidth * 0.05;
    instructions.y = sceneWidth * 0.1;
}

// HELPER FUNCTION: Convert a Hexadecimal string to a Decimal Number (for changing colors)
function HexToDec(hexString) {
    let num = 0;
    for (let i = 0; i < hexString.length; i++) {
        if (!Number.isNaN(parseInt(hexString[i]))) {
            num += parseInt(hexString[i]) * Math.pow(16, hexString.length - 1 - i);
        }
        else {
            if (Math.abs(hexString[i].charCodeAt(0) - 55) <= 15 )
                num += (hexString[i].charCodeAt(0) - 55) * Math.pow(16, hexString.length - 1 - i);
        }
    }
    return num;
}
