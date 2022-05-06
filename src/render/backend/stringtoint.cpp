/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
******************************************************************************/

#include "stringtoint_p.h"
#include <QHash>
#include <mutex>
#include <shared_mutex>
#include <vector>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace {

struct StringToIntCache
{
    std::shared_mutex lock;
    QHash<QString, int> map;
    std::vector<QString> reverseMap;

    static StringToIntCache& instance()
    {
        static StringToIntCache c;
        return c;
    }
};

} // anonymous

int StringToInt::lookupId(QLatin1String str)
{
    // ### optimize me
    return lookupId(QString(str));
}

int StringToInt::lookupId(const QString &str)
{
    auto& cache = StringToIntCache::instance();
    int idx;
    {
        std::shared_lock readLocker(cache.lock);
        idx = cache.map.value(str, -1);
    }

    if (Q_UNLIKELY(idx < 0)) {
        std::unique_lock writeLocker(cache.lock);
        idx = cache.map.value(str, -1);
        if (idx < 0) {
            idx = int(cache.reverseMap.size());
            Q_ASSERT(size_t(cache.map.size()) == cache.reverseMap.size());
            cache.map.insert(str, idx);
            cache.reverseMap.push_back(str);
        }
    }
    return idx;
}

QString StringToInt::lookupString(int idx)
{
    auto& cache = StringToIntCache::instance();
    std::shared_lock readLocker(cache.lock);
    if (Q_LIKELY(cache.reverseMap.size() > size_t(idx)))
        return cache.reverseMap[idx];

    return QString();
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
