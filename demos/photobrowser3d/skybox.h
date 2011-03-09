/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SKYBOX_H
#define SKYBOX_H

#include <QString>
#include <QObject>

class QGLBuilder;
class QGLSceneNode;
class QGLPainter;
class QGLView;
class QGLCamera;

class SkyBox : public QObject
{
    Q_OBJECT
public:
    SkyBox(QGLView *view, const QString &imagePath = QString());
    QString imagePath() const { return m_imagePath; }
    void setImagePath(const QString &imagePath);
    void draw(QGLPainter *painter) const;
private:
    QGLSceneNode *m_scene;
    QString m_imagePath;
    QGLSceneNode *m_faces[6];
    QGLView *m_view;
    QGLCamera *m_camera;
};

#endif // SKYBOX_H
