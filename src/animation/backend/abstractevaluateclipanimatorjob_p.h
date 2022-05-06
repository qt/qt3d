/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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

#ifndef QT3DANIMATION_ANIMATION_ABSTRACTEVALUATECLIPANIMATORJOB_P_H
#define QT3DANIMATION_ANIMATION_ABSTRACTEVALUATECLIPANIMATORJOB_P_H

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

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DAnimation/private/animationutils_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class AbstractEvaluateClipAnimatorJobPrivate;

class AbstractEvaluateClipAnimatorJob : public Qt3DCore::QAspectJob
{
protected:
    AbstractEvaluateClipAnimatorJob();

    void setPostFrameData(const AnimationRecord &record, const QVector<AnimationCallbackAndValue> &callbacks);

private:
    Q_DECLARE_PRIVATE(AbstractEvaluateClipAnimatorJob)
};

} // Animation
} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_ABSTRACTEVALUATECLIPANIMATORJOB_P_H
