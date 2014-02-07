/* -*- C++ -*-

   This file declares debugging aids for multithreaded applications.

   $ Author: Mirko Boehm $
   $ Copyright: (C) 2004-2013 Mirko Boehm $
   $ Contact: mirko@kde.org
         http://www.kde.org
         http://creative-destruction.me $

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.

   $Id: DebuggingAids.h 30 2005-08-16 16:16:04Z mirko $
*/

//krazy:excludeall=inline

#ifndef DEBUGGINGAIDS_H
#define DEBUGGINGAIDS_H

#include <QtCore/QtGlobal>

extern "C"
{
#include <stdarg.h>
#ifndef Q_OS_WIN
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
}

#include <QtCore/QMutex>
#include <QtCore/QString>
#include "threadweaver_export.h"

namespace ThreadWeaver
{

extern THREADWEAVER_EXPORT bool Debug;
extern THREADWEAVER_EXPORT int DebugLevel;
extern THREADWEAVER_EXPORT QMutex GlobalMutex;

/** Set the debug log level.
@see debug
*/
extern inline void setDebugLevel(bool debug, int level);

/** This method prints a text message on the screen, if debugging is
enabled. Otherwise, it does nothing. The message is thread safe,
therefore providing that the messages appear in the order they where
issued by the different threads.
All messages are suppressed when Debug is false. All messages with a
lower importance (higher number) than DebugLevel will be suppressed,
too. Debug level 0 messages will always be printed as long as
Debug is true.
    We use our own debugging method, since debugging threads is a more
    complicated experience than debugging single threaded
    contexts. This might change in future in the way that debug
    prints its messages to another logging facility provided by
    the platform.
Use setDebugLevel () to integrate adapt debug () to your platform.
*/
inline void debug(int severity, const char *cformat, ...)
#ifdef __GNUC__
__attribute__((format(printf, 2, 3)))
#endif
;

/** Prints the message to the console if condition is true. */
inline void debug(bool condition, int severity, const char *cformat, ...)
#ifdef __GNUC__
__attribute__((format(printf, 3, 4)))
#endif
;

/** PROTECT executes x with GlobalMutex locked.
    Mostly used for debugging, as in P_ASSERT. */
#ifdef PROTECT
#undef PROTECT
#endif
#define PROTECT(x) do { QMutexLocker l(&ThreadWeaver::GlobalMutex); (x); } while (0)

/** P_ASSERT ensures that error messages occur in the correct order. */
#ifdef P_ASSERT
#undef P_ASSERT
#endif

#define P_ASSERT(x) do { QMutexLocker l(&ThreadWeaver::GlobalMutex); Q_ASSERT(x); } while (0)

inline void setDebugLevel(bool debug, int level)
{
    Debug = debug;
    DebugLevel = level;
}

#ifndef QT_NO_DEBUG
inline void debug(int severity, const char *cformat, ...)
{
    if (Debug == true && (severity <= DebugLevel || severity == 0)) {
        QString text;

        va_list ap;
        va_start(ap, cformat);
        PROTECT(vprintf(cformat, ap));
        va_end(ap);
    }
}

inline void debug(bool condition, int severity, const char *cformat, ...)
{
    if (condition && Debug == true && (severity <= DebugLevel || severity == 0)) {
        QString text;

        va_list ap;
        va_start(ap, cformat);
        PROTECT(vprintf(cformat, ap));
        va_end(ap);
    }
}
#else
inline void debug(int, const char *, ...) {}
inline void debug(bool, int, const char *, ...) {}
#endif

inline bool invariant()
{
    return true;
}

#define INVARIANT Q_ASSERT_X (invariant(), __FILE__, "class invariant failed" );

#define REQUIRE(x) \
    INVARIANT \
    Q_ASSERT_X (x, Q_FUNC_INFO, "unfulfilled requirement " #x );

#define ENSURE(x) \
    INVARIANT \
    Q_ASSERT_X (x, Q_FUNC_INFO, "broken guarantee " #x );

#ifdef QT_NO_DEBUG
#define DEBUGONLY(x)
#else
#define DEBUGONLY(x) x
#endif

}

#endif // DEBUGGINGAIDS_H
