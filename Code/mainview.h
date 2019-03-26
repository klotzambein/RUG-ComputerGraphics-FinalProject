#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QImage>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QVector3D>
#include <QVector>
#include <memory>
#include <vector>
#include <map>
#include <string>

typedef std::map<std::string, GLint> UniformMap;

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {

    // State
    float rotation_x = 0;
    float rotation_y = 0;
    QVector2D pos_xz;

    QVector2D lastMousePos;
    bool pressed_w = false;
    bool pressed_a = false;
    bool pressed_s = false;
    bool pressed_d = false;

    float gameTime = 0;
    float aspectRatio;

    QOpenGLDebugLogger* debugLogger;
    QTimer timer; // timer used for animation

    std::string fragPath;
    uint32_t fragHash;

    // OpenGL refs
    QOpenGLShaderProgram shaderProgram;
    UniformMap uniforms;
    std::vector<GLuint> textures;
    GLuint quadVAO;
    GLuint quadVBO;

public:
    MainView(QWidget* parent = 0);
    ~MainView();

protected:
    void initializeGL();
    void resizeGL(int newWidth, int newHeight);
    void paintGL();

    // Functions for keyboard input events
    void keyPressEvent(QKeyEvent* ev);
    void keyReleaseEvent(QKeyEvent* ev);

    // Function for mouse input events
    void mouseDoubleClickEvent(QMouseEvent* ev);
    void mouseMoveEvent(QMouseEvent* ev);
    void mousePressEvent(QMouseEvent* ev);
    void mouseReleaseEvent(QMouseEvent* ev);
    void wheelEvent(QWheelEvent* ev);

private slots:
    void onMessageLogged(QOpenGLDebugMessage Message);

private:
    void reloadShaderProgram(std::string fragPath);
    void createShaderProgram(std::string fragPath);
    void createQuad();
    void createTextures(std::vector<std::string> paths);

    void updateViewTransform();
    void updateProjectionTransform();

    // Useful utility method to convert image to bytes.
    QVector<quint8> imageToBytes(QImage image);
};

#endif // MAINVIEW_H
