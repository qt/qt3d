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

#include "mesh.h"

#include <qchangearbiter.h>

#include <QDebug>
#include <QFile>
#include <QFileInfo>

#include <objloader.h>

namespace Qt3D {

Mesh::Mesh(Node *parent)
    : Component( parent )
    , m_sourceDirty( false )
{
}

Mesh::~Mesh()
{
}

void Mesh::setSource( const QString& source )
{
    if ( source == m_source )
        return;
    m_source = source;
    m_sourceDirty = true;

    // FIXME - synchronous update
    update();

    // Let aspects know about the change
    QScenePropertyChangePtr e(new QScenePropertyChange(Qt3D::MeshChange, this));
    e->m_propertyName = QByteArrayLiteral("source");
    e->m_value = m_source;
    notifySceneChange(e);

    emit sourceChanged();
}

QString Mesh::source() const
{
    return m_source;
}

MeshDataPtr Mesh::data() const
{
    return m_data;
}

void Mesh::setData(MeshDataPtr d)
{
    m_data = d;
}

void Mesh::update()
{
    if (m_sourceDirty) {
        m_data = MeshDataPtr();
        m_sourceDirty = false;

        QFileInfo finfo(m_source);
        if (!finfo.exists()) {
            qWarning() << Q_FUNC_INFO << "not found:" << m_source;
            return;
        }

        QFile f(m_source);
        f.open(QIODevice::ReadOnly);

        ObjLoader objLoad;
        objLoad.setLoadTextureCoordinatesEnabled(true);

        if (!objLoad.load(&f)) {
            qWarning() << Q_FUNC_INFO << "OBJ load failure for:" << m_source;
            return;
        }

        qDebug() << "Loaded OBJ ok";
        setData(objLoad.mesh());
        setObjectName("OBJ");
    }

    m_sourceDirty = false;
}

} // of namespace Qt3D
