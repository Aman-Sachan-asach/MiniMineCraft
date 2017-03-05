#version 150
// ^ Change this to version 130 if you have compatibility issues

// This is a fragment shader. If you've opened this file first, please
// open and read lambert.vert.glsl before reading on.
// Unlike the vertex shader, the fragment shader actually does compute
// the shading of geometry. For every pixel in your program's output
// screen, the fragment shader is run for every bit of geometry that
// particular pixel overlaps. By implicitly interpolating the position
// data passed into the fragment shader by the vertex shader, the fragment shader
// can compute what color to apply to its pixel based on things like vertex
// position, light position, and vertex color.

uniform vec4 u_Color; // The color with which to render this instance of geometry.
uniform vec4 u_CameraEye;

uniform sampler2D TextureSampler; //uniform variable required to read the texture
uniform sampler2D NormalSampler; //uniform variable required to read the normal texture
uniform sampler2D SnowSampler; //uniform variable required to read the texture
uniform sampler2D RainSampler; //uniform variable required to read the normal texture
in vec2 fs_UVs; //UV coordinates of the current texture

uniform float LIntensity;
uniform vec4 LColor;

// These are the interpolated values out of the rasterizer, so you can't know
// their specific values without knowing the vertices that contributed to them
in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec4 fs_Col;
in vec4 fs_tangent;
in vec4 fs_bitangent;
in float fs_shininess;
flat in int time;
flat in int fs_flag_animate;

out vec4 out_Color; // This is the final output color that you will see on your
                  // screen for the pixel that is currently being processed.

void main()
{
    // Material base color (before shading)
    vec4 diffuseColor = fs_Col;

    //-----------Normal Mapping---------
    vec4 last_column = vec4(0.0f,0.0f,0.0f,1.0f);
    mat4 TBN = mat4(fs_tangent, fs_bitangent, fs_Nor, last_column);

    vec2 UV_coords;
    vec4 Normal;
    vec4 tex_color;
    //for animating TextureSampler
    if(fs_flag_animate == 1)
    {
        int t = (time%5);
        float div_factor = 5;
        if(t == 0)
        {
            UV_coords = fs_UVs;
        }
        else if(t == 1)
        {
            UV_coords = fs_UVs + vec2(((1.0f/16.0f)/div_factor),0);
        }
        else if(t == 2)
        {
            UV_coords = fs_UVs + vec2(((2.0f/16.0f)/div_factor),0);
        }
        else if(t == 3)
        {
            UV_coords = fs_UVs + vec2(((3.0f/16.0f)/div_factor),0);
        }
        else if(t == 4)
        {
            UV_coords = fs_UVs + vec2(((4.0f/16.0f)/div_factor),0);
        }
    }
    else
    {
        UV_coords = fs_UVs;
    }

    if(fs_flag_animate >= 11)
    {
        Normal = fs_Nor;
    }
    else if(fs_flag_animate < 10)
    {
        vec4 _normal = texture(NormalSampler, UV_coords);
        Normal = normalize((2.0f * _normal) - 1.0f);
        Normal = normalize(TBN*Normal);
    }
    //----------------------------------

    //-----------Texture Mapping--------

    if(fs_flag_animate == 12)
    {
        tex_color = texture(SnowSampler, UV_coords);
    }
    else if(fs_flag_animate == 11)
    {
        tex_color = texture(RainSampler, UV_coords);
    }
    else if(fs_flag_animate <= 10)
    {
        tex_color = texture(TextureSampler, UV_coords);
    }

    //----------------------------------
    float lightIntensity = 0.5f;
    vec4 specularColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    if(fs_flag_animate < 2)
    {
        //-----------Specularity---------
        vec4 cameye = u_CameraEye-fs_Pos;
        vec4 L = normalize(-fs_LightVec); //a unit vector
        vec4 V = normalize(-cameye); //also a unit vector
        vec4 H = (V+L)/2;
        //it's (1/(dot(H,Normal))) because shininess does'nt show up otherwise
        float specularCoefficient = max(pow((dot(H,Normal)), (0.2f + fs_shininess)), 0);
        //vec4 specularColor = specularCoefficient * tex_color;
        specularColor = specularCoefficient * vec4(1.0f, 1.0f, 1.0f, 1.0f);
        //-------------------------------

        //-----------Lambertian Lighting---------
        // Calculate the diffuse term for Lambert shading
        float diffuseTerm = dot(normalize(Normal), normalize(fs_LightVec));
        // Avoid negative lighting values
        diffuseTerm = clamp(diffuseTerm, 0, 1);

        float ambientTerm = 0.4;

        lightIntensity = diffuseTerm + ambientTerm;   //Add a small float value to the color multiplier
                                                            //to simulate ambient lighting. This ensures that faces that are not
                                                            //lit by our point light are not completely black.
        lightIntensity = lightIntensity*LIntensity;
    }
    //---------------------------------------
    else if(fs_flag_animate == 2)
    {
        //then its a cloud block
        lightIntensity = 0.85f;
    }
    else if(fs_flag_animate >= 11)
    {
        lightIntensity = 1.0f;
    }

    // Compute final shaded color
    vec4 final_color;
    vec4 color_wo_specular = vec4(tex_color.rgb * lightIntensity, tex_color[3]);

    if(fs_flag_animate < 2)
    {
        final_color = color_wo_specular + specularColor;
        if((LColor[0]) < 0.1)
        {
            final_color = final_color * (0.333); //To make things Darker
        }
        else if(LColor[0] < 0.2)
        {
            final_color = final_color * (0.333 + 0.667*0.1);
        }
        else if(LColor[0] < 0.3)
        {
            final_color = final_color * (0.333 + 0.667*0.2);
        }
        else if(LColor[0] < 0.4)
        {
            final_color = final_color * (0.333 + 0.667*0.3);
        }
        else if(LColor[0] < 0.5)
        {
            final_color = final_color * (0.333 + 0.667*0.4);
        }
        else if(LColor[0] < 0.6)
        {
            final_color = final_color * (0.333 + 0.667*0.5);
        }
        else if(LColor[0] < 0.7)
        {
            final_color = final_color * (0.333 + 0.667*0.6);
        }
        else if(LColor[0] < 0.8)
        {
            final_color = final_color * (0.333 + 0.667*0.7);
        }
        else if(LColor[0] < 0.9)
        {
            final_color = final_color * (0.333 + 0.667*0.8);
        }
        else
        {
            final_color = final_color;
        }

        final_color[3] = 1;
    }
    else if(fs_flag_animate == 2)
    {
       final_color = color_wo_specular + specularColor;
    }
    else if(fs_flag_animate >= 11)
    {
       final_color = color_wo_specular;
    }

    out_Color = final_color;
    //out_Color = specularColor;
    //out_Color = color_wo_specular;
    //out_Color = tex_color;
    //out_Color = Normal;
}
