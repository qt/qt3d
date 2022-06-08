// Copyright (C) 2017 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OBJGEOMETRYLOADER_H
#define OBJGEOMETRYLOADER_H

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

#include "basegeometryloader_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

#define OBJGEOMETRYLOADER_EXT QLatin1String("obj")

class ObjGeometryLoader : public BaseGeometryLoader
{
protected:
    bool doLoad(QIODevice *ioDev, const QString &subMesh) final;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // OBJGEOMETRYLOADER_H
