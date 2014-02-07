/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#ifndef SKYBOX_H
#define SKYBOX_H

#include <QString>
#include <QObject>
#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtQml/qqml.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlparserstatus.h>

QT_BEGIN_NAMESPACE

class QGLSceneNode;
class QGLPainter;
class QGLView;
class QGLCamera;
class Viewport;

class Skybox : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Viewport *viewport READ viewport WRITE setViewport NOTIFY viewportChanged)
public:
    Skybox(QObject *parent = 0);
    ~Skybox();
    QUrl source() const { return m_source; }
    void setSource(const QUrl &url);
    void draw(QGLPainter *painter);
    Viewport *viewport() const
    {
        return m_view;
    }
    void setViewport(Viewport *view);

    void classBegin();
    void componentComplete();

Q_SIGNALS:
    void sourceChanged();
    void viewportChanged();

protected:
    bool event(QEvent *event);

private Q_SLOTS:
    void handleOpenglContextIsAboutToBeDestroyed();

private:
    void scanLocalDir(const QString &imagePath);

    static const char *EXPECTED_NAMES[];

    bool m_ready;
    QGLSceneNode *m_scene;
    QString m_imagePath;
    QUrl m_source;
    QGLSceneNode *m_faces[6];
    Viewport *m_view;
    QGLCamera *m_camera;
    bool m_bConnectedToOpenGLContextSignal;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(Skybox)

#endif // SKYBOX_H
