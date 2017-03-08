#include "shaderprogram.h"
#include <QFile>
#include <QStringBuilder>


ShaderProgram::ShaderProgram(GLWidget277 *context)
    : vertShader(), fragShader(), prog(),
      //texture_general_handle(),
      attrPos(-1), attrNor(-1), attrCol(-1),
      attrUVs(-1), attrtangent(-1), attrbitangent(-1),
      attrshininess(-1), attranimatetexture_flag(-1),
      unifModel(-1), unifModelInvTr(-1), unifViewProj(-1), unifColor(-1),
      unifCameraEye(-1), unifTimeCount(-1), unifHeight(-1),
      unifLIntensity(-1), unifLColor(-1), unifLDirection(-1),
      unifTextureSampler(-1), unifNormalSampler(-1),
      unifSnowSampler(-1), unifRainSampler(-1),
      context(context)
{}

void ShaderProgram::create(const char *vertfile, const char *fragfile)
{
    // Allocate space on our GPU for a vertex shader and a fragment shader and a shader program to manage the two
    vertShader = context->glCreateShader(GL_VERTEX_SHADER);
    fragShader = context->glCreateShader(GL_FRAGMENT_SHADER);
    prog = context->glCreateProgram();
    // Get the body of text stored in our two .glsl files
    QString qVertSource = qTextFileRead(vertfile);
    QString qFragSource = qTextFileRead(fragfile);

    char* vertSource = new char[qVertSource.size()+1];
    strcpy(vertSource, qVertSource.toStdString().c_str());
    char* fragSource = new char[qFragSource.size()+1];
    strcpy(fragSource, qFragSource.toStdString().c_str());


    // Send the shader text to OpenGL and store it in the shaders specified by the handles vertShader and fragShader
    context->glShaderSource(vertShader, 1, &vertSource, 0);
    context->glShaderSource(fragShader, 1, &fragSource, 0);
    // Tell OpenGL to compile the shader text stored above
    context->glCompileShader(vertShader);
    context->glCompileShader(fragShader);
    // Check if everything compiled OK
    GLint compiled;
    context->glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printShaderInfoLog(vertShader);
    }
    context->glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printShaderInfoLog(fragShader);
    }

    // Tell prog that it manages these particular vertex and fragment shaders
    context->glAttachShader(prog, vertShader);
    context->glAttachShader(prog, fragShader);
    context->glLinkProgram(prog);

    // Check for linking success
    GLint linked;
    context->glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) {
        printLinkInfoLog(prog);
    }

    // Get the handles to the variables stored in our shaders
    // See shaderprogram.h for more information about these variables

    attrPos = context->glGetAttribLocation(prog, "vs_Pos");
    attrNor = context->glGetAttribLocation(prog, "vs_Nor");
    attrCol = context->glGetAttribLocation(prog, "vs_Col");
    attrUVs = context->glGetAttribLocation(prog, "vs_UV");
    attrtangent = context->glGetAttribLocation(prog, "vs_tangent");
    attrbitangent = context->glGetAttribLocation(prog, "vs_bitangent");
    attrshininess = context->glGetAttribLocation(prog, "vs_shininess");
    attranimatetexture_flag = context->glGetAttribLocation(prog, "vs_flag_animate");

    unifModel      = context->glGetUniformLocation(prog, "u_Model");
    unifModelInvTr = context->glGetUniformLocation(prog, "u_ModelInvTr");
    unifViewProj   = context->glGetUniformLocation(prog, "u_ViewProj");
    unifColor      = context->glGetUniformLocation(prog, "u_Color");

    unifCameraEye  = context->glGetUniformLocation(prog, "u_CameraEye");
    unifTimeCount  = context->glGetUniformLocation(prog, "u_Time");
    unifHeight = context->glGetUniformLocation(prog, "u_Height");
    unifLIntensity = context->glGetUniformLocation(prog, "LIntensity");
    unifLColor = context->glGetUniformLocation(prog, "LColor");
    unifLDirection = context->glGetUniformLocation(prog, "LDirection");

    unifTextureSampler = context->glGetUniformLocation(prog, "TextureSampler");
    unifNormalSampler = context->glGetUniformLocation(prog, "NormalSampler");
    unifSnowSampler = context->glGetUniformLocation(prog, "SnowSampler");
    unifRainSampler = context->glGetUniformLocation(prog, "RainSampler");
}

