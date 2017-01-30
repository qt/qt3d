/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DEXTRAS_QMETALROUGHMATERIAL_H
#define QT3DEXTRAS_QMETALROUGHMATERIAL_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qmaterial.h>
#include <QtGui/qcolor.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QAbstractTexture;
}

namespace Qt3DExtras {

class QMetalRoughMaterialPrivate;

class QT3DEXTRASSHARED_EXPORT QMetalRoughMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT
    Q_PROPERTY(QColor baseColor READ baseColor WRITE setBaseColor NOTIFY baseColorChanged)
    Q_PROPERTY(float metallic READ metallic WRITE setMetallic NOTIFY metallicChanged)
    Q_PROPERTY(float roughness READ roughness WRITE setRoughness NOTIFY roughnessChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *environmentIrradiance READ environmentIrradiance WRITE setEnvironmentIrradiance NOTIFY environmentIrradianceChanged)
    Q_PROPERTY(Qt3DRender::QAbstractTexture *environmentSpecular READ environmentSpecular WRITE setEnvironmentSpecular NOTIFY environmentSpecularChanged)
    Q_PROPERTY(float exposure READ exposure WRITE setExposure NOTIFY exposureChanged)

public:
    explicit QMetalRoughMaterial(Qt3DCore::QNode *parent = nullptr);
    ~QMetalRoughMaterial();

    QColor baseColor() const;
    float metallic() const;
    float roughness() const;
    Qt3DRender::QAbstractTexture *environmentIrradiance() const;
    Qt3DRender::QAbstractTexture *environmentSpecular() const;
    float exposure() const;

public Q_SLOTS:
    void setBaseColor(const QColor &baseColor);
    void setMetallic(float metallic);
    void setRoughness(float roughness);
    void setEnvironmentIrradiance(Qt3DRender::QAbstractTexture *environmentIrradiance);
    void setEnvironmentSpecular(Qt3DRender::QAbstractTexture *environmentSpecular);
    void setExposure(float exposure);

Q_SIGNALS:
    void baseColorChanged(const QColor &baseColor);
    void metallicChanged(float metallic);
    void roughnessChanged(float roughness);
    void environmentIrradianceChanged(Qt3DRender::QAbstractTexture *environmentIrradiance);
    void environmentSpecularChanged(Qt3DRender::QAbstractTexture *environmentSpecular);
    void exposureChanged(float exposure);

protected:
    QMetalRoughMaterial(QMetalRoughMaterialPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QMetalRoughMaterial)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QMETALROUGHMATERIAL_H
