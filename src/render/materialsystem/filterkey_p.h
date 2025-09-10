// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RENDERFILTERKEY_P_H
#define QT3DRENDER_RENDER_RENDERFILTERKEY_P_H

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

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/qfilterkey.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QFilterKey;

namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT FilterKey : public BackendNode
{
public:
    FilterKey();
    ~FilterKey();
    void cleanup();

    const QVariant &value() const { return m_value; }
    const QString &name() const { return m_name; }
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    friend bool operator==(const FilterKey &lhs, const FilterKey &rhs)
    { return lhs.equals(rhs); }
    friend bool operator !=(const FilterKey &lhs, const FilterKey &rhs)
    { return !lhs.equals(rhs); }

private:
    bool equals(const FilterKey &other) const;

    QVariant m_value;
    QString m_name;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERFILTERKEY_P_H
