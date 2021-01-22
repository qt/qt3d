/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

#include "rhigraphicspipeline_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

RHIGraphicsPipeline::RHIGraphicsPipeline()
    : m_rvUbo(nullptr),
      m_cmdUbo(nullptr),
      m_pipeline(nullptr),
      m_shaderResourceBindings(nullptr),
      m_score(0)
{
}

RHIGraphicsPipeline::~RHIGraphicsPipeline() { }

void RHIGraphicsPipeline::cleanup()
{
    delete m_shaderResourceBindings;
    delete m_rvUbo;
    delete m_cmdUbo;
    delete m_pipeline;
    m_rvUbo = nullptr;
    m_cmdUbo = nullptr;
    m_pipeline = nullptr;
    m_shaderResourceBindings = nullptr;
    m_ubos.clear();
    m_attributeNameIdToBindingIndex.clear();
}

} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE
