// Multiply a Vector3 by a 4 X 4 Matrix, returns the new altered Vector3
function MultiplyMatrixVector(originalVector, matrix4x4) {
    let alteredVector = new Vector3();
    alteredVector.x = originalVector.x * matrix4x4.m[0][0] + originalVector.y * matrix4x4.m[1][0] + originalVector.z * matrix4x4.m[2][0] + matrix4x4.m[3][0];
    alteredVector.y = originalVector.x * matrix4x4.m[0][1] + originalVector.y * matrix4x4.m[1][1] + originalVector.z * matrix4x4.m[2][1] + matrix4x4.m[3][1];
    alteredVector.z = originalVector.x * matrix4x4.m[0][2] + originalVector.y * matrix4x4.m[1][2] + originalVector.z * matrix4x4.m[2][2] + matrix4x4.m[3][2];
    let w = originalVector.x * matrix4x4.m[0][3] + originalVector.y * matrix4x4.m[1][3] + originalVector.z * matrix4x4.m[2][3] + matrix4x4.m[3][3];

    // This last value is vital to creating the 3D illusion. 
    // As objects move away from the user, their x and y values appear to change less. Dividing each component by w makes this happen.
    if (w != 0.0) {
        alteredVector.x /= w;
        alteredVector.y /= w;
        alteredVector.z /= w;
    }
    return alteredVector;
}

// Calculate Cross Product of 2 3D vectors
function Cross(v1, v2) {
    return new Vector3((v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x));
}

// Add v1 + v2
function Add(v1, v2) {
    return new Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

// Subtract v1 - v2
function Subtract(v1, v2) {
    return new Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

// Calculate the Dot Product of 2 vectors
function Dot(v1, v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Convert a Hexadecimal string to a Decimal Number (for changing colors)
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

// Because structs don't exist in Javascript (I hate this language)
function Copy(vOriginal) {
    return new Vector3(vOriginal.x, vOriginal.y, vOriginal.z);
}
