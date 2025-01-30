#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Input from vertex shader
in vec3 FragPos[]; // Vertex positions in world space

// Output to fragment shader
out vec3 Normal;       // Smooth normal
out vec3 FragPosition; // Vertex position in world space

uniform vec3 CameraPosition; // World-space camera position

void main() {
    // Compute the face normal for the triangle
    vec3 p0 = FragPos[0];
    vec3 p1 = FragPos[1];
    vec3 p2 = FragPos[2];
    vec3 faceNormal = normalize(cross(p1 - p0, p2 - p0));

    // Correct face normal if it's pointing away from the camera
    vec3 viewVector = normalize(CameraPosition - p0);
    if (dot(faceNormal, viewVector) < 0.0) {
        faceNormal = -faceNormal; // Flip the normal
    }

    // Emit each vertex with the same smooth normal
    for (int i = 0; i < 3; ++i) {
        // Smooth normal calculation (optional if adjacent normals are unavailable)
        Normal = faceNormal;
        FragPosition = FragPos[i];
        gl_Position = gl_in[i].gl_Position;

        EmitVertex();
    }

    EndPrimitive();
}
//#version 330 core
//layout (triangles) in;
//layout (triangle_strip, max_vertices = 3) out;
//
//// Input from vertex shader
//in vec3 FragPos[];
//
//// Output to fragment shader
//out vec3 Normal;
//out vec3 FragPosition;
//
//void main() {
//    // Calculate the normal for the triangle in world space
//    vec3 p0 = FragPos[0];
//    vec3 p1 = FragPos[1];
//    vec3 p2 = FragPos[2];
//
//    vec3 normal = normalize(cross(p1 - p0, p2 - p0));
//
//    for (int i = 0; i < 3; ++i) {
//        // Pass through the transformed positions from the vertex shader
//        FragPosition = FragPos[i];
//
//        // Pass the calculated normal
//        Normal = normal;
//
//        // Pass the position for rendering (already transformed in vertex shader)
//        gl_Position = gl_in[i].gl_Position;
//
//        EmitVertex();
//    }
//
//    EndPrimitive();
//}
