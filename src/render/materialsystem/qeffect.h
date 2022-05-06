/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
