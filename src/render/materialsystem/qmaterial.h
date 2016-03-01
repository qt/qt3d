/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_QMATERIAL_H
#define QT3DRENDER_QMATERIAL_H

#include <QVariant>

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractTextureProvider;
class QParameter;
class QMaterialPrivate;
class QEffect;
typedef QMap<QString, QAbstractTextureProvider*> TextureDict;

class QT3DRENDERSHARED_EXPORT QMaterial : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QEffect *effect READ effect WRITE setEffect NOTIFY effectChanged)

public:
    explicit QMaterial(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~QMaterial();

    QEffect *effect() const;

    void addParameter(QParameter *parameter);
    void removeParameter(QParameter *parameter);
    QList<QParameter *> parameters() const;

    TextureDict textureValues() const;

    void setTextureParameter(QString name, QAbstractTextureProvider* tex);

public Q_SLOTS:
    void setEffect(QEffect *effect);

Q_SIGNALS:
    void effectChanged(QEffect *effect);

protected:
    QMaterial(QMaterialPrivate &dd, Qt3DCore::QNode *parent = Q_NULLPTR);
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QMaterial)
    QT3D_CLONEABLE(QMaterial)
};

}

QT_END_NAMESPACE

#endif // QT3DRENDER_QMATERIAL_H
