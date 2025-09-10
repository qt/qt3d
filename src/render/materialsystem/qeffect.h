// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QEFFECT_H
#define QT3DRENDER_QEFFECT_H

#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QParameter;
class QTechnique;
class QEffectPrivate;

class Q_3DRENDERSHARED_EXPORT QEffect : public Qt3DCore::QNode
{
    Q_OBJECT
public:
    explicit QEffect(Qt3DCore::QNode *parent = nullptr);
    ~QEffect();

    void addParameter(QParameter *parameter);
    void removeParameter(QParameter *parameter);
    QList<QParameter *> parameters() const;

    void addTechnique(QTechnique *t);
    void removeTechnique(QTechnique *t);
    QList<QTechnique *> techniques() const;

protected:
    explicit QEffect(QEffectPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QEffect)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QEFFECT_H
