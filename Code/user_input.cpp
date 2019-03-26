#include "mainview.h"

#include <QDebug>
#include <math.h>

// Triggered by pressing a key
void MainView::keyPressEvent(QKeyEvent *ev)
{
    // ev->key() is an integer. For alpha numeric characters keys it equivalent with the char value ('A' == 65, '1' == 49)
    // Alternatively, you could use Qt Key enums, see http://doc.qt.io/qt-5/qt.html#Key-enum
    qDebug() << ev->key() << "pressed";
    switch (ev->key()) {
    case 'W': pressed_w = true; break;
    case 'A': pressed_a = true; break;
    case 'S': pressed_s = true; break;
    case 'D': pressed_d = true; break;
    }

}

// Triggered by releasing a key
void MainView::keyReleaseEvent(QKeyEvent *ev)
{
    switch (ev->key()) {
    case 'W': pressed_w = false; break;
    case 'A': pressed_a = false; break;
    case 'S': pressed_s = false; break;
    case 'D': pressed_d = false; break;
    }

    update();
}

// Triggered by clicking two subsequent times on any mouse button
// It also fires two mousePress and mouseRelease events!
void MainView::mouseDoubleClickEvent(QMouseEvent *ev)
{
    qDebug() << "Mouse double clicked:" << ev->button();
}

// Triggered when moving the mouse inside the window (only when the mouse is clicked!)
void MainView::mouseMoveEvent(QMouseEvent *ev)
{
    qDebug() << "x" << ev->x() << "y" << ev->y();
    QVector2D pos(ev->x(), ev->y());
    QVector2D delta = (pos - lastMousePos);
    rotation_x += delta.x() / -5.0f;
    rotation_y += delta.y() / 5.0f;
    if (rotation_y < -90)
        rotation_y = -90;
    if (rotation_y > 90)
        rotation_y = 90;
    lastMousePos = pos;
}

// Triggered when pressing any mouse button
void MainView::mousePressEvent(QMouseEvent *ev)
{
    qDebug() << "Mouse button pressed:" << ev->button();
    lastMousePos = QVector2D(ev->x(), ev->y());
    // Do not remove the line below, clicking must focus on this widget!
    this->setFocus();
}

// Triggered when releasing any mouse button
void MainView::mouseReleaseEvent(QMouseEvent *ev)
{
    qDebug() << "Mouse button released" << ev->button();
}

// Triggered when clicking scrolling with the scroll wheel on the mouse
void MainView::wheelEvent(QWheelEvent *ev)
{
    // Implement something
    qDebug() << "Mouse wheel:" << ev->delta();
//    setScale(scale + ev->delta() * 0.005);
}
