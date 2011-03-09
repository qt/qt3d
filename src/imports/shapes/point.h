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

#ifndef POINT_H
#define POINT_H

#include "qdeclarativeitem3d.h"
#include "qglscenenode.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Point : public QDeclarativeItem3D
{
    Q_OBJECT
    Q_PROPERTY(qreal pointSize READ pointSize WRITE setPointSize NOTIFY pointSizeChanged)
    Q_PROPERTY(QVariant vertices READ vertices WRITE setVertices NOTIFY verticesChanged)

public:
    explicit Point(QObject *parent = 0);
    ~Point() {}

    QVariant vertices() const;
    void setVertices(const QVariant &value);

    qreal pointSize() const {return m_pointSize;}
    void setPointSize(qreal pointSize);

Q_SIGNALS:
    void verticesChanged();
    void pointSizeChanged();

protected:
    void drawItem(QGLPainter *painter);

private:
    qreal m_pointSize;
    QVariant m_vertices;
    QVector3DArray m_vertexArray;
    QGLSceneNode * m_geometry;
    bool m_changeFlag;
};

QML_DECLARE_TYPE(Point)

QT_END_NAMESPACE

QT_END_HEADER

#endif // POINT_H