//Texture_general functions are for things like the billboards used for weather
void ShaderProgram::setActiveTexture_General(int tex_slot_num)
{
    switch(tex_slot_num)
    {
        case 2: //GL_TEXTURE2 -- snow
            context->glActiveTexture(GL_TEXTURE2);
            context->glBindTexture(GL_TEXTURE_2D, snow_handle);
            context->glUseProgram(prog);
            context->glUniform1i(unifSnowSampler, 2);
            break;
        case 3: //GL_TEXTURE3 -- rain
            context->glActiveTexture(GL_TEXTURE3);
            context->glBindTexture(GL_TEXTURE_2D, rain_handle);
            context->glUseProgram(prog);
            context->glUniform1i(unifRainSampler, 3);
            break;
    }
}

void ShaderProgram::setupTexture_General(const char* imagepath, int& width, int& height, int tex_slot_num, bool transparnecy_flag)
{
    switch(tex_slot_num)
    {
        case 2: //GL_TEXTURE2 -- snow
            context->glGenTextures(1, &snow_handle);
            context->glActiveTexture(GL_TEXTURE2);
            context->glBindTexture(GL_TEXTURE_2D, snow_handle);
            break;
        case 3: //GL_TEXTURE3 -- rain
            context->glGenTextures(1, &rain_handle);
            context->glActiveTexture(GL_TEXTURE3);
            context->glBindTexture(GL_TEXTURE_2D, rain_handle);
            break;
    }

    //set param variable of glTexParameteri() to GL_REPEAT in case the UV's
    //are out of [0,1] range
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T , GL_REPEAT);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER , GL_NEAREST);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER , GL_NEAREST);

    // set up SOIL
    if(transparnecy_flag)
    {
        image = SOIL_load_image(imagepath,
                                &width,
                                &height,
                                0,
                                SOIL_LOAD_RGBA);
        std::cout << "SOIL RESULTS: " << SOIL_last_result() << std::endl;
        context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }
    else
    {
        image = SOIL_load_image(imagepath,
                                &width,
                                &height,
                                0,
                                SOIL_LOAD_RGB);
        std::cout << "SOIL RESULTS: " << SOIL_last_result() << std::endl;
        context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }

    context->glUseProgram(prog);

    switch(tex_slot_num)
    {
        case 2: //GL_TEXTURE2 -- snow
            unifSnowSampler = context->glGetUniformLocation(prog, "SnowSampler");
            context->glUniform1i(unifSnowSampler, 2);
            break;
        case 3: //GL_TEXTURE3 -- rain
            unifRainSampler = context->glGetUniformLocation(prog, "RainSampler");
            context->glUniform1i(unifRainSampler, 3);
            break;
    }
}

//specific texture functions for any thing that has to do with actual blocks that make up the minecraft world
void ShaderProgram::setActiveTexture()
{
    context->glActiveTexture(GL_TEXTURE0);
    context->glBindTexture(GL_TEXTURE_2D, textureHandle);

    context->glUseProgram(prog);
    context->glUniform1i(unifTextureSampler, 0);
}

void ShaderProgram::setupTexture(const char* imagepath, int& width, int& height)
{
    context->glGenTextures(1, &textureHandle);
    context->glActiveTexture(GL_TEXTURE0);
    context->glBindTexture(GL_TEXTURE_2D, textureHandle);

    //set param variable of glTexParameteri() to GL_REPEAT in case the UV's
    //are out of [0,1] range
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T , GL_REPEAT);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER , GL_NEAREST);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER , GL_NEAREST);

    // set up SOIL
    image = SOIL_load_image(imagepath,
                            &width,
                            &height,
                            0,
                            SOIL_LOAD_RGB);
    std::cout << "SOIL RESULTS: " << SOIL_last_result() << std::endl;
    context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    context->glUseProgram(prog);
    unifTextureSampler = context->glGetUniformLocation(prog, "TextureSampler");
    context->glUniform1i(unifTextureSampler, 0);
}

