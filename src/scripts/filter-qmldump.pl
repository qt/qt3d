#!/usr/bin/perl
############################################################################
##
## Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
## All rights reserved.
## Contact: Nokia Corporation (qt-info@nokia.com)
##
## This file is part of the QtQuick3D module of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## GNU Lesser General Public License Usage
## This file may be used under the terms of the GNU Lesser General Public
## License version 2.1 as published by the Free Software Foundation and
## appearing in the file LICENSE.LGPL included in the packaging of this
## file. Please review the following information to ensure the GNU Lesser
## General Public License version 2.1 requirements will be met:
## http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Nokia gives you certain additional
## rights. These rights are described in the Nokia Qt LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU General
## Public License version 3.0 as published by the Free Software Foundation
## and appearing in the file LICENSE.GPL included in the packaging of this
## file. Please review the following information to ensure the GNU General
## Public License version 3.0 requirements will be met:
## http://www.gnu.org/copyleft/gpl.html.
##
## Other Usage
## Alternatively, this file may be used in accordance with the terms and
## conditions contained in a signed written agreement between you and Nokia.
##
##
##
##
##
## $QT_END_LICENSE$
##
############################################################################

use strict;
use warnings;

use XML::XPath;
use XML::XPath::XMLParser;

my $mod = shift or die "Usage: filter-qmldump.pl <module name>\n";

warn "Filtering for modules in $mod\n";

my $xp = XML::XPath->new(filename => 'library.xml');

my $nodeset = $xp->find("//type/exports/export[\@module=\'$mod\']/../..");

print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
print "<module>\n";

foreach my $node ($nodeset->get_nodelist)
{
    print "    " . XML::XPath::XMLParser::as_string($node) . "\n";
}

print "<\\module>\n";
