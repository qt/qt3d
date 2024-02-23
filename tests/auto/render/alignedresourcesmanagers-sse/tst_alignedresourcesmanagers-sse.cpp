// Copyright (C) 2017 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/private/matrix4x4_sse_p.h>
#include <Qt3DCore/private/qresourcemanager_p.h>
#include <Qt3DRender/private/cameralens_p.h>

using namespace Qt3DCore;

using HMatrix = Qt3DCore::QHandle<Matrix4x4_SSE>;
using HCameraLens = Qt3DCore::QHandle<Qt3DRender::Render::CameraLens>;

class MatrixManager : public Qt3DCore::QResourceManager<
        Matrix4x4_SSE,
        Qt3DCore::QNodeId,
        Qt3DCore::NonLockingPolicy>
{
public:
    MatrixManager() {}
};

class CameraLensManager : public Qt3DCore::QResourceManager<
        Qt3DRender::Render::CameraLens,
        Qt3DCore::QNodeId,
        Qt3DCore::NonLockingPolicy>
{
public:
    CameraLensManager() {}
};


class tst_AlignedResourcesManagersSSE: public QObject
{
    Q_OBJECT

#ifdef __SSE2__
private Q_SLOTS:

    void checkAllocationAndAlignmentMatrix4x4()
    {
        // GIVEN
        MatrixManager manager;

        // WHEN
        for (uint i = 0; i < std::numeric_limits<ushort>::max(); ++i)
            manager.getOrCreateResource(Qt3DCore::QNodeId::createId());

        // THEN
        // Shouldn't crash

        const std::vector<HMatrix> &activeHandles = manager.activeHandles();
        for (const HMatrix &handle : activeHandles) {
            // WHEN
            Matrix4x4_SSE *mat = manager.data(handle);
            // THEN
            QCOMPARE(int((uintptr_t)mat % 16), 0);
        }

        // WHEN
        for (uint i = 2; i < std::numeric_limits<ushort>::max(); ++i) {
            Matrix4x4_SSE *mat1 = manager.data(activeHandles.at(i - 2));
            Matrix4x4_SSE *mat2 = manager.data(activeHandles.at(i - 1));
            Matrix4x4_SSE *mat3 = manager.data(activeHandles.at(i));

            // WHEN
            *mat3 = (*mat2 * *mat1);

            // THEN
            // Shouldn't crash
        }
    }

    void checkAllocationAndAlignmentCameraLens()
    {
        // GIVEN
        CameraLensManager manager;

        // WHEN
        for (uint i = 0; i < std::numeric_limits<ushort>::max(); ++i)
            manager.getOrCreateResource(Qt3DCore::QNodeId::createId());

        // THEN
        // Shouldn't crash

        const std::vector<HCameraLens> &activeHandles = manager.activeHandles();
        for (const HCameraLens &handle : activeHandles) {
            // WHEN
            Qt3DRender::Render::CameraLens *lens = manager.data(handle);
            // THEN
#  ifdef __AVX2__
            QCOMPARE(int((uintptr_t)lens % 32), 0);
#  else
            QCOMPARE(int((uintptr_t)lens % 16), 0);
#  endif
        }
    }
#endif // __SSE2__
};

QTEST_MAIN(tst_AlignedResourcesManagersSSE)

#include "tst_alignedresourcesmanagers-sse.moc"
