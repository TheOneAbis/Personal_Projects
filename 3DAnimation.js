"use strict"

// Classes

// Creates a new vector with an x, y, and z component
class Vector3 {
    constructor(x = 0, y = 0, z = 0) {
        this.x = x;
        this.y = y;
        this.z = z;

        this.Length = Math.sqrt(x * x + y * y + z * z);
    }

    // Normalize this vector3
    Normalize() {
        if (this.Length != 0) {
            return new Vector3(this.x / this.Length, this.y / this.Length, this.z / this.Length);
        }
    }
    Log() {
        console.log(`${this.x}, ${this.y}, ${this.z}`);
    }
}

// Create a 4x4 Matrix for projection, rotation, etc
class Mat4x4 {
    constructor() {
        this.m = [
            [ 0, 0, 0, 0 ],
            [ 0, 0, 0, 0 ],
            [ 0, 0, 0, 0 ],
            [ 0, 0, 0, 0 ],
        ];
    }
}

// Creates a new Triangle with 3 points p1, p2, and p3, defined in a CLOCKWISE fashion
class Triangle3D extends PIXI.Graphics {
    constructor(p1 = new Vector3(0, 0, 0), p2 = new Vector3(0, 0, 0), p3 = new Vector3(0, 0, 0)) {
        super();
        this.Points = [p1, p2, p3];
        this.Center = new Vector3((p1.x + p2.x + p3.x) / 3, (p1.y + p2.y + p3.y) / 3, (p1.z + p2.z + p3.z) / 3);
        this.Projected = {
            Points: [new Vector3(), new Vector3(), new Vector3()],
            CenterZ: this.Center.z
        };
        this.Normal = Cross(Subtract(this.Points[1], this.Points[0]), Subtract(this.Points[2], this.Points[0])).Normalize();
        this.x = p1.x;
        this.y = p1.y;
        this.z = p1.z;
    }

    Draw(shadeValue) {
        this.x = this.Projected.Points[0].x;
        this.y = this.Projected.Points[0].y;
        this.z = this.Projected.Points[0].z;
        // Draw the triangle projected onto 2D screen
        shadeValue = shadeValue < 16 ? `0${shadeValue.toString(16)}` : shadeValue.toString(16);
        // Converts Color attributes to hex string, then converts it to decimal, used for fill color param
        let triColorDecimal = HexToDec((shadeValue + shadeValue + shadeValue).toUpperCase());
        this.beginFill(triColorDecimal, 1); 
        this.lineStyle(1, triColorDecimal, 1);
        this.moveTo(0, 0);
        this.lineTo(this.Projected.Points[1].x - this.Projected.Points[0].x, this.Projected.Points[1].y - this.Projected.Points[0].y);
        this.lineTo(this.Projected.Points[2].x - this.Projected.Points[0].x, this.Projected.Points[2].y - this.Projected.Points[0].y);
        this.lineTo(0, 0);
        this.endFill();
    }
}

// Creates a new mesh made up of an initially undefined amount of triangles
class Mesh {
    constructor(tris) {
        this.tris = tris;
        this.offsetZ = 4.0;

        // Rotation matrices
        this.matRotX = new Mat4x4();
        this.matRotZ = new Mat4x4();
        this.matRotY = new Mat4x4();

        // Offset it out so it's not on top of the camera
        for (let tri of this.tris) {
            tri.Points[0].z += this.offsetZ;
            tri.Points[1].z += this.offsetZ;
            tri.Points[2].z += this.offsetZ;
        }
    }

    // Performs a rotation around the Z axis, the X axis, then the Y axis
    Rotate(xRad, yRad, zRad) {
        this.matRotZ.m[0][0] = Math.cos(zRad);
        this.matRotZ.m[0][1] = Math.sin(zRad);
        this.matRotZ.m[1][0] = -Math.sin(zRad);
        this.matRotZ.m[1][1] = Math.cos(zRad);
        this.matRotZ.m[2][2] = 1;
        this.matRotZ.m[3][3] = 1;

        this.matRotX.m[0][0] = 1
        this.matRotX.m[1][1] = Math.cos(xRad);
        this.matRotX.m[1][2] = Math.sin(xRad);
        this.matRotX.m[2][1] = -Math.sin(xRad);
        this.matRotX.m[2][2] = Math.cos(xRad);
        this.matRotX.m[3][3] = 1;

        this.matRotY.m[0][0] = Math.cos(yRad);
        this.matRotY.m[2][0] = Math.sin(yRad);
        this.matRotY.m[0][2] = -Math.sin(yRad);
        this.matRotY.m[2][2] = Math.cos(yRad);
        this.matRotY.m[1][1] = 1;
        this.matRotY.m[3][3] = 1;

        for (let tri of this.tris) {
            for (let i = 0; i < tri.Points.length; i++) {
                tri.Points[i].z -= this.offsetZ; // Bring mesh to origin point before rotating
                tri.Points[i] = MultiplyMatrixVector(MultiplyMatrixVector(MultiplyMatrixVector( // Rotate mesh around Z axis, then X axis, then Y axis
                    tri.Points[i], this.matRotZ), this.matRotX), this.matRotY);
                tri.Points[i].z += this.offsetZ;
            }
            //tri.Center.z = (tri.Points[0].z + tri.Points[1].z + tri.Points[2].z) / 3;
        }
    }

