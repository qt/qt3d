/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef BLENDSTATE_H
#define BLENDSTATE_H

#include <Qt3DRenderer/private/renderstate_p.h>
#include <Qt3DRenderer/private/genericstate_p.h>

#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

class BlendState : public GenericState2<BlendState, GLenum, GLenum>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;
    virtual StateMaskSet mask() const Q_DECL_OVERRIDE;

    static BlendState *getOrCreate(GLenum src, GLenum dst);
private:
    BlendState(GLenum src, GLenum dst);
};

class BlendEquation : public GenericState1<BlendEquation, GLenum>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;

    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return BlendStateMask; }

    static BlendEquation *getOrCreate(GLenum func);

private:
    BlendEquation(GLenum func);
};


class AlphaFunc : public GenericState2<AlphaFunc, GLenum, GLclampf>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;

    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return AlphaTestMask; }

    static AlphaFunc *getOrCreate(GLenum func, GLclampf value);
private:
    AlphaFunc(GLenum func, GLclampf value);
};

class DepthTest : public GenericState1<DepthTest, GLenum>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;

    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return DepthTestStateMask; }

    static DepthTest *getOrCreate(GLenum func);

private:
    DepthTest(GLenum func);
};

class DepthMask : public GenericState1<DepthMask, GLboolean>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;

    virtual StateMaskSet mask() const  Q_DECL_OVERRIDE
    { return DepthWriteStateMask; }

    static DepthMask *getOrCreate(GLboolean func);

private:
    DepthMask(GLboolean func);
};

class CullFace : public GenericState1<CullFace, GLenum>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;

    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return CullFaceStateMask; }

    static CullFace *getOrCreate(GLenum func);

private:
    CullFace(GLenum func);
};

class FrontFace : public GenericState1<FrontFace, GLenum>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;

    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return FrontFaceStateMask; }
    static FrontFace *getOrCreate(GLenum func);

private:
    FrontFace(GLenum func);
};

class Dithering : public Qt3D::Render::RenderState
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;
    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return DitheringStateMask; }

    bool isEqual(const Dithering &) { return true; }

    static Dithering *getOrCreate();
private:
    Dithering();
};

class ScissorTest : public GenericState4<ScissorTest, int, int, int, int>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;
    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return ScissorStateMask; }

    static ScissorTest *getOrCreate(int left, int bottom, int width, int height);

private:
    ScissorTest(int left, int bottom, int width, int height);
};

class StencilTest : public GenericState3<StencilTest, uint, GLenum, GLenum>
{
public:
    virtual void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;
    virtual StateMaskSet mask() const Q_DECL_OVERRIDE
    { return StencilTestStateMask|StencilWriteStateMask; }
    static StencilTest *getOrCreate(uint mask, GLenum func, GLenum faceMode);

private:
    StencilTest(uint mask, GLenum func, GLenum faceMode);
};

class AlphaCoverage : public Qt3D::Render::RenderState
{
public:
    void apply(QGraphicsContext *gc) const Q_DECL_OVERRIDE;
    StateMaskSet mask() const Q_DECL_OVERRIDE
    { return AlphaCoverageStateMask; }

    bool isEqual(const AlphaCoverage &) { return true; }

    static AlphaCoverage *getOrCreate();

private:
    AlphaCoverage();
};

} // Render
} // Qt3D

QT_END_NAMESPACE

#endif // BLENDSTATE_H
