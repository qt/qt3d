/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef LINE_H
#define LINE_H

#include "qdeclarativeitem3d.h"
#include "qglscenenode.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Line : public QDeclarativeItem3D
{
    Q_OBJECT
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(QVariant vertices READ vertices WRITE setVertices NOTIFY verticesChanged)

public:
    explicit Line(QObject *parent = 0);
    ~Line() {}

    QVariant vertices() const;
    void setVertices(const QVariant &value);

    qreal width() const {return m_width;}
    void setWidth(qreal width);

Q_SIGNALS:
    void verticesChanged();
    void widthChanged();

protected:
    void drawItem(QGLPainter *painter);

private:
    qreal m_width;
    QVariant m_vertices;
    QVector3DArray m_vertexArray;
    QGLSceneNode * m_geometry;
    bool m_changeFlag;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(Line)

QT_END_HEADER

#endif // LINE_H
