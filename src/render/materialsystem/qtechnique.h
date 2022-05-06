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

#ifndef QT3DRENDER_QTECHNIQUE_H
#define QT3DRENDER_QTECHNIQUE_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qfilterkey.h>
#include <QtCore/QList>
#include <QtCore/QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QParameter;
class QTechniquePrivate;
class QGraphicsApiFilter;

class Q_3DRENDERSHARED_EXPORT QTechnique : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QGraphicsApiFilter *graphicsApiFilter READ graphicsApiFilter CONSTANT)

public:
    explicit QTechnique(Qt3DCore::QNode *parent = nullptr);
    ~QTechnique();

    void addFilterKey(QFilterKey *filterKey);
    void removeFilterKey(QFilterKey *filterKey);
    QList<QFilterKey*> filterKeys() const;

    void addParameter(QParameter *p);
    void removeParameter(QParameter *p);
    QList<QParameter *> parameters() const;

    void addRenderPass(QRenderPass *pass);
    void removeRenderPass(QRenderPass *pass);
    QList<QRenderPass *> renderPasses() const;

    QGraphicsApiFilter *graphicsApiFilter();
    const QGraphicsApiFilter *graphicsApiFilter() const;

protected:
    explicit QTechnique(QTechniquePrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QTechnique)
    Q_PRIVATE_SLOT(d_func(), void _q_graphicsApiFilterChanged())
};

}

QT_END_NAMESPACE

#endif // QT3DRENDER_QTECHNIQUE_H
