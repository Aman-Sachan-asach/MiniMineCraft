#version 150
// ^ Change this to version 130 if you have compatibility issues

//This is a vertex shader. While it is called a "shader" due to outdated conventions, this file
//is used to apply matrix transformations to the arrays of vertex data passed to it.
//Since this code is run on your GPU, each vertex is transformed simultaneously.
//If it were run on your CPU, each vertex would have to be processed in a FOR loop, one at a time.
//This simultaneous transformation allows your program to run much faster, especially when rendering
//geometry with millions of vertices.

uniform mat4 u_Model;       // The matrix that defines the transformation of the
                            // object we're rendering. In this assignment,
                            // this will be the result of traversing your scene graph.

uniform mat4 u_ModelInvTr;  // The inverse transpose of the model matrix.
                            // This allows us to transform the object's normals properly
                            // if the object has been non-uniformly scaled.

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself
uniform int u_Height;
uniform int u_Time;

uniform vec4 LDirection;

in vec4 vs_Pos;             // The array of vertex positions passed to the shader
in vec4 vs_Nor;             // The array of vertex normals passed to the shader
in vec4 vs_Col;             // The array of vertex colors passed to the shader.
in vec2 vs_UV;             // The array of vertex UVs passed to the shader.
in vec4 vs_tangent;
in vec4 vs_bitangent;
in float vs_shininess;
in int vs_flag_animate;

out vec4 fs_Pos;
out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_LightVec;       // The direction in which our virtual light lies, relative to each vertex. This is implicitly passed to the fragment shader.
out vec4 fs_Col;            // The color of each vertex. This is implicitly passed to the fragment shader.
out vec2 fs_UVs;            //UV coordinates of the current texture
out vec4 fs_tangent;
out vec4 fs_bitangent;
out float fs_shininess;
flat out int time;
flat out int fs_flag_animate;

const vec4 lightDir = vec4(1,1,1,0);  // The direction of our virtual light, which is used to compute the shading of
                                        // the geometry in the fragment shader.

void main()
{
    vec4 Pos = vs_Pos;
    time = u_Time;
    if(vs_flag_animate == 2)
    {
        //then its a cloud block
        int t = (u_Time%100);
        Pos = vs_Pos - vec4(t,0,0,0);
    }
    else if(vs_flag_animate >= 11)
    {
        float h = (u_Height);

        if(Pos[1] <= 0)
        {
            Pos = vs_Pos;
        }
        else
        {
            Pos = vs_Pos - vec4(0,h,0,0);
        }
    }

    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);          // Pass the vertex normals to the fragment shader for interpolation.
                                                            // Transform the geometry's normals by the inverse transpose of the
                                                            // model matrix. This is necessary to ensure the normals remain
                                                            // perpendicular to the surface after the surface is transformed by
                                                            // the model matrix.

    vec4 modelposition = u_Model * Pos;   // Temporarily store the transformed vertex positions for use below
    //fs_LightVec = (lightDir);  // Compute the direction in which the light source lies
    fs_LightVec = LDirection;

    fs_Col = vs_Col;                         // Pass the vertex colors to the fragment shader for interpolation
    fs_UVs = vs_UV;
    fs_Pos = Pos;
    fs_shininess = vs_shininess;
    fs_tangent = vs_tangent;
    fs_bitangent = vs_bitangent;
    fs_flag_animate = vs_flag_animate;

    gl_Position = u_ViewProj * modelposition;// gl_Position is a built-in variable of OpenGL which is
                                             // used to render the final positions of the geometry's vertices
}