void ShaderProgram::setActiveNormalMap()
{
    context->glActiveTexture(GL_TEXTURE1);
    context->glBindTexture(GL_TEXTURE_2D, normalHandle);

    context->glUseProgram(prog);
    context->glUniform1i(unifNormalSampler, 1);
}

void ShaderProgram::setupNormalMap(const char* imagepath, int& width, int& height)
{
    context->glGenTextures(1, &normalHandle);
    context->glActiveTexture(GL_TEXTURE1);
    context->glBindTexture(GL_TEXTURE_2D, normalHandle);

    //set param variable of glTexParameteri() to GL_REPEAT in case the UV's
    //are out of [0,1] range
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T , GL_REPEAT);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER , GL_NEAREST);
    context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER , GL_NEAREST);

    // set up SOIL
    image = SOIL_load_image(imagepath,
                            &width,
                            &height,
                            0,
                            SOIL_LOAD_RGB);
    std::cout << "SOIL RESULTS: " << SOIL_last_result() << std::endl;
    context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    context->printGLErrorLog();

    context->glUseProgram(prog);
    unifNormalSampler = context->glGetUniformLocation(prog, "NormalSampler");
    context->glUniform1i(unifNormalSampler, 1);
    context->printGLErrorLog();
}

void ShaderProgram::useMe()
{
    context->glUseProgram(prog);
}

void ShaderProgram::setTimeCount(int timecount)
{
    useMe();
    if(unifTimeCount != -1)
    {
        context->glUniform1i(unifTimeCount, timecount);
    }
}

void ShaderProgram::setHeight(int h)
{
    useMe();
    if(unifHeight != -1)
    {
        context->glUniform1i(unifHeight, h);
    }
}

void ShaderProgram::setLIntensity(float lintensity)
{
    useMe();

    if (unifLIntensity != -1)
    {
        context->glUniform1f(unifLIntensity, lintensity);
    }
}

void ShaderProgram::setLColor(glm::vec4 lcolor)
{
    useMe();

    if (unifLColor != -1)
    {
        context->glUniform4fv(unifLColor, 1, &lcolor[0]);
    }
}

void ShaderProgram::setLDirection(glm::vec4 ldir)
{
    useMe();

    if (unifLDirection != -1)
    {
        context->glUniform4fv(unifLDirection, 1, &ldir[0]);
    }
}

void ShaderProgram::setCameraEye(glm::vec3 camera_eye)
{
    useMe();

    glm::vec4 ce(camera_eye, 1);
    if (unifCameraEye != -1)
    {
        context->glUniform4fv(unifCameraEye, 1, &ce[0]);
    }
}

void ShaderProgram::setModelMatrix(const glm::mat4 &model)
{
    useMe();

    if (unifModel != -1) {
        // Pass a 4x4 matrix into a uniform variable in our shader
                        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModel,
                        // How many matrices to pass
                           1,
                        // Transpose the matrix? OpenGL uses column-major, so no.
                           GL_FALSE,
                        // Pointer to the first element of the matrix
                           &model[0][0]);
    }

    if (unifModelInvTr != -1) {
        glm::mat4 modelinvtr = glm::inverse(glm::transpose(model));
        // Pass a 4x4 matrix into a uniform variable in our shader
                        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModelInvTr,
                        // How many matrices to pass
                           1,
                        // Transpose the matrix? OpenGL uses column-major, so no.
                           GL_FALSE,
                        // Pointer to the first element of the matrix
                           &modelinvtr[0][0]);
    }
}

