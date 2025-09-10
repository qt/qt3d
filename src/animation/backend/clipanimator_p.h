// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_CLIPANIMATOR_P_H
#define QT3DANIMATION_ANIMATION_CLIPANIMATOR_P_H

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

#include <Qt3DAnimation/private/backendnode_p.h>
#include <Qt3DAnimation/private/animationutils_p.h>
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

struct Channel;
class Handler;

class Q_AUTOTEST_EXPORT ClipAnimator : public BackendNode
{
public:
    ClipAnimator();

    void cleanup();
    void setClipId(Qt3DCore::QNodeId clipId);
    Qt3DCore::QNodeId clipId() const { return m_clipId; }
    void setMapperId(Qt3DCore::QNodeId mapperId);
    Qt3DCore::QNodeId mapperId() const { return m_mapperId; }
    void setClockId(Qt3DCore::QNodeId clockId);
    Qt3DCore::QNodeId clockId() const { return m_clockId; }

    void setRunning(bool running);
    bool isRunning() const { return m_running; }
    void setLoops(int loops) { m_loops = loops; }
    int loops() const { return m_loops; }
    void setNormalizedLocalTime(float normalizedLocalTime, bool allowMarkDirty = true);
    float normalizedLocalTime() const { return m_normalizedLocalTime; }

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;
    void setHandler(Handler *handler) { m_handler = handler; }

    // Called by jobs
    bool canRun() const { return !m_clipId.isNull() && !m_mapperId.isNull(); }
    void setMappingData(const QVector<MappingData> &mappingData) { m_mappingData = mappingData; }
    QVector<MappingData> mappingData() const { return m_mappingData; }

    void setStartTime(qint64 globalTime) { m_lastGlobalTimeNS = globalTime; }

    int currentLoop() const { return m_currentLoop; }
    void setCurrentLoop(int currentLoop) { m_currentLoop = currentLoop; }

    void animationClipMarkedDirty() { setDirty(Handler::ClipAnimatorDirty); }

    void setClipFormat(const ClipFormat &clipFormat) { m_clipFormat = clipFormat; }
    ClipFormat clipFormat() const { return m_clipFormat; }

    qint64 nsSincePreviousFrame(qint64 currentGlobalTimeNS);
    void setLastGlobalTimeNS(qint64 lastGlobalTimeNS);

    double lastLocalTime() const;
    void setLastLocalTime(double lastLocalTime);

    float lastNormalizedLocalTime() { return m_lastNormalizedLocalTime; }
    void setLastNormalizedLocalTime(float normalizedLocalTime);
    bool isSeeking() const
    {
        return isValidNormalizedTime(m_normalizedLocalTime)
                && !qFuzzyCompare(m_lastNormalizedLocalTime, m_normalizedLocalTime);
    }

private:
    Qt3DCore::QNodeId m_clipId;
    Qt3DCore::QNodeId m_mapperId;
    Qt3DCore::QNodeId m_clockId;
    bool m_running;
    int m_loops;

    // Working state
    qint64 m_lastGlobalTimeNS;
    double m_lastLocalTime;
    QVector<MappingData> m_mappingData;

    int m_currentLoop;
    ClipFormat m_clipFormat;

    float m_normalizedLocalTime;
    float m_lastNormalizedLocalTime;
};

} // namespace Animation
} // namespace Qt3DAnimation


QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_CLIPANIMATOR_P_H
