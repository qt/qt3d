/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