void ShaderProgram::setViewProjMatrix(const glm::mat4 &vp)
{
    // Tell OpenGL to use this shader program for subsequent function calls
    useMe();

    if(unifViewProj != -1) {
    // Pass a 4x4 matrix into a uniform variable in our shader
                    // Handle to the matrix variable on the GPU
    context->glUniformMatrix4fv(unifViewProj,
                    // How many matrices to pass
                       1,
                    // Transpose the matrix? OpenGL uses column-major, so no.
                       GL_FALSE,
                    // Pointer to the first element of the matrix
                       &vp[0][0]);
    }
}

void ShaderProgram::setGeometryColor(glm::vec4 color)
{
    useMe();

    if(unifColor != -1)
    {
        context->glUniform4fv(unifColor, 1, &color[0]);
    }
}

void ShaderProgram::draw(Drawable &d)
{
        useMe();

    // Each of the following blocks checks that:
    //   * This shader has this attribute, and
    //   * This Drawable has a vertex buffer for this attribute.
    // If so, it binds the appropriate buffers to each attribute.

        // Remember, by calling bindPos(), we call
        // glBindBuffer on the Drawable's VBO for vertex position,
        // meaning that glVertexAttribPointer associates vs_Pos
        // (referred to by attrPos) with that VBO
    if (attrPos != -1 && d.bindPos()) {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrNor != -1 && d.bindNor()) {
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrCol != -1 && d.bindCol()) {
        context->glEnableVertexAttribArray(attrCol);
        context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 0, NULL);
    }

    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindIdx();
    context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);

    context->printGLErrorLog();
}

void ShaderProgram::DrawWeather(Drawable &d)
{
    useMe();
    d.bindGeneralVBO();

    int stride = sizeof(weather_vertex_data);
    if (attrPos != -1)
    {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, GL_FALSE, stride, (void*)0);
        context->printGLErrorLog();
    }
    if (attrNor != -1)
    {
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec4)));
        context->printGLErrorLog();
    }
    if (attrUVs != -1)
    {
        context->glEnableVertexAttribArray(attrUVs);
        context->glVertexAttribPointer(attrUVs, 2, GL_FLOAT, GL_FALSE,stride, (void*)(2*sizeof(glm::vec4)));
        context->printGLErrorLog();
    }
    if (attranimatetexture_flag != -1)
    {
        int size = 2*sizeof(glm::vec4) + sizeof(glm::vec2);
        context->glEnableVertexAttribArray(attranimatetexture_flag);
        context->glVertexAttribIPointer(attranimatetexture_flag, 1, GL_INT, stride, (void*)(size));

        context->printGLErrorLog();
    }

    d.bindGeneralVBO();
    context->glDrawElements(d.drawMode(), d.VBOindex_count, GL_UNSIGNED_INT, 0);


    if (attrPos != -1)
    {
        context->glDisableVertexAttribArray(attrPos);
    }
    if (attrNor != -1)
    {
        context->glDisableVertexAttribArray(attrNor);
    }
    if(attrUVs != -1)
    {
        context->glDisableVertexAttribArray(attrUVs);
    }
    if(attranimatetexture_flag != -1)
    {
        context->glDisableVertexAttribArray(attranimatetexture_flag);
    }

    context->printGLErrorLog();
}

