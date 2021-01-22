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

#ifndef QT3DCORE_QIDTYPEVISITOR_H
#define QT3DCORE_QIDTYPEVISITOR_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/private/qentity_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QDestructionIdAndTypeCollector
{
public:
    QDestructionIdAndTypeCollector(QNode *rootNode);

    QVector<QNodeIdTypePair> subtreeIdsAndTypes() const { return m_subtreeIdsAndTypes; }

private:
    void collectIdAndType(QNode *node)
    {
        QNodeIdTypePair idAndType(node->id(), QNodePrivate::get(node)->m_typeInfo);
        m_subtreeIdsAndTypes.push_back(idAndType);

        // Mark this node as having been handled for destruction so we don't
        // repeat it unnecessarily in an O(n^2) manner
        QNodePrivate::get(node)->m_hasBackendNode = false;
    }

    QVector<QNodeIdTypePair> m_subtreeIdsAndTypes;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QIDTYPEVISITOR_H
