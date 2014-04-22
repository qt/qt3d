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

#include "material.h"

#include <qchangearbiter.h>
#include <texture.h>

#include "renderlogging.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

Material::Material(Node *parent)
    : Component(parent),
      m_effect(0)
{
}

Qt3D::Effect *Material::effect() const
{
    return m_effect;
}

void Material::setEffect(Qt3D::Effect *effect)
{
    if (effect == m_effect)
        return;

    m_effect = effect;
    emit effectChanged();
}

void Material::setParameter(QString name, QVariant val)
{
    m_parameters[name] = val;

    // schedule update to the backend
    QScenePropertyChangePtr change(new QScenePropertyChange(MaterialParameter, this));
    change->m_propertyName = name.toLocal8Bit();
    change->m_value = val;
    notifySceneChange(change);
}

QVariantMap Material::parameterValues() const
{
    return m_parameters;
}

Material::TextureDict Material::textureValues() const
{
    return m_textures;
}

void Material::setTextureParameter(QString name, Texture *tex)
{
    m_textures[name] = tex;
}

void Material::onTagValueChanged()
{
    Tag* t = qobject_cast<Tag*>(sender());
    Q_ASSERT(m_tagList.contains(t));

    QVariant v = t->value();
    QmlTexture* qmlTex = v.value<QmlTexture*>();
    if (qmlTex) {
        qCDebug(Render::Frontend) << "got texture parameter" << t->name();
        setTextureParameter(t->name(), qmlTex->texture());
    } else {
        setParameter(t->name(), t->value());
    }
}

QQmlListProperty<Tag> Material::qmlParameters()
{
    return QQmlListProperty<Tag>(this, 0,
                                       &Material::appendTag,
                                       &Material::tagCount,
                                       &Material::tagAt,
                                       &Material::clearTags );
}

void Material::appendTag(QQmlListProperty<Tag> *list, Tag *tag)
{
    Material *mat = qobject_cast<Material *>(list->object);
    if (mat) {
        tag->setParent(mat);
        connect(tag, SIGNAL(valueChanged()), mat, SLOT(onTagValueChanged()));
        mat->m_tagList.append(tag);
        emit mat->parametersChanged();
    }
}

Tag *Material::tagAt(QQmlListProperty<Tag> *list, int index)
{
    Material *mat = qobject_cast<Material *>(list->object);
    if (mat)
        return mat->m_tagList.value(index);
    return 0;
}

int Material::tagCount(QQmlListProperty<Tag> *list)
{
    Material *mat = qobject_cast<Material *>(list->object);
    if (mat)
        return mat->m_tagList.size();
    return 0;
}

void Material::clearTags(QQmlListProperty<Tag> *list)
{
    Material *mat = qobject_cast<Material *>(list->object);
    if (mat) {
        mat->m_parameters.clear();
        mat->m_tagList.clear();
        emit mat->parametersChanged();
    }
}

} // namespace Qt3D

QT_END_NAMESPACE
