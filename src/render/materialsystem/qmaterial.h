// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QMATERIAL_H
#define QT3DRENDER_QMATERIAL_H

#include <QtCore/QVariant>

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qabstracttexture.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QParameter;
class QMaterialPrivate;

class Q_3DRENDERSHARED_EXPORT QMaterial : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QEffect *effect READ effect WRITE setEffect NOTIFY effectChanged)

public:
    explicit QMaterial(Qt3DCore::QNode *parent = nullptr);
    ~QMaterial();

    QEffect *effect() const;

    void addParameter(QParameter *parameter);
    void removeParameter(QParameter *parameter);
    QList<QParameter *> parameters() const;

public Q_SLOTS:
    void setEffect(QEffect *effect);

Q_SIGNALS:
    void effectChanged(QEffect *effect);

protected:
    explicit QMaterial(QMaterialPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QMaterial)
};

}

QT_END_NAMESPACE

#endif // QT3DRENDER_QMATERIAL_H
