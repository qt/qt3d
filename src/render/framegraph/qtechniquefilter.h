// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QTECHNIQUEFILTER_H
#define QT3DRENDER_QTECHNIQUEFILTER_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qframegraphnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTechniqueFilterPrivate;
class QFilterKey;
class QParameter;

class Q_3DRENDERSHARED_EXPORT QTechniqueFilter : public QFrameGraphNode
{
    Q_OBJECT
public:
    explicit QTechniqueFilter(Qt3DCore::QNode *parent = nullptr);
    ~QTechniqueFilter();

    QList<QFilterKey *> matchAll() const;
    void addMatch(QFilterKey *filterKey);
    void removeMatch(QFilterKey *filterKey);

    void addParameter(QParameter *p);
    void removeParameter(QParameter *p);
    QList<QParameter *> parameters() const;

protected:
    QTechniqueFilter(QTechniqueFilterPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QTechniqueFilter)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QTECHNIQUEFILTER_H
