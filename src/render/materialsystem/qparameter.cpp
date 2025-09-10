// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qparameter.h"
#include "qparameter_p.h"
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/qtexture.h>
#include <QtCore/qiterable.h>
#include <QtCore/qsequentialiterable.h>

/*!
    \qmltype Parameter
    \nativetype Qt3DRender::QParameter
    \inqmlmodule Qt3D.Render
    \brief Provides storage for a name and value pair. This maps to a shader uniform.

    A Parameter can be referenced by a RenderPass, Technique, Effect, Material,
    TechniqueFilter, RenderPassFilter. At runtime, depending on which shader is
    selected for a given step of the rendering, the value contained in a
    Parameter will be converted and uploaded if the shader contains a uniform
    with a name matching that of the Parameter.

    \code
    Parameter {
        name: "diffuseColor"
        value: "blue"
    }

    // Works with the following GLSL uniform shader declarations
    // uniform vec4 diffuseColor;
    // uniform vec3 diffuseColor;
    // uniform vec2 diffuseColor;
    // uniform float diffuseColor;
    \endcode

    \note some care must be taken to ensure the value wrapped by a Parameter
    can actually be converted to what the real uniform expect. Giving a value
    stored as an int where the actual shader uniform is of type float could
    result in undefined behaviors.

    \note when the targeted uniform is an array, the name should be the name
    of the uniform with [0] appended to it.

    \note Parameter node can not be disabled.

    \code
    Parameter {
        name: "diffuseValues[0]"
        value: [0.0, 1.0. 2.0, 3.0, 4.0, 883.0, 1340.0, 1584.0]
    }

    // Matching GLSL shader uniform  declaration
    // uniform float diffuseValues[8];
    \endcode

    When it comes to texture support, the Parameter value should be set to the
    appropriate \l {Qt3DRender::QAbstractTexture}{texture} subclass that matches the sampler type of the shader
    uniform.

    \code
    Parameter {
        name: "diffuseTexture"
        value: Texture2D { ... }
    }

    // Works with the following GLSL uniform shader declaration
    // uniform sampler2D diffuseTexture
    \endcode

    \sa Qt3DRender::QAbstractTexture
 */

/*!
    \class Qt3DRender::QParameter
    \inheaderfile Qt3DRender/QParameter
    \inmodule Qt3DRender
    \brief Provides storage for a name and value pair. This maps to a shader uniform.

    A QParameter can be referenced by a QRenderPass, QTechnique, QEffect, QMaterial,
    QTechniqueFilter, QRenderPassFilter. At runtime, depending on which shader is
    selected for a given step of the rendering, the value contained in a
    QParameter will be converted and uploaded if the shader contains a uniform
    with a name matching that of the QParameter.

    \code
    QParameter *param = new QParameter();
    param->setName(QStringLiteral("diffuseColor"));
    param->setValue(QColor::fromRgbF(0.0f, 0.0f, 1.0f, 1.0f));

    // Alternatively you can create and set a QParameter this way
    QParameter *param2 = new QParameter(QStringLiteral("diffuseColor"), QColor::fromRgbF(0.0f, 0.0f, 1.0f, 1.0f));

    // Such QParameters will work with the following GLSL uniform shader declarations
    // uniform vec4 diffuseColor;
    // uniform vec3 diffuseColor;
    // uniform vec2 diffuseColor;
    // uniform float diffuseColor;
    \endcode

    \note some care must be taken to ensure the value wrapped by a QParameter
    can actually be converted to what the real uniform expect. Giving a value
    stored as an int where the actual shader uniform is of type float could
    result in undefined behaviors.

    \note when the targeted uniform is an array, the name should be the name
    of the uniform with [0] appended to it.

    \note QParameter node can not be disabled.

    \code
    QParameter *param = new QParameter();
    QVariantList values = QVariantList() << 0.0f << 1.0f << 2.0f << 3.0f << 4.0f << 883.0f << 1340.0f << 1584.0f;

    param->setName(QStringLiteral("diffuseValues[0]"));
    param->setValue(values);

    // Matching GLSL shader uniform  declaration
    // uniform float diffuseValues[8];
    \endcode

    When it comes to texture support, the QParameter value should be set to the
    appropriate QAbstractTexture subclass that matches the sampler type of the shader
    uniform.

    \code
    QTexture2D *texture = new QTexture2D();
    ...
    QParameter *param = new QParameter();
    param->setName(QStringLiteral("diffuseTexture"));
    param->setValue(QVariant::fromValue(texture));

    // Works with the following GLSL uniform shader declaration
    // uniform sampler2D diffuseTexture
    \endcode

    \sa QAbstractTexture
 */

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QParameterPrivate::QParameterPrivate()
    : QNodePrivate()
{
}