void ShaderProgram::VBOdraw(Drawable &d, int index)
{
    useMe();

    d.bindVBO(index);
    int stride = sizeof(vertex_data);

    if (attrPos != -1)
    {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, GL_FALSE, stride, (void*)0);
        //3*sizeof(glm::vec4) is for the distance between 2 color values or normal values or position values
        //(void*)0 is a pointer to the first position color or normal value

        context->printGLErrorLog();
    }
    if (attrCol != -1)
    {
        context->glEnableVertexAttribArray(attrCol);
        context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec4)));

        context->printGLErrorLog();
    }
    if (attrNor != -1)
    {
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2*sizeof(glm::vec4)));

        context->printGLErrorLog();
    }
    if (attrUVs != -1)
    {
        context->glEnableVertexAttribArray(attrUVs);
        context->glVertexAttribIPointer(attrUVs, 2, GL_INT, stride, (void*)(3*sizeof(glm::vec4)));

        context->printGLErrorLog();
    }
    if (attrtangent != -1)
    {
        int size = 3*sizeof(glm::vec4) + sizeof(glm::vec2);
        context->glEnableVertexAttribArray(attrtangent);
        context->glVertexAttribPointer(attrtangent, 4, GL_FLOAT, GL_FALSE, stride, (void*)(size));

        context->printGLErrorLog();
    }
    if (attrbitangent != -1)
    {
        int size = 3*sizeof(glm::vec4) + sizeof(glm::vec2) + sizeof(glm::vec4);
        context->glEnableVertexAttribArray(attrbitangent);
        context->glVertexAttribPointer(attrbitangent, 4, GL_FLOAT, GL_FALSE, stride, (void*)(size));

        context->printGLErrorLog();
    }
    if (attrshininess != -1)
    {
        int size = 3*sizeof(glm::vec4) + sizeof(glm::vec2) + 2*sizeof(glm::vec4);
        context->glEnableVertexAttribArray(attrshininess);
        context->glVertexAttribPointer(attrshininess, 1, GL_FLOAT, GL_FALSE, stride, (void*)(size));

        context->printGLErrorLog();
    }
    if (attranimatetexture_flag != -1)
    {
        int size = 3*sizeof(glm::vec4) + sizeof(glm::vec2) + 2*sizeof(glm::vec4) + sizeof(float);
        context->glEnableVertexAttribArray(attranimatetexture_flag);
        context->glVertexAttribIPointer(attranimatetexture_flag, 1, GL_INT, stride, (void*)(size));

        context->printGLErrorLog();
    }

    d.bindVBO(index);
    context->glDrawElements(d.drawMode(), d.IndexCount[index], GL_UNSIGNED_INT, 0);


    if (attrPos != -1)
    {
        context->glDisableVertexAttribArray(attrPos);
    }
    if (attrNor != -1)
    {
        context->glDisableVertexAttribArray(attrNor);
    }
    if (attrCol != -1)
    {
        context->glDisableVertexAttribArray(attrCol);
    }

    if(attrUVs != -1)
    {
        context->glDisableVertexAttribArray(attrUVs);
    }
    if(attrtangent != -1)
    {
        context->glDisableVertexAttribArray(attrtangent);
    }
    if(attrbitangent != -1)
    {
        context->glDisableVertexAttribArray(attrbitangent);
    }
    if(attrshininess != -1)
    {
        context->glDisableVertexAttribArray(attrshininess);
    }
    if(attranimatetexture_flag != -1)
    {
        context->glDisableVertexAttribArray(attranimatetexture_flag);
    }

    context->printGLErrorLog();
}

char* ShaderProgram::textFileRead(const char* fileName) {
    char* text;

    if (fileName != NULL) {
        FILE *file = fopen(fileName, "rt");

        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);

            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';	//cap off the string with a terminal symbol, fixed by Cory
            }
            fclose(file);
        }
    }
    return text;
}

QString ShaderProgram::qTextFileRead(const char *fileName)
{
    QString text;
    QFile file(fileName);
    if(file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);
        text = in.readAll();
        text.append('\0');
    }
    return text;
}

void ShaderProgram::printShaderInfoLog(int shader)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    context->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0)
    {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetShaderInfoLog(shader,infoLogLen, &charsWritten, infoLog);
        qDebug() << "ShaderInfoLog:" << endl << infoLog << endl;
        delete [] infoLog;
    }

    // should additionally check for OpenGL errors here
}

void ShaderProgram::printLinkInfoLog(int prog)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    context->glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
        qDebug() << "LinkInfoLog:" << endl << infoLog << endl;
        delete [] infoLog;
    }
}
