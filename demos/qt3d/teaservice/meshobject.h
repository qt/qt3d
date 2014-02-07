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

#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include <QtCore/qobject.h>
#include <QtGui/qevent.h>

#include "qglpainter.h"
#include "qglabstractscene.h"

QT_BEGIN_NAMESPACE
class QGLView;
class QGLSceneNode;
QT_END_NAMESPACE

class MeshObject : public QObject
{
    Q_OBJECT
public:
    explicit MeshObject(QGLSceneNode *meshObject, QObject *parent=0);
    explicit MeshObject(QGLAbstractScene *scene, QObject *parent=0);
    virtual ~MeshObject();

    QVector3D position() const { return m_position; }
    void setPosition(const QVector3D& value) { m_position = value; }

    float scale() const { return m_scale; }
    void setScale(float value) { m_scale = value; }

    float rotationAngle() const { return m_rotationAngle; }
    void setRotationAngle(float value) { m_rotationAngle = value; }

    QVector3D rotationVector() const { return m_rotationVector; }
    void setRotationVector(const QVector3D& value) { m_rotationVector = value; }

    QGLMaterial *material() const { return m_material; }
    void setMaterial(QGLMaterial *value)
        { m_material = value; m_hoverMaterial = value; }

    QGLMaterial *hoverMaterial() const { return m_hoverMaterial; }
    void setHoverMaterial(QGLMaterial *value) { m_hoverMaterial = value; }

    QGLAbstractEffect *effect() const { return m_effect; }
    void setEffect(QGLAbstractEffect *value) { m_effect = value; }

    int objectId() const { return m_objectId; }
    void setObjectId(int id) { m_objectId = id; }

    void initialize(QGLView *view, QGLPainter *painter);
    void draw(QGLPainter *painter);

signals:
    void pressed();
    void released();
    void clicked();
    void doubleClicked();
    void hoverChanged();

protected:
    bool event(QEvent *e);

private:
    QGLSceneNode *m_mesh;
    QGLSceneNode *m_meshObject;
    QGLAbstractScene *m_scene;
    QVector3D m_position;
    float m_scale;
    float m_rotationAngle;
    QVector3D m_rotationVector;
    QGLMaterial *m_material;
    QGLMaterial *m_hoverMaterial;
    QGLAbstractEffect *m_effect;
    int m_objectId;
    bool m_hovering;
};

#endif
