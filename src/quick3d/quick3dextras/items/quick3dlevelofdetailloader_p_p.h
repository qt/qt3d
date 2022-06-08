// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
