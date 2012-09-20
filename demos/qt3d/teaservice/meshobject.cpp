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

#include "meshobject.h"
#include "qglview.h"

MeshObject::MeshObject(QGLSceneNode *meshObject, QObject *parent)
    : QObject(parent)
{
    m_mesh = 0;
    m_meshObject = meshObject;
    m_scale = 1.0f;
    m_rotationAngle = 0.0f;
    m_effect = 0;
    m_objectId = -1;
    m_hovering = false;
    m_material = 0;
    m_hoverMaterial = 0;
}

MeshObject::MeshObject(QGLAbstractScene *scene, QObject *parent)
    : QObject(parent)
{
    scene->setParent(this);
    m_mesh = 0;
    m_meshObject = scene->mainNode();
    m_scale = 1.0f;
    m_rotationAngle = 0.0f;
    m_effect = 0;
    m_objectId = -1;
    m_hovering = false;
    m_material = 0;
    m_hoverMaterial = 0;
}

MeshObject::~MeshObject()
{
    delete m_mesh;
}

void MeshObject::initialize(QGLView *view, QGLPainter *painter)
{
    Q_UNUSED(painter);
    if (m_objectId != -1)
        view->registerObject(m_objectId, this);
}

void MeshObject::draw(QGLPainter *painter)
{
    // Position the model at its designated position, scale, and orientation.
    painter->modelViewMatrix().push();
    painter->modelViewMatrix().translate(m_position);
    if (m_scale != 1.0f)
        painter->modelViewMatrix().scale(m_scale);
    if (m_rotationAngle != 0.0f)
        painter->modelViewMatrix().rotate(m_rotationAngle, m_rotationVector);

    // Apply the material and effect to the painter.
    QGLMaterial *material;
    if (m_hovering)
        material = m_hoverMaterial;
    else
        material = m_material;
    painter->setColor(material->diffuseColor());
    painter->setFaceMaterial(QGL::AllFaces, material);
    if (m_effect)
        painter->setUserEffect(m_effect);
    else
        painter->setStandardEffect(QGL::LitMaterial);

    // Mark the object for object picking purposes.
    int prevObjectId = painter->objectPickId();
    if (m_objectId != -1)
        painter->setObjectPickId(m_objectId);

    // Draw the geometry mesh.
    if (m_meshObject)
        m_meshObject->draw(painter);
    else
        m_mesh->draw(painter);

    // Turn off the user effect, if present.
    if (m_effect)
        painter->setStandardEffect(QGL::LitMaterial);

    // Revert to the previous object identifier.
    painter->setObjectPickId(prevObjectId);

    // Restore the modelview matrix.
    painter->modelViewMatrix().pop();
}

bool MeshObject::event(QEvent *e)
{
    // Convert the raw event into a signal representing the user's action.
    if (e->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = (QMouseEvent *)e;
        if (me->button() == Qt::LeftButton)
            emit pressed();
    } else if (e->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *me = (QMouseEvent *)e;
        if (me->button() == Qt::LeftButton) {
            emit released();
            if (me->x() >= 0)   // Positive: inside object, Negative: outside.
                emit clicked();
        }
    } else if (e->type() == QEvent::MouseButtonDblClick) {
        emit doubleClicked();
    } else if (e->type() == QEvent::Enter) {
        m_hovering = true;
        emit hoverChanged();
    } else if (e->type() == QEvent::Leave) {
        m_hovering = false;
        emit hoverChanged();
    }
    return QObject::event(e);
}
