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

#ifndef LINE_H
#define LINE_H

#include <Qt3DQuick/qquickitem3d.h>

#include <Qt3D/qglscenenode.h>

QT_BEGIN_NAMESPACE

class Q_QT3D_QUICK_EXPORT Line : public QQuickItem3D
{
    Q_OBJECT
    Q_PROPERTY(float width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(QVariant vertices READ vertices WRITE setVertices NOTIFY verticesChanged)

public:
    explicit Line(QObject *parent = 0);
    ~Line();

    QVariant vertices() const;
    void setVertices(const QVariant &value);

    float width() const {return m_width;}
    void setWidth(float width);

Q_SIGNALS:
    void verticesChanged();
    void widthChanged();

protected:
    void drawItem(QGLPainter *painter);

private:
    float m_width;
    QVariant m_vertices;
    QVector3DArray m_vertexArray;
    QGLSceneNode * m_geometry;
    bool m_changeFlag;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(Line)

#endif // LINE_H
