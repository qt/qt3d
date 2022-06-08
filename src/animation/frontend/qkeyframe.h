// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_QKEYFRAME_H
#define QT3DANIMATION_QKEYFRAME_H

#include <QtGui/qvector2d.h>
#include <Qt3DAnimation/qt3danimation_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QKeyFrame
{
public:
    enum InterpolationType : quint8 {
        ConstantInterpolation,
        LinearInterpolation,
        BezierInterpolation
    };

    constexpr QKeyFrame() noexcept
        : m_coordinates()
        , m_leftControlPoint()
        , m_rightControlPoint()
        , m_interpolationType(BezierInterpolation)
    {
    }

    constexpr explicit QKeyFrame(QVector2D coords) noexcept
        : m_coordinates(coords)
        , m_leftControlPoint()
        , m_rightControlPoint()
        , m_interpolationType(LinearInterpolation)
    {
    }

    constexpr explicit QKeyFrame(QVector2D coords,
                                        QVector2D lh,
                                        QVector2D rh) noexcept
        : m_coordinates(coords)
        , m_leftControlPoint(lh)
        , m_rightControlPoint(rh)
        , m_interpolationType(BezierInterpolation)
    {
    }

    void setCoordinates(QVector2D coords) noexcept
    {
        m_coordinates = coords;
    }

    constexpr QVector2D coordinates() const noexcept
    {
        return m_coordinates;
    }

    void setLeftControlPoint(QVector2D lh) noexcept
    {
        m_leftControlPoint = lh;
    }

    constexpr QVector2D leftControlPoint() const noexcept
    {
        return m_leftControlPoint;
    }

    void setRightControlPoint(QVector2D rh) noexcept
    {
        m_rightControlPoint = rh;
    }

    constexpr QVector2D rightControlPoint() const noexcept
    {
        return m_rightControlPoint;
    }

    void setInterpolationType(InterpolationType interp) noexcept
    {
        m_interpolationType = interp;
    }

    constexpr InterpolationType interpolationType() const noexcept
    {
        return m_interpolationType;
    }

    friend inline bool operator==(const QKeyFrame &, const QKeyFrame &) noexcept;
    friend inline bool operator!=(const QKeyFrame &, const QKeyFrame &) noexcept;

private:
    QVector2D m_coordinates;
    QVector2D m_leftControlPoint;
    QVector2D m_rightControlPoint;
    InterpolationType m_interpolationType;
};

QT3D_DECLARE_TYPEINFO(Qt3DAnimation, QKeyFrame, Q_PRIMITIVE_TYPE)

inline bool operator==(const QKeyFrame &lhs, const QKeyFrame &rhs) noexcept
{
    if (lhs.m_interpolationType != rhs.m_interpolationType)
        return false;

    if (lhs.m_interpolationType == QKeyFrame::BezierInterpolation) {
        return lhs.m_coordinates == rhs.m_coordinates &&
               lhs.m_leftControlPoint == rhs.m_leftControlPoint &&
               lhs.m_rightControlPoint == rhs.m_rightControlPoint;
    }

    return lhs.m_coordinates == rhs.m_coordinates;
}

inline bool operator!=(const QKeyFrame &lhs, const QKeyFrame &rhs) noexcept
{
    return !(lhs == rhs);
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QKEYFRAME_H
