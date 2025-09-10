// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_VECTOR_HELPER_P_H
#define QT3DCORE_VECTOR_HELPER_P_H

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

#include <Qt3DCore/private/qt3dcore-config_p.h>
#include <qobjectdefs.h>
#include <vector>
#include <algorithm>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

template<typename T, typename U>
inline void moveAtEnd(std::vector<T>& destination, std::vector<U>&& source)
{
    destination.insert(destination.end(),
                       std::make_move_iterator(source.begin()),
                       std::make_move_iterator(source.end()));
}

template<typename T>
inline T moveAndClear(T &data)
{
    T ret(std::move(data));
    data.clear();
    return ret;
}

template<typename T, typename U>
inline void append(std::vector<T>& destination, const U& source)
{
    destination.insert(destination.end(),
                       source.cbegin(),
                       source.cend());
}

template<typename T, typename U>
inline bool contains(const std::vector<T>& destination, const U& element) noexcept
{
    return std::find(destination.begin(), destination.end(), element) != destination.end();
}

template <typename ForwardIterator>
inline void deleteAll(ForwardIterator begin, ForwardIterator end)
{
    while (begin != end) {
        delete *begin;
        ++begin;
    }
}

template <typename Container>
inline void deleteAll(const Container &c)
{
    qDeleteAll(c.begin(), c.end());
}

} // namespace Qt3DCore

QT_END_NAMESPACE
#endif // QT3DCORE_VECTOR_HELPER_P_H
