#!/usr/bin/perl

############################################################################
##
## Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
## Contact: http://www.qt-project.org/legal
##
## This file is part of the Qt3D module of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## Commercial License Usage
## Licensees holding valid commercial Qt licenses may use this file in
## accordance with the commercial license agreement provided with the
## Software or, alternatively, in accordance with the terms contained in
## a written agreement between you and Digia.  For licensing terms and
## conditions see http://qt.digia.com/licensing.  For further information
## use the contact form at http://qt.digia.com/contact-us.
##
## GNU Lesser General Public License Usage
## Alternatively, this file may be used under the terms of the GNU Lesser
## General Public License version 2.1 as published by the Free Software
## Foundation and appearing in the file LICENSE.LGPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU Lesser General Public License version 2.1 requirements
## will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Digia gives you certain additional
## rights.  These rights are described in the Digia Qt LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU
## General Public License version 3.0 as published by the Free Software
## Foundation and appearing in the file LICENSE.GPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU General Public License version 3.0 requirements will be
## met: http://www.gnu.org/copyleft/gpl.html.
##
##
## $QT_END_LICENSE$
##
############################################################################/

use strict;
use warnings;

my $filtname = shift || 'qt3d';

print "Filtering to find all qdoc errors in $filtname directory.\n";

# Find lines we care about for our project
# /Users/sarasmit/depot/qt5new/qt3d/src/imports/shapes/teapot.h:58: No documentation for 'Teapot::~Teapot()'
# matches if it has '/qt3d/'
my $filt_rx = qr/\/$filtname\// ;

# Find lines that report an error with a line number - lines that do not match this are considered "follow-on lines" with more explanation relating to a previous error report
# /Users/sarasmit/depot/qt5new/qtbase/src/corelib/global/qnumeric.h:54: Global function, qIsFinite(), has documentation but no \relates command
# matches if it has non-space characters followed by colon, numbers, colon
my $line_rx = qr/\S+:\d+\:/ ;

my ( $is_line_of_interest, $is_error_line, $is_follow_on_line, $last_line_was_of_interest );
my $line_count = 0;

while (<>)
{
    chomp;
    $is_line_of_interest = m/$filt_rx/ ;
    $is_error_line = m/$line_rx/ ;
    $is_follow_on_line = 1 if ( $last_line_was_of_interest && ! $is_error_line );
    $last_line_was_of_interest = undef;
    next unless ( $is_follow_on_line || ( $is_line_of_interest && $is_error_line ));
    if ( $is_error_line )
    {
        my ( $file_name, $line_no, $message ) = m/$filt_rx([^:]+):(\d+): (.+)$/ ;
        print "$file_name, line: $line_no - $message\n";
    }
    else
    {
        print "   > $_\n";
    }
    $line_count++;
    $last_line_was_of_interest = 1;
    $is_follow_on_line = undef;
}

print "\n\n=== Found $line_count lines ===\n";
