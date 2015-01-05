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

#ifndef QT3D_QMESHDATA_H
#define QT3D_QMESHDATA_H

#include <Qt3DRenderer/qt3drenderer_global.h>
#include <Qt3DCore/qaxisalignedboundingbox.h>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QAbstractAttribute;
class QAbstractBuffer;
class QMeshDataPrivate;

typedef QSharedPointer<QAbstractAttribute> QAbstractAttributePtr;
typedef QSharedPointer<QAbstractBuffer> QAbstractBufferPtr;

class QT3DRENDERERSHARED_EXPORT QMeshData
{
public:
    enum PrimitiveType {
        Points = 0x0000,
        Lines = 0x0001,
        LineLoop = 0x0002,
        LineStrip = 0x0003,
        Triangles = 0x0004,
        TriangleStrip = 0x0005,
        TriangleFan = 0x0006,
        LinesAdjacency = 0x000A,
        TrianglesAdjacency = 0x000C,
        LineStripAdjacency = 0x000B,
        TriangleStripAdjacency = 0x000D,
        Patches = 0x000E
    };

    explicit QMeshData(PrimitiveType primitiveType = Triangles);
    virtual ~QMeshData();

    void addAttribute(const QString &name, const QAbstractAttributePtr &attr);
    void setIndexAttribute(const QAbstractAttributePtr &attr);

    QStringList attributeNames() const;
    QAbstractAttributePtr attributeByName(const QString &name) const;
    QAbstractAttributePtr indexAttribute() const;

    static const QString defaultPositionAttributeName() { return QStringLiteral("vertexPosition"); }
    static const QString defaultNormalAttributeName() { return QStringLiteral("vertexNormal"); }
    static const QString defaultColorAttributeName() { return QStringLiteral("vertexColor"); }
    static const QString defaultTextureCoordinateAttributeName() { return QStringLiteral("vertexTexCoord"); }
    static const QString defaultTangentAttributeName() { return QStringLiteral("vertexTangent"); }

    void setPrimitiveType(PrimitiveType primitiveType);
    int primitiveType() const;

    void setVerticesPerPatch(int verticesPerPatch);
    int verticesPerPatch() const;

    int primitiveCount() const;

    QList<QAbstractBufferPtr> buffers() const;

    void setBoundingBox(const QAxisAlignedBoundingBox &bbox);
    void computeBoundsFromAttribute(const QString &name);

    QAxisAlignedBoundingBox boundingBox() const;

protected:
    Q_DECLARE_PRIVATE(QMeshData)
    QMeshDataPrivate *d_ptr;
    QMeshData(QMeshDataPrivate &dd);
};

typedef QSharedPointer<QMeshData> QMeshDataPtr;

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QMESHDATA_H
