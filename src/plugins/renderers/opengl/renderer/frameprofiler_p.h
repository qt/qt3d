// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_FRAMEPROFILER_P_H
#define QT3DRENDER_RENDER_FRAMEPROFILER_P_H

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

#include <QOpenGLTimeMonitor>
#include <Qt3DCore/private/qthreadpooler_p.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>
#include <memory>

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_OPENGL) && !QT_CONFIG(opengles2)
#define QT3D_SUPPORTS_GL_MONITOR
#endif

namespace Qt3DCore {
class QSystemInformationService;
}

namespace Qt3DRender {

namespace Render {

namespace Profiling {

enum RecordingType
{
    DrawArray = 512,
    DrawElement,
    DispatchCompute,
    StateUpdate,
    UniformUpdate,
    ShaderUpdate,
    TextureUpload,
    BufferUpload,
    ShaderUpload,
    ClearBuffer,
    VAOUpdate,
    VAOUpload,
    RenderTargetUpdate
};

class FrameTimeRecorder
{
public:
    FrameTimeRecorder(Qt3DCore::QSystemInformationService *service)
        : m_service(service)
    {
    }

    ~FrameTimeRecorder()
    {
    }

    void init(int eventCount)
    {
#ifdef QT3D_SUPPORTS_GL_MONITOR
        if (m_monitor.isCreated()) {
            m_remainingEvents = m_monitor.sampleCount();
            reset();
        } else {
            m_monitor.setSampleCount(eventCount * 2);
            m_monitor.create();
            m_remainingEvents = eventCount;
        }
#else
        m_remainingEvents = eventCount;
#endif
    }

    void startRecordEvent()
    {
#ifdef QT3D_SUPPORTS_GL_MONITOR
        m_monitor.recordSample();
#endif
        --m_remainingEvents;
    }

    void recordEvent(RecordingType type)
    {
#ifdef QT3D_SUPPORTS_GL_MONITOR
        m_monitor.recordSample();
#endif
        --m_remainingEvents;

        GLRecording rec;
        rec.type = type;
        rec.startTime = Qt3DCore::QSystemInformationServicePrivate::get(m_service)->m_jobsStatTimer.nsecsElapsed();
        m_recordings.push_back(rec);
    }

    void reset()
    {
#ifdef QT3D_SUPPORTS_GL_MONITOR
        m_monitor.reset();
#endif
        m_recordings.clear();
    }

    inline bool canStillRecord() { return m_remainingEvents > 0; }

    bool tryWriteResults()
    {
#ifdef QT3D_SUPPORTS_GL_MONITOR
        if (m_monitor.isResultAvailable()) {
            const auto &samples = m_monitor.waitForSamples();
            Q_ASSERT(samples.size() >= 2 * m_recordings.size());

            Qt3DCore::QSystemInformationServicePrivate *dservice = Qt3DCore::QSystemInformationServicePrivate::get(m_service);

            qsizetype j = 0;
            for (qsizetype i = 0, m = m_recordings.size(); i < m; ++i) {
                const GLRecording rec = m_recordings.at(i);
                Qt3DCore::QSystemInformationServicePrivate::JobRunStats glRecordingStat;

                glRecordingStat.jobId.typeAndInstance[0] = rec.type;
                glRecordingStat.jobId.typeAndInstance[1] = 0;
                glRecordingStat.threadId = FrameTimeRecorder::GLThreadID;
                glRecordingStat.startTime = rec.startTime;
                glRecordingStat.endTime = rec.startTime + (samples.at(j + 1) - (samples.at(j)));

                dservice->addSubmissionLogStatsEntry(glRecordingStat);
                j += 2;
            }
            return true;
        }
#endif
        return false;
    }

private:
    struct GLRecording
    {
        RecordingType type;
        qint64 startTime;
    };

    static const int GLThreadID = 0x454;

    Qt3DCore::QSystemInformationService *m_service;
#ifdef QT3D_SUPPORTS_GL_MONITOR
    QOpenGLTimeMonitor m_monitor;
#endif
    QList<GLRecording> m_recordings;
    int m_remainingEvents = 0;
};

class FrameProfiler
{
public:
    FrameProfiler(Qt3DCore::QSystemInformationService *service)
        : m_service(service)
        , m_currentRecorder(nullptr)
    {}

    ~FrameProfiler()
    {
        qDeleteAll(m_recorders);
    }

    void startRecordEvent()
    {
        if (m_currentRecorder == nullptr) {
            if (!m_availableRecorders.empty()) {
                m_currentRecorder = m_availableRecorders.takeFirst();
            } else {
                m_recorders.push_back(new FrameTimeRecorder(m_service));
                m_currentRecorder = m_recorders.last();
            }
            // We record events 10 by 10
            m_currentRecorder->init(10);
        }
        m_currentRecorder->startRecordEvent();
    }

    void recordEvent(RecordingType type)
    {
        m_currentRecorder->recordEvent(type);
        if (!m_currentRecorder->canStillRecord()) {
            m_busyRecorders.push_back(m_currentRecorder);
            m_currentRecorder = nullptr;
        }
    }

    void writeResults()
    {
        for (int i = m_busyRecorders.size() - 1; i >= 0; --i) {
            FrameTimeRecorder *recorder = m_busyRecorders.at(i);
            if (recorder->tryWriteResults()) {
                m_availableRecorders.push_back(m_busyRecorders.takeAt(i));
            }
        }
    }

private:
    Qt3DCore::QSystemInformationService *m_service;
    QList<FrameTimeRecorder *> m_recorders;
    QList<FrameTimeRecorder *> m_availableRecorders;
    QList<FrameTimeRecorder *> m_busyRecorders;
    FrameTimeRecorder *m_currentRecorder;
};


class GLTimeRecorder
{
public:
    explicit GLTimeRecorder(RecordingType type, FrameProfiler *profiler)
        : m_type(type)
        , m_frameProfiler(profiler)
    {
        if (m_frameProfiler)
            m_frameProfiler->startRecordEvent();
    }

    ~GLTimeRecorder()
    {
        if (m_frameProfiler)
            m_frameProfiler->recordEvent(m_type);
    }

private:
    RecordingType m_type;
    FrameProfiler *m_frameProfiler;
};

} // Profiling

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_FRAMEPROFILER_P_H
