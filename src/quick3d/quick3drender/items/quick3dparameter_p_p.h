// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_QUICK_QUICK3DPARAMETER_P_P_H
#define QT3DRENDER_RENDER_QUICK_QUICK3DPARAMETER_P_P_H

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

#include <Qt3DQuickRender/private/quick3dparameter_p.h>
#include <Qt3DRender/private/qparameter_p.h>


QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

class Quick3DParameter;

class Quick3DParameterPrivate : public QParameterPrivate
{
public:
    Quick3DParameterPrivate();

    Q_DECLARE_PUBLIC(Quick3DParameter)

    void setValue(const QVariant &value) override;
};

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QUICK_QUICK3DPARAMETER_P_P_H
