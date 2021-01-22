/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

#ifndef QT3DEXTRAS_EXTRAS_QUICK_QUICK3DLEVELOFDETAILLOADER_P_P_H
#define QT3DEXTRAS_EXTRAS_QUICK_QUICK3DLEVELOFDETAILLOADER_P_P_H

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

#include "quick3dlevelofdetailloader_p.h"
#include <Qt3DCore/private/qentity_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {
class Quick3DEntityLoader;
}
}

namespace Qt3DExtras {
namespace Extras {
namespace Quick {

class Quick3DLevelOfDetailLoaderPrivate : public Qt3DCore::QEntityPrivate
{
public:
    Quick3DLevelOfDetailLoaderPrivate();

    Q_DECLARE_PUBLIC(Quick3DLevelOfDetailLoader)

    QVariantList m_sources;
    Qt3DCore::Quick::Quick3DEntityLoader *m_loader;
    Qt3DRender::QLevelOfDetail *m_lod;
};

} // namespace Quick
} // namespace Extras
} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_EXTRAS_QUICK_QUICK3DLEVELOFDETAILLOADER_P_P_H
