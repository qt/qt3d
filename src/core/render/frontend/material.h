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

#ifndef MATERIAL_H
#define MATERIAL_H

#include <QVariant>

#include <component.h>

// FIXME - write a custom QML parser and stop mis-using Tag
#include "tag.h"

#include <qt3drenderer_global.h>

namespace Qt3D {

class Effect;
class Texture;

class QT3DRENDERERSHARED_EXPORT Material : public Component
{
    Q_OBJECT
    Q_PROPERTY(Qt3D::Effect* effect READ effect WRITE setEffect NOTIFY effectChanged)
    Q_PROPERTY(QQmlListProperty<Qt3D::Tag> parameters READ qmlParameters)

public:
    explicit Material(Node *parent = 0);

    Qt3D::Effect *effect() const;
    void setEffect(Qt3D::Effect *effect);

    void setParameter(QString name, QVariant val);

    QVariantMap parameterValues() const;

    typedef QMap<QString, Texture*> TextureDict;
    TextureDict textureValues() const;

    void setTextureParameter(QString name, Texture* tex);

signals:
    void effectChanged();
    void parametersChanged();

private slots:
    void onTagValueChanged();

private:
    QQmlListProperty<Tag> qmlParameters();

    // FIXME - remove when we have a custom QML parser
    static void appendTag(QQmlListProperty<Tag> *list, Tag *bar);
    static Tag *tagAt(QQmlListProperty<Tag> *list, int index);
    static int tagCount(QQmlListProperty<Tag> *list);
    static void clearTags(QQmlListProperty<Tag> *list);


    Effect *m_effect;
    QVariantMap m_parameters;

    // FIXME - remove with tags
    QList<Tag *> m_tagList;

    TextureDict m_textures;
};

}

#endif // MATERIAL_H
