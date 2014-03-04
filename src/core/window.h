/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_WINDOW_H
#define QT3D_WINDOW_H

#include <QWindow>
#include "qt3dcore_global.h"

#include <QQmlEngine>

class QTimer;

namespace Qt3D {

class AbstractAspect;
class QAspectEngine;
class Camera;

// temporary solution to get control over camera
class CameraController;

class QT3DCORESHARED_EXPORT Window : public QWindow
{
    Q_OBJECT
public:
    explicit Window(QScreen *screen = 0);
    ~Window();

    void setSource( const QUrl& url );
    void setRootObject( QObject* obj );

    enum Status { Null, Ready, Loading, Error };
    Status status() const;

    QSharedPointer<QObject> rootObject() { return m_root; }
    void    registerAspect(AbstractAspect *aspect);

signals:
    void statusChanged( Qt3D::Window::Status );

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent( QKeyEvent* e );

    virtual void mousePressEvent( QMouseEvent* e );
    virtual void mouseReleaseEvent( QMouseEvent* e );
    virtual void mouseMoveEvent( QMouseEvent* e );
    virtual void resizeEvent(QResizeEvent *e);

private slots:
    void continueExecute();

    void onUpdate();

private:

    QScopedPointer<QQmlEngine> m_engine;
    QSharedPointer<QObject> m_root;
    QSharedPointer<QQmlComponent> m_component;

    // The various aspects (subsystems) that will be interested in (parts)
    // of the objects in the object tree.
    QAspectEngine *m_aspectEngine;

    Camera* m_camera;

    // temporary, borrowed from training material
    CameraController* m_controller;

    QTimer* m_updateTimer;
};

} // namespace Qt3D

#endif // QT3D_WINDOW_H
