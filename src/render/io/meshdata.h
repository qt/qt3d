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

#ifndef MESHDATA_H
#define MESHDATA_H

#include <QSharedPointer>
#include <QList>
#include <QMap>
#include <QOpenGLBuffer>
#include <QVector2D>

#include <axisalignedboundingbox.h>


namespace Qt3D
{

GLint elementType(GLint type);
GLint tupleSizeFromType(GLint type);
GLuint byteSizeFromType(GLint type);

class Buffer
{
public:
    Buffer(QOpenGLBuffer::Type ty);

    void setUsage(QOpenGLBuffer::UsagePattern usage);

    void setData(QByteArray bytes);
    QByteArray data() const;

    QOpenGLBuffer::Type type() const
    { return m_type; }

    void bind();

    QOpenGLBuffer createGL() const;
    void upload(QOpenGLBuffer b);

    // make a QObject and signal when contents change?
    // GraphicsContext could listen, orphan the QOpenGLBuffer and hence
    // reupload next time it's need
private:
    const QOpenGLBuffer::Type m_type;
    QOpenGLBuffer::UsagePattern m_usage;
    QByteArray m_clientSideBytes;
};

typedef QSharedPointer<Buffer> BufferPtr;

class Attribute
{
public:
    Attribute(BufferPtr buf, int type, int count, int offset=0, int stride = 0);

    void setDivisor(unsigned int divisor);

    unsigned int divisor() const
    { return m_divisor; }

    BufferPtr buffer() const;

    int type() const
    { return m_type; }

    unsigned int count() const
    { return m_count; }

    unsigned int byteStride() const
    { return m_stride; }

    unsigned int byteOffset() const
    { return m_offset; }

    QVector<QVector3D> asVector3D() const;
    QVector<QVector2D> asVector2D() const;

    void dump(int count);
private:
    BufferPtr m_buffer;
    int m_type, m_count;
    unsigned int m_stride, m_offset; // both in bytes
    // AxisAlignedBoundBox m_range;
    unsigned int m_divisor;
};

typedef QSharedPointer<Attribute> AttributePtr;

/**
 * @brief The MeshData class is shared by all instances of a RenderMesh,
 * and holds the actual client (CPU)-side buffers representing mesh attributes
 * and indices.
 */
class MeshData
{
public:
    MeshData();
    explicit MeshData(int primitiveType);

    void addAttribute(QString name, AttributePtr attr);

    // permit inline 'new' call, will take ownership
    void addAttribute(QString name, Attribute* attr);

    void setIndexAttr(AttributePtr indexAttr);
    void setIndexData(BufferPtr buf, int type, int count, int offset = 0);

    GLint primitiveType() const;
    GLsizei primitiveCount() const;

    QStringList attributeNames() const;

    AttributePtr attributeByName(QString nm) const;

    AttributePtr indexAttr() const;

    QString materialName() const;

    QList<BufferPtr> buffers() const;

    // specify the bounding box explicitly
    void setBoundingBox(const AxisAlignedBoundingBox& bbox);

    void computeBoundsFromAttribute(QString name);

    AxisAlignedBoundingBox boundingBox() const
    { return m_box; }
private:
    QMap<QString, AttributePtr> m_attributes;
    QString m_materialName;

    int m_primitiveType;
    AttributePtr m_indexAttr;

    AxisAlignedBoundingBox m_box;
};

typedef QSharedPointer<MeshData> MeshDataPtr;

}

#endif // MESHDATA_H
