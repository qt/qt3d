/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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
****************************************************************************/

#ifndef QT3DCORE_QNODEDESTROYEDCHANGE_H
#define QT3DCORE_QNODEDESTROYEDCHANGE_H

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qscenechange.h>
#include <QtCore/qsharedpointer.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QNodeDestroyedChangePrivate;

class Q_3DCORESHARED_EXPORT QNodeDestroyedChange : public QSceneChange
{
public:
    Q3D_DECL_DEPRECATED explicit QNodeDestroyedChange(const QNode *node, const QVector<QNodeIdTypePair> &subtreeIdsAndTypes);
    ~QNodeDestroyedChange();

    QVector<QNodeIdTypePair> subtreeIdsAndTypes() const;

private:
    Q_DECLARE_PRIVATE(QNodeDestroyedChange)
};

Q3D_DECL_DEPRECATED typedef QSharedPointer<QNodeDestroyedChange> QNodeDestroyedChangePtr;

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QNODEDESTROYEDCHANGE_H
