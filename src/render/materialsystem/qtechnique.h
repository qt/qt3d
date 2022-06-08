// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