namespace {

/*! \internal */
inline QVariant toBackendValue(const QVariant &v)
{
    if (auto nodeValue = v.value<Qt3DCore::QNode*>())
        return QVariant::fromValue(nodeValue->id());
    return v;
}

} // anonymous

void QParameterPrivate::setValue(const QVariant &v)
{
    if (v.metaType().id() == QMetaType::QVariantList) {
        QSequentialIterable iterable = v.value<QSequentialIterable>();
        QVariantList variants;
        variants.reserve(iterable.size());
        for (const auto &v : iterable)
            variants.append(toBackendValue(v));
        m_backendValue = variants;
    } else {
        m_backendValue = toBackendValue(v);
    }
    m_value = v;
}

/*! \internal */
QParameter::QParameter(QParameterPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

/*!
  \fn Qt3DRender::QParameter::QParameter(Qt3DCore::QNode *parent)
  Constructs a new QParameter with the specified \a parent.
 */
QParameter::QParameter(QNode *parent)
    : QNode(*new QParameterPrivate, parent)
{
}

/*!
  \fn Qt3DRender::QParameter::QParameter(const QString &name, const QVariant &value, QNode *parent)
  Constructs a new QParameter with the specified \a parent \a name and \a value.
 */
QParameter::QParameter(const QString &name, const QVariant &value, QNode *parent)
    : QNode(*new QParameterPrivate, parent)
{
    Q_D(QParameter);
    d->m_name = name;
    setValue(value);
}

/*!
  \fn Qt3DRender::QParameter::QParameter(const QString &name, QAbstractTexture *texture, QNode *parent)
  Constructs a new QParameter with the specified \a parent \a name and takes its value from \a texture.
 */
QParameter::QParameter(const QString &name, QAbstractTexture *texture, QNode *parent)
    : QNode(*new QParameterPrivate, parent)
{
    Q_D(QParameter);
    d->m_name = name;
    setValue(QVariant::fromValue(texture));
}

/*! \internal */
QParameter::~QParameter()
{
}

/*!
  \qmlproperty string Qt3D.Render::Parameter::name
    Specifies the name of the parameter
*/

/*!
  \property Qt3DRender::QParameter::name
    Specifies the name of the parameter
 */
void QParameter::setName(const QString &name)
{
    Q_D(QParameter);
    if (d->m_name != name) {
        d->m_name = name;
        emit nameChanged(name);
    }
}

QString QParameter::name() const
{
    Q_D(const QParameter);
    return d->m_name;
}

/*!
  \qmlproperty QVariant Qt3D.Render::Parameter::value
    Specifies the value of the parameter
*/

/*!
  \property Qt3DRender::QParameter::value
    Specifies the value of the parameter
 */
void QParameter::setValue(const QVariant &dv)
{
    Q_D(QParameter);
    if (d->m_value != dv) {

        QNode *oldNodeValue = d->m_value.value<QNode *>();
        if (oldNodeValue != nullptr)
            d->unregisterDestructionHelper(oldNodeValue);

        // In case node values are declared inline
        QNode *nodeValue = dv.value<QNode *>();
        if (nodeValue != nullptr && !nodeValue->parent())
            nodeValue->setParent(this);

        d->setValue(dv);

        // Ensures proper bookkeeping
        if (nodeValue != nullptr)
            d->registerDestructionHelper(nodeValue, &QParameter::setValue, nodeValue, QVariant());

        emit valueChanged(dv);
    }
}

QVariant QParameter::value() const
{
    Q_D(const QParameter);
    return d->m_value;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qparameter.cpp"
