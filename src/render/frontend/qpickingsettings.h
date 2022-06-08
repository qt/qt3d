// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QPICKINGSETTINGS_H
#define QT3DRENDER_QPICKINGSETTINGS_H

#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QPickingSettingsPrivate;

// TO DO: Qt 6 -> Make this a QObject

class Q_3DRENDERSHARED_EXPORT QPickingSettings : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(PickMethod pickMethod READ pickMethod WRITE setPickMethod NOTIFY pickMethodChanged)
    Q_PROPERTY(PickResultMode pickResultMode READ pickResultMode WRITE setPickResultMode NOTIFY pickResultModeChanged)
    Q_PROPERTY(FaceOrientationPickingMode faceOrientationPickingMode READ faceOrientationPickingMode WRITE setFaceOrientationPickingMode NOTIFY faceOrientationPickingModeChanged)
    Q_PROPERTY(float worldSpaceTolerance READ worldSpaceTolerance WRITE setWorldSpaceTolerance NOTIFY worldSpaceToleranceChanged REVISION 10)
public:
    explicit QPickingSettings(Qt3DCore::QNode *parent = nullptr);
    ~QPickingSettings();

    enum PickMethod {
        BoundingVolumePicking = 0x00,
        TrianglePicking = 0x01,
        LinePicking = 0x02,
        PointPicking = 0x04,
        PrimitivePicking = TrianglePicking | LinePicking | PointPicking
    };
    Q_ENUM(PickMethod) // LCOV_EXCL_LINE

    enum PickResultMode {
        NearestPick,
        AllPicks,
        NearestPriorityPick
    };
    Q_ENUM(PickResultMode) // LCOV_EXCL_LINE

    enum FaceOrientationPickingMode {
        FrontFace = 0x01,
        BackFace = 0x02,
        FrontAndBackFace = 0x03
    };
    Q_ENUM(FaceOrientationPickingMode) // LCOV_EXCL_LINE

    PickMethod pickMethod() const;
    PickResultMode pickResultMode() const;
    FaceOrientationPickingMode faceOrientationPickingMode() const;
    float worldSpaceTolerance() const;

public Q_SLOTS:
    void setPickMethod(PickMethod pickMethod);
    void setPickResultMode(PickResultMode pickResultMode);
    void setFaceOrientationPickingMode(FaceOrientationPickingMode faceOrientationPickingMode);
    void setWorldSpaceTolerance(float worldSpaceTolerance);

Q_SIGNALS:
    void pickMethodChanged(QPickingSettings::PickMethod pickMethod);
    void pickResultModeChanged(QPickingSettings::PickResultMode pickResult);
    void faceOrientationPickingModeChanged(QPickingSettings::FaceOrientationPickingMode faceOrientationPickingMode);
    void worldSpaceToleranceChanged(float worldSpaceTolerance);

protected:
    Q_DECLARE_PRIVATE(QPickingSettings)
    explicit QPickingSettings(QPickingSettingsPrivate &dd, Qt3DCore::QNode *parent = nullptr);
};

} // namespace Qt3Drender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPICKINGSETTINGS_H