    Project() {
        let trisToDraw = [];
        for (let tri of this.tris) {
            tri.clear(); // Clear previous triangle's drawing for redraw

            tri.Normal = Cross(Subtract(tri.Points[1], tri.Points[0]), Subtract(tri.Points[2], tri.Points[0])).Normalize();

            // Only project & draw visible triangles (if angle between normal and any point on the tri relative to the camera is <= 90deg)
            if (Dot(tri.Normal, Subtract(tri.Points[0], cameraPos)) < 0.0) {

                for (let i = 0; i < tri.Points.length; i++) {
                    // Project triangles to 2D screen using Projection Matrix
                    tri.Projected.Points[i] = MultiplyMatrixVector(tri.Points[i], projMatrix);
                    
                    // Move projected mesh to middle of screen
                    tri.Projected.Points[i].x += 1.0;
                    tri.Projected.Points[i].y += 1.0;
    
                    // Scale mesh from 1x1x1 for visibility
                    tri.Projected.Points[i].x *= 0.5 * sceneWidth;
                    tri.Projected.Points[i].y *= 0.5 * sceneHeight;
                }
                tri.Projected.CenterZ = (tri.Projected.Points[0].z + tri.Projected.Points[1].z + tri.Projected.Points[2].z) / 3;
                trisToDraw.push(tri);
            }
            /*
            if (this.tris.indexOf(tri, 0) == 5) {
                console.log(`${tri.p1.x}, ${tri.p1.y}, ${tri.p1.z}`);
                console.log(`${tri.p2.x}, ${tri.p2.y}, ${tri.p2.z}`);
                console.log(`${tri.p3.x}, ${tri.p3.y}, ${tri.p3.z}`);
                console.log(""); }
            */
        }
        trisToDraw = trisToDraw.sort((tri1, tri2) => (tri2.Projected.CenterZ - tri1.Projected.CenterZ));
        //console.log(trisToDraw);
        for (let tri of trisToDraw) {
            // Draw tri with applied shading
            tri.Draw(parseInt(Math.abs(255 * Dot(tri.Normal, lightDir))));
        }
    }
}


// ====================================================================================================================================================


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
let projMatrix = new Mat4x4();
let theta = 0;
let vel = new Vector3(0, 0, 0);
let mesh;

let near, far, fov, fovRad, aspectRatio;

// Lighting direction
let lightDir = new Vector3(0, 0, -1); 
lightDir = lightDir.Normalize();

let cameraPos = new Vector3();

let meshLoadPromise; // Promise where mesh is manipulated in seperately from rest of the program to account for file read time

animation.loader.onComplete.add(setup);
animation.loader.load();

// Called upon site initialization
function setup() {
    // Load the mesh from File
    meshLoadPromise = GenerateMeshFromFile("media/SwordMesh.obj").then(trisFromFile => {
        mesh = new Mesh(trisFromFile);
        // Add mesh triangle objects to the scene
        for (let tri of mesh.tris) {
            animation.stage.addChild(tri);
        }
    });

    meshLoadPromise.then(()=> mesh.Rotate(Math.PI / 3, 3 * Math.PI / 4, 0)); // Turn it around so blade faces up
    
    // Create the Projection Matrix
    near = 0.1; // Near Camera Clipping Plane
    far  = 1000.0; // Far Camera Clipping Plane (View distance)
    fov = 80.0; // Field of View Angle (Deg)
    aspectRatio = sceneHeight / sceneWidth; // Container aspect ratio
    fovRad = 1 / Math.tan(fov * 0.5 / 180 * Math.PI); // FOV converted to Radians for matrix multiplication

    projMatrix.m[0][0] = aspectRatio * fovRad; // multiply X by this
    projMatrix.m[1][1] = fovRad; // Multiply Y by this
    projMatrix.m[2][2] = far / (far - near); // Multiply Z by this
    projMatrix.m[3][2] = (-far * near) / (far - near); // Add this to Z
    projMatrix.m[2][3] = 1.0; // 4th component is original Z, used to divide the other 3 by
    projMatrix.m[3][3] = 0.0;

    animation.ticker.add(update);
}


// Called every frame
function update() {
    let dt = 1 / animation.ticker.FPS;
    if (dt > 1 / 12) dt = 1 / 12;

    theta = dt * 2;

    // Only alter the mesh after it's been loaded
    meshLoadPromise.then(() => {
        mesh.Rotate(0, 0, 0);
        mesh.Project();
    });
    
}


function GenerateMeshFromFile(fileLoc) {
    // Read Obj files, will be implemented soon
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
    
    //console.log(faceData);

    for (let vString of vertexData) {
        let points = vString.split(" ");
        vertices.push(new Vector3(parseFloat(points[0]), parseFloat(points[1]), parseFloat(points[2])));
    }
    for (let fString of faceData) {
        let faces = fString.split(" ");
        tris.push(new Triangle3D(
            Copy(vertices[parseInt(faces[0].split("/")[0] - 1)]), 
            Copy(vertices[parseInt(faces[1].split("/")[0] - 1)]), 
            Copy(vertices[parseInt(faces[2].split("/")[0] - 1)])));
            
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
}

// NOTE: Found the problem. Animation.stage.children needs to be sorted, not the drawing sequence.
