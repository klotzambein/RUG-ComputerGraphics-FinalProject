#include "mainview.h"

#include <QDateTime>
#include <math.h>
#include <fstream>

/**
 * @brief MainView::MainView
 *
 * Constructor of MainView
 *
 * @param parent
 */
MainView::MainView(QWidget* parent)
    : QOpenGLWidget(parent)
{
    qDebug() << "MainView constructor";
    gameTime = 0.0;
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView()
{
    debugLogger->stopLogging();

    qDebug() << "MainView destructor";

    glDeleteTextures(int(textures.size()), textures.data());
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL
 *
 * Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions
 */
void MainView::initializeGL()
{
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    debugLogger = new QOpenGLDebugLogger();
    connect(debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)),
        this, SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

    if (debugLogger->initialize()) {
        qDebug() << ":: Logging initialized";
        debugLogger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
        debugLogger->enableMessages();
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glDepthFunc(GL_LEQUAL);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    createShaderProgram(":/shaders/fragshader.glsl");
    createQuad();
    createTextures({":/textures/snakes.png", ":/textures/terrain.png" });

    timer.start(1000 / 60);
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL()
{
    if (pressed_w)
        pos_xz += QVector2D(sin(rotation_x/57.2974f), cos(rotation_x / 57.2974f)) / 100.0;
    if (pressed_s)
        pos_xz += QVector2D(-sin(rotation_x/57.2974f), -cos(rotation_x / 57.2974f)) / 100.0;
    if (pressed_a)
        pos_xz += QVector2D(-cos(rotation_x/57.2974f), sin(rotation_x / 57.2974f)) / 100.0;
    if (pressed_d)
        pos_xz += QVector2D(cos(rotation_x/57.2974f), -sin(rotation_x / 57.2974f)) / 100.0;

    QMatrix4x4 view;
    view.setToIdentity();
    view.translate(QVector3D(pos_xz.x(), 3.0f, pos_xz.y()));
    view.rotate(rotation_x, QVector3D(0, 1.0f, 0));
    view.rotate(rotation_y, QVector3D(1.0f, 0, 0));

    gameTime += timer.interval() / 1000.0f;

    // Clear the screen before rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Choose the selected shader.
    shaderProgram.bind();

//    for (unsigned i = 0; i < textures.size(); i++) {
//        glActiveTexture(GL_TEXTURE0 + i);
//        glBindTexture(GL_TEXTURE_2D, textures[i]);
//    }

    glUniform1f(uniforms["uAspectRatio"], aspectRatio);
    glUniform1f(uniforms["uTime"], gameTime);
    glUniformMatrix4fv(uniforms["uView"], 1, false, view.data());
    glUniform1i(uniforms["uTex0"], 0);
    glUniform1i(uniforms["uTex1"], 1);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    shaderProgram.release();
}

/**
 * @brief MainView::resizeGL
 *
 * Called upon resizing of the screen
 *
 * @param newWidth
 * @param newHeight
 */
void MainView::resizeGL(int newWidth, int newHeight)
{
    aspectRatio = float(newWidth) / float(newHeight);
}

// --- Private helpers

/**
 * @brief MainView::onMessageLogged
 *
 * OpenGL logging function, do not change
 *
 * @param Message
 */
void MainView::onMessageLogged(QOpenGLDebugMessage Message)
{
    qDebug() << " → Log:" << Message;
}
