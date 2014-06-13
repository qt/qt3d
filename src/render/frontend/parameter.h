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

#ifndef QT3D_PARAMETER_H
#define QT3D_PARAMETER_H

#include <QObject>
#include <Qt3DRenderer/qt3drenderer_global.h>
#include <Qt3DRenderer/quniformvalue.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QT3DRENDERERSHARED_EXPORT Parameter : public QObject
{
    Q_OBJECT
    Q_ENUMS(OpenGLTypes)
    Q_ENUMS(StandardUniform)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString meshAttributeName READ meshAttributeName WRITE setMeshAttributeName NOTIFY meshAttributeNameChanged)
    Q_PROPERTY(OpenGLTypes datatype READ datatype WRITE setDatatype NOTIFY datatypeChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(StandardUniform standardUniform WRITE setStandardUniform READ standardUniform CONSTANT)

public:
    // FIXME - sort this by frequency, to minimize the size of the
    // vector in RenderShader. (We want to use compact storage, but we index
    // by this enum, and resize to the largest value)
    enum StandardUniform
    {
        None = -1,

        ModelMatrix = 0,
        ViewMatrix,
        ProjectionMatrix,
        ModelView,
        ModelViewProjection,

        ModelInverse,
        ViewInverse,
        ProjectionInverse,
        ModelViewInverse,
        ModelViewProjectionInverse,

        ModelNormal,
        ModelViewNormal
    };

    enum OpenGLTypes
    {
        Undefined = 0,
        Sampler1D,
        Sampler2D,
        Sampler3D,
        SamplerCube,
        Bool,
        BoolVec2,
        BoolVec3,
        BoolVec4,
        Double,
        DoubleVec2,
        DoubleVec3,
        DoubleVec4,
        Float,
        FloatVec2,
        FloatVec3,
        FloatVec4,
        FloatMat2,
        FloatMat3,
        FloatMat4,
        Int,
        IntVec2,
        IntVec3,
        IntVec4
    };

    explicit Parameter(QObject *parent = 0);
    Parameter(QObject* parent, const QString& name, OpenGLTypes ty);
    Parameter(QObject* parent, const QString& name, OpenGLTypes ty, const QVariant& value);

    void setName(const QString &name);
    QString name() const
    { return m_name; }

    // permit one extra level of indrection in mesh naming of
    // attributes (glTf at least does this)
    void setMeshAttributeName(QString name);
    QString meshAttributeName() const
    { return m_meshName; }

    bool isStandardUniform() const;

    void setStandardUniform(StandardUniform su);
    StandardUniform standardUniform() const;

    /**
     * @brief setDefaultValue - for non-texture uniform parameters
     * @param dv
     */
    void setValue(const QVariant& dv);
    QVariant value() const;

    OpenGLTypes datatype() const
    { return m_type; }
    void setDatatype(OpenGLTypes type);

    bool isTextureType() const;

    /**
     * @brief uniformType - map the data type to the primitive uniform type
     * @return
     */
    Render::QUniformValue::Type uniformType() const;
Q_SIGNALS:
    void valueChanged();
    void nameChanged();
    void meshAttributeNameChanged();
    void datatypeChanged();

private:
    QString m_name;
    OpenGLTypes m_type;
    QVariant m_value;
    QString m_meshName;
    StandardUniform m_standardUniform;
};

} // Qt3D

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3D::Parameter *)

#endif // QT3D_PARAMETER_H
