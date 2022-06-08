// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qurlhelper_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QString QUrlHelper::urlToLocalFileOrQrc(const QUrl &url)
{
    const QString scheme(url.scheme().toLower());
    if (scheme == QLatin1String("qrc")) {
        if (url.authority().isEmpty())
            return QLatin1Char(':') + url.path();
        return QString();
    }

#if defined(Q_OS_ANDROID)
    if (scheme == QLatin1String("assets")) {
        if (url.authority().isEmpty())
            return url.toString();
        return QString();
    }
#endif

    return url.toLocalFile();
}

} // Qt3DCore

QT_END_NAMESPACE
