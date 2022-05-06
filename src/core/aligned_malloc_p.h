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
******************************************************************************/

#ifndef QT3DRENDER_RENDER_ALIGNED_MALLOC_P_H
#define QT3DRENDER_RENDER_ALIGNED_MALLOC_P_H

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

#include <QtCore/private/qsimd_p.h>
#include <Qt3DCore/private/qt3dcore-config_p.h>

#if QT_CONFIG(qt3d_simd_avx2) && defined(__AVX2__) && defined(QT_COMPILER_SUPPORTS_AVX2)
# define QT3D_ALIGNED_MALLOC(s) _mm_malloc(s, 32)
#elif QT_CONFIG(qt3d_simd_sse2) && defined(__SSE2__) && defined(QT_COMPILER_SUPPORTS_SSE2)
# define QT3D_ALIGNED_MALLOC(s) _mm_malloc(s, 16)
#else
    #define QT3D_ALIGNED_MALLOC(s) malloc(s)
#endif

#if (QT_CONFIG(qt3d_simd_avx2) && defined(__AVX2__) && defined(QT_COMPILER_SUPPORTS_AVX2)) || (QT_CONFIG(qt3d_simd_sse2) && defined(__SSE2__) && defined(QT_COMPILER_SUPPORTS_SSE2))
# define QT3D_ALIGNED_FREE(ptr) _mm_free(ptr)
#else
# define QT3D_ALIGNED_FREE(ptr) free(ptr)
#endif

#define QT3D_ALIGNED_MALLOC_AND_FREE() \
    static void *operator new(size_t s) \
    { \
        return QT3D_ALIGNED_MALLOC(s); \
    } \
    static void operator delete(void *ptr) \
    { \
        QT3D_ALIGNED_FREE(ptr); \
    }

#endif // QT3DRENDER_RENDER_ALIGNED_MALLOC_P_H
