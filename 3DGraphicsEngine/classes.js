class Vector3 {
    // Creates a new vector with an x, y, and z component
    constructor(x = 0, y = 0, z = 0) {
        this.x = x;
        this.y = y;
        this.z = z;
    }
    // Quick internal function that updates the vector's length whenever it's required
    Length() {
        return Math.sqrt(this.x * this.x + this.y * this.y + this.z * this.z);
    }

    // Set the given vector's length to 1
    static Normalize(v) {
        let len = v.Length();
        if (len != 0) {
            v.x /= len;
            v.y /= len;
            v.z /= len;
        }
        return v;
    }
    // Return a new version of this vector with a length of 1
    Normalized() {
        let len = this.Length();
        return len == 0 ? new Vector3() : 
        new Vector3(this.x / len, this.y / len, this.z / len);
    }

    // Add v1 + v2
    static Add(v1, v2) {
        return new Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    }
    // Subtract v1 - v2
    static Subtract(v1, v2) {
        return new Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    }
    static Multiply(v, amt) {
        return new Vector3(v.x * amt, v.y * amt, v.z * amt);
    }
    static Divide(v, amt) {
        return new Vector3(v.x / amt, v.y / amt, v.z / amt);
    }

    // Calculate Cross Product of 2 3D vectors
    static Cross(v1, v2) {
        return new Vector3((v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x));
    }
    // Calculate the Dot Product of 2 vectors
    static Dot(v1, v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    // Because structs don't exist in Javascript (I hate this language)
    static Copy(vOriginal) {
        return new Vector3(vOriginal.x, vOriginal.y, vOriginal.z);
    }

    static GlobalUp 

    // For debugging purposes
    Log() {
        console.log(`${this.x}, ${this.y}, ${this.z}`);
    }
}


class Mat3x3 {
    // Create a 3x3 Matrix for world-space vector manipulation
    constructor(elements) {
        this.m = elements;
    }

    // Identity Matrix
    static Identity = new Mat3x3(
        [
            [1, 0, 0],
            [0, 1, 0],
            [0, 0, 1],
        ]
    );
    // Empty Matrix
    static Empty = new Mat3x3(
        [
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
        ]
    );
    // Rotation X Matrix
    static RotationX(theta) {
        return new Mat3x3(
            [
                [1, 0, 0],
                [0, Math.cos(theta), -Math.sin(theta)],
                [0, Math.sin(theta), Math.cos(theta)]
            ]
        );
    }
    // Rotation Y Matrix
    static RotationY(theta) {
        return new Mat3x3(
            [
                [Math.cos(theta), 0, Math.sin(theta)],
                [0, 1, 0],
                [-Math.sin(theta), 0, Math.cos(theta)],
            ]
        );
    }
    // Rotation Z Matrix
    static RotationZ(theta) {
        return new Mat3x3(
            [
                [Math.cos(theta), -Math.sin(theta), 0],
                [Math.sin(theta), Math.cos(theta), 0],
                [0, 0, 1],
            ]
        );
    }
    // Rotation about any vector
    static Rotation(axis, theta) {
        let r = axis.Normalized();
        let c = Math.cos(theta), t = 1 - c,  s = Math.sin(theta);
        return new Mat3x3(
            [
                [t * r.x * r.x + c,       t * r.y * r.x - s * r.z, t * r.z * r.x + s * r.y],
                [t * r.x * r.y + s * r.z, t * r.y * r.y + c,       t * r.z * r.y - s * r.x],
                [t * r.x * r.z - s * r.y, t * r.y * r.z + s * r.x, t * r.z * r.z + c      ],
            ]
        )
    }
    // Multiply a Vector3 by a 3 X 3 Matrix, returns the new altered Vector3
    static MultiplyMatrixVector(originalVector, matrix3x3) {
        let alteredVector = new Vector3();
        alteredVector.x = originalVector.x * matrix3x3.m[0][0] + originalVector.y * matrix3x3.m[1][0] + originalVector.z * matrix3x3.m[2][0];
        alteredVector.y = originalVector.x * matrix3x3.m[0][1] + originalVector.y * matrix3x3.m[1][1] + originalVector.z * matrix3x3.m[2][1];
        alteredVector.z = originalVector.x * matrix3x3.m[0][2] + originalVector.y * matrix3x3.m[1][2] + originalVector.z * matrix3x3.m[2][2];
        return alteredVector;
    }
    // Multiply a 2 3x3 matrices, returns the new matrix
    static MultiplyMatrixMatrix(mat1, mat2) {
        return new Mat3x3(
            [
                [mat1.m[0][0] * mat2.m[0][0] + mat1.m[0][1] * mat2.m[1][0] + mat1.m[0][2] * mat2.m[2][0], 
                    mat1.m[0][0] * mat2.m[0][1] + mat1.m[0][1] * mat2.m[1][1] + mat1.m[0][2] * mat2.m[2][1], 
                    mat1.m[0][0] * mat2.m[0][2] + mat1.m[0][1] * mat2.m[1][2] + mat1.m[0][2] * mat2.m[2][2]],
                [mat1.m[1][0] * mat2.m[0][0] + mat1.m[1][1] * mat2.m[1][0] + mat1.m[1][2] * mat2.m[2][0], 
                    mat1.m[1][0] * mat2.m[0][1] + mat1.m[1][1] * mat2.m[1][1] + mat1.m[1][2] * mat2.m[2][1], 
                    mat1.m[1][0] * mat2.m[0][2] + mat1.m[1][1] * mat2.m[1][2] + mat1.m[1][2] * mat2.m[2][2]],
                [mat1.m[2][0] * mat2.m[0][0] + mat1.m[2][1] * mat2.m[1][0] + mat1.m[2][2] * mat2.m[2][0], 
                    mat1.m[2][0] * mat2.m[0][1] + mat1.m[2][1] * mat2.m[1][1] + mat1.m[2][2] * mat2.m[2][1], 
                    mat1.m[2][0] * mat2.m[0][2] + mat1.m[2][1] * mat2.m[1][2] + mat1.m[2][2] * mat2.m[2][2]]
            ]
        );
    }
}


class Mat4x4 {
    // Create a 4x4 Matrix for projection
    constructor(elements) {
        this.m = elements;
    }
    // Projection Matrix
    static Projection(near, far, fov, aspectRatio) {
        let fovRad = 1 / Math.tan(fov * 0.5 / 180 * Math.PI); // FOV converted to Radians for matrix multiplication
        return new Mat4x4(
            [
                [aspectRatio * fovRad, 0,                0,              0],
                [0,                 fovRad,              0,              0],
                [0,                    0,        far / (far - near),     1],
                [0,                    0,  (-far * near) / (far - near), 0],
            ]
        )
    }
    // Multiply a Vector3 by a 4 X 4 Matrix, returns the new altered Vector3
    static MultiplyMatrixVector(originalVector, matrix4x4) {
        let alteredVector = new Vector3();
        alteredVector.x = originalVector.x * matrix4x4.m[0][0] + originalVector.y * matrix4x4.m[1][0] + originalVector.z * matrix4x4.m[2][0] + matrix4x4.m[3][0];
        alteredVector.y = originalVector.x * matrix4x4.m[0][1] + originalVector.y * matrix4x4.m[1][1] + originalVector.z * matrix4x4.m[2][1] + matrix4x4.m[3][1];
        alteredVector.z = originalVector.x * matrix4x4.m[0][2] + originalVector.y * matrix4x4.m[1][2] + originalVector.z * matrix4x4.m[2][2] + matrix4x4.m[3][2];
        let w = originalVector.x * matrix4x4.m[0][3] + originalVector.y * matrix4x4.m[1][3] + originalVector.z * matrix4x4.m[2][3] + matrix4x4.m[3][3];

        // As objects move away from the user, their x and y values appear to change less. Dividing each component by w makes this happen.
        if (w != 0.0) {
            alteredVector.x /= w;
            alteredVector.y /= w;
            alteredVector.z /= w;
        }
        return alteredVector;
    }
}


class Triangle3D extends PIXI.Graphics {
    // Creates a new Triangle with 3 points p1, p2, and p3, defined in a CLOCKWISE fashion
    constructor(p1 = new Vector3(), p2 = new Vector3(), p3 = new Vector3()) {
        super();
        this.Points = [p1, p2, p3];
        this.Projected = {
            Points: [new Vector3(), new Vector3(), new Vector3()],
            CenterZ: (p1.z + p2.z + p3.z) / 3
        };
        this.Normal = Vector3.Normalize(Vector3.Cross(Vector3.Subtract(this.Points[1], this.Points[0]), Vector3.Subtract(this.Points[2], this.Points[0])));
        this.x = p1.x;
        this.y = p1.y;
        this.z = p1.z;
    }

    // Draw the triangle projected onto 2D screen
    Draw(shadeValue, r, g, b) {
        this.x = this.Projected.Points[0].x;
        this.y = this.Projected.Points[0].y;
        this.z = this.Projected.Points[0].z;

        let red = Math.max(parseInt(r % 256 * shadeValue), 0), 
        green = Math.max(parseInt(g % 256 * shadeValue), 0), 
        blue = Math.max(parseInt(b % 256 * shadeValue), 0);
        red = red < 16 ? `0${red.toString(16)}` : red.toString(16);
        green = green < 16 ? `0${green.toString(16)}` : green.toString(16);
        blue = blue < 16 ? `0${blue.toString(16)}` : blue.toString(16);

        // Converts Color attributes to hex string, then converts it to decimal, used for fill color param
        let triColorDecimal = HexToDec((red + green + blue).toUpperCase());
        this.beginFill(triColorDecimal, 1); 
        this.lineStyle(1, triColorDecimal, 0);
        this.moveTo(0, 0);
        this.lineTo(this.Projected.Points[1].x - this.Projected.Points[0].x, this.Projected.Points[1].y - this.Projected.Points[0].y);
        this.lineTo(this.Projected.Points[2].x - this.Projected.Points[0].x, this.Projected.Points[2].y - this.Projected.Points[0].y);
        this.lineTo(0, 0);
        this.endFill();
    }
}


class Mesh {
    // Creates a new mesh made up of an initially undefined amount of triangles
    constructor(tris) {
        this.tris = tris;
        this.pivot = new Vector3(0, 0, 0);
        this.offset = new Vector3(0, 0, 4);
        
        this.Right = new Vector3(1, 0, 0);
        this.Up = new Vector3(0, 1, 0);
        this.Forward = new Vector3(0, 0, 1);

        // Offset it out so it's not on top of the camera
        for (let tri of this.tris) {
            for (let point of tri.Points) {
                point.x += this.offset.x + this.pivot.x;
                point.y += this.offset.y + this.pivot.y;
                point.z += this.offset.z + this.pivot.z;
            }
        }
    }

    static GlobalRight = new Vector3(1, 0, 0);
    static GlobalUp = new Vector3(0, 1, 0);
    static GlobalForward = new Vector3(0, 0, 1);

    // Performs a rotation around the Z axis, the X axis, then the Y axis
    Rotate(xRad, yRad, zRad, useLocal = false) {
        if (!useLocal) {
            this.Up = Mat3x3.MultiplyMatrixVector(Mat3x3.MultiplyMatrixVector(Mat3x3.MultiplyMatrixVector(
                this.Up, Mat3x3.Rotation(Mesh.GlobalForward, zRad)), Mat3x3.Rotation(Mesh.GlobalRight, xRad)), Mat3x3.Rotation(Mesh.GlobalUp, yRad));
            this.Forward = Mat3x3.MultiplyMatrixVector(Mat3x3.MultiplyMatrixVector(Mat3x3.MultiplyMatrixVector(
                this.Forward, Mat3x3.Rotation(Mesh.GlobalForward, zRad)), Mat3x3.Rotation(Mesh.GlobalRight, xRad)), Mat3x3.Rotation(Mesh.GlobalUp, yRad));
            this.Right = Vector3.Cross(this.Forward, this.Up);
        }
        
        //console.log(`${this.Up.z}`);
        for (let tri of this.tris) {
            for (let i = 0; i < tri.Points.length; i++) {

                // Bring mesh to origin point before rotating
                tri.Points[i].x -= this.offset.x;
                tri.Points[i].y -= this.offset.y;
                tri.Points[i].z -= this.offset.z; 
                
                // Rotate mesh around Z axis, then X axis, then Y axis
                tri.Points[i] = useLocal ? Mat3x3.MultiplyMatrixVector(Mat3x3.MultiplyMatrixVector(Mat3x3.MultiplyMatrixVector(
                    tri.Points[i], Mat3x3.Rotation(this.Forward, zRad)), Mat3x3.Rotation(this.Right, xRad)), Mat3x3.Rotation(this.Up, yRad)) :
                Mat3x3.MultiplyMatrixVector(Mat3x3.MultiplyMatrixVector(Mat3x3.MultiplyMatrixVector(
                    tri.Points[i], Mat3x3.Rotation(Mesh.GlobalForward, zRad)), Mat3x3.Rotation(Mesh.GlobalRight, xRad)), Mat3x3.Rotation(Mesh.GlobalUp, yRad));

                tri.Points[i].x += this.offset.x;
                tri.Points[i].y += this.offset.y;
                tri.Points[i].z += this.offset.z;
            }
        }
    }
    // Project the 3D mesh onto the screen
    Project(near, far, fov, aspectRatio) {
        let trisToDraw = [];
        for (let tri of this.tris) {
            tri.clear(); // Clear previous triangle's drawing for redraw

            tri.Normal = Vector3.Normalize(Vector3.Cross(Vector3.Subtract(tri.Points[1], tri.Points[0]), Vector3.Subtract(tri.Points[2], tri.Points[0])));

            // Only project & draw visible triangles (if angle between normal and any point on the tri relative to the camera is <= 90deg)
            if (Vector3.Dot(tri.Normal, Vector3.Subtract(tri.Points[0], cameraPos)) < 0.0) {
                trisToDraw.push(tri);
                for (let i = 0; i < tri.Points.length; i++) {
                    // Project triangles to 2D screen using Projection Matrix
                    tri.Projected.Points[i] = Mat4x4.MultiplyMatrixVector(tri.Points[i], Mat4x4.Projection(near, far, fov, aspectRatio));
                    
                    // Move projected mesh to middle of screen
                    tri.Projected.Points[i].x += 1.0;
                    tri.Projected.Points[i].y += 1.0;
    
                    // Scale mesh from 1x1x1 for visibility
                    tri.Projected.Points[i].x *= 0.5 * sceneWidth;
                    tri.Projected.Points[i].y *= 0.5 * sceneHeight;
                }
                tri.Projected.CenterZ = (tri.Projected.Points[0].z + tri.Projected.Points[1].z + tri.Projected.Points[2].z) / 3;

                
            }
            /*
            if (this.tris.indexOf(tri, 0) == 5) {
                console.log(`${tri.p1.x}, ${tri.p1.y}, ${tri.p1.z}`);
                console.log(`${tri.p2.x}, ${tri.p2.y}, ${tri.p2.z}`);
                console.log(`${tri.p3.x}, ${tri.p3.y}, ${tri.p3.z}`);
                console.log(""); }
            */
        }
        // Sort draw order, draw tris farthest to nearest
        meshContainer.children.sort((tri1, tri2) => (tri2.Projected.CenterZ - tri1.Projected.CenterZ));
        trisToDraw.forEach(tri => tri.Draw(Vector3.Dot(tri.Normal, lightDir), 255, 255, 255));
    }
}
