#include "mainview.h"

QVector<quint8> MainView::imageToBytes(QImage image)
{
    // needed since (0,0) is bottom left in OpenGL
    QImage im = image.mirrored();
    QVector<quint8> pixelData;
    pixelData.reserve(im.width() * im.height() * 4);

    for (int i = 0; i != im.height(); ++i) {
        for (int j = 0; j != im.width(); ++j) {
            QRgb pixel = im.pixel(j, i);

            // pixel is of format #AARRGGBB (in hexadecimal notation)
            // so with bitshifting and binary AND you can get
            // the values of the different components
            quint8 r = quint8((pixel >> 16) & 0xFF); // Red component
            quint8 g = quint8((pixel >> 8) & 0xFF); // Green component
            quint8 b = quint8(pixel & 0xFF); // Blue component
            quint8 a = quint8((pixel >> 24) & 0xFF); // Alpha component

            // Add them to the Vector
            pixelData.append(r);
            pixelData.append(g);
            pixelData.append(b);
            pixelData.append(a);
        }
    }
    return pixelData;
}

void MainView::reloadShaderProgram(std::string fragPath) {
    shaderProgram.release();
    shaderProgram.removeAllShaders();

    // Create Normal Shader program
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
        ":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
        fragPath.c_str());
    shaderProgram.link();

    uniforms["uAspectRatio"] = GLint(shaderProgram.uniformLocation("uAspectRatio"));
    uniforms["uTime"] = GLint(shaderProgram.uniformLocation("uTime"));
    uniforms["uView"] = GLint(shaderProgram.uniformLocation("uView"));
    uniforms["uTex0"] = GLint(shaderProgram.uniformLocation("uTex0"));
    uniforms["uTex1"] = GLint(shaderProgram.uniformLocation("uTex1"));
}

void MainView::createShaderProgram(std::string fragPath)
{
    this->fragPath = fragPath;

    // Create Normal Shader program
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
        ":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
        fragPath.c_str());
    shaderProgram.link();

    uniforms["uAspectRatio"] = GLint(shaderProgram.uniformLocation("uAspectRatio"));
    uniforms["uTime"] = GLint(shaderProgram.uniformLocation("uTime"));
    uniforms["uMousePos"] = GLint(shaderProgram.uniformLocation("uMousePos"));
    uniforms["uMouseClicked"] = GLint(shaderProgram.uniformLocation("uMouseClicked"));
    uniforms["uTex0"] = GLint(shaderProgram.uniformLocation("uTex0"));
    uniforms["uTex1"] = GLint(shaderProgram.uniformLocation("uTex1"));
}

void MainView::createQuad()
{
    glGenBuffers(1, &quadVBO);
    glGenVertexArrays(1, &quadVAO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

    float quadData[2 * 6] = {
        0,
        0,
        0,
        1,
        1,
        1,
        0,
        0,
        1,
        0,
        1,
        1,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(quadData), &quadData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(float) * 2, nullptr);
}

void MainView::createTextures(std::vector<std::string> paths)
{
    textures.clear();
    textures.resize(paths.size());
    glGenTextures(GLint(paths.size()), textures.data());

    for (unsigned i = 0; i < paths.size(); i++) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // Push image data to texture.
        QImage image(paths[i].c_str());
        QVector<quint8> imageData = imageToBytes(image);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height(),
            0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.data());
    }
}
