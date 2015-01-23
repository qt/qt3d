/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_QMESHV2_H
#define QT3D_QMESHV2_H

#include <Qt3DRenderer/qt3drenderer_global.h>
#include <Qt3DRenderer/qgeometryrenderer.h>
#include <QUrl>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QMeshV2Private;

class QT3DRENDERERSHARED_EXPORT QMeshV2 : public QGeometryRenderer
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

public:
    QMeshV2(QNode *parent = 0);
    ~QMeshV2();

    QUrl source() const;
    void setSource(const QUrl &arg);

    // FIXME now this is an attribute provider directly. but we still do want
    // a functor factory for loading the mesh data to load it in a aspect's job?
    // so when THAT happens, how is the loaded mesh data connected to this class?

    // should this class have something like
    // * the usual mesh functor to create mesh data
    // * a protected/private "setMeshData" that sets back the mesh data
    // * and attributes() reading from that mesh data?

Q_SIGNALS:
    void sourceChanged(const QUrl &arg);

protected:
    void copy(const QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(QMeshV2)
    Q_DECLARE_PRIVATE(QMeshV2)
    QT3D_CLONEABLE(QMeshV2)
};

}

QT_END_NAMESPACE

#endif // QT3D_QMESHV2_H
