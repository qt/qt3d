#!/usr/bin/perl
use File::Find;

sub system_example_dirs {
    print "Setting example directories\n";
    return ( '../../../examples/qt3d', '../../../examples/quick3d', '../../../demos/qt3d', '../../../demos/quick3d');
}

sub pkg_template_files
{
    print "Recording pkg files of QtQuick3D\n";
    my @out;

    #get all of the pkg files we might want.
    my $wanted = sub {
        # If the file has _template.pkg in the name, store the file and path to it
        my $file = lc $File::Find::name;
        if ($file =~ /_template\.pkg/) {
            print "Found .pkg file: $file \n";
            push(@out, $file);
        }
    };

    # Iterate through the example and demo directories to find pkg files
    foreach my $dir (system_example_dirs) {
        find($wanted, "${dir}");
    }

    return @out;
}

sub parse_pkg
{
  my $filename = shift;
  open FILE, $filename or die "Could not read from $filename, program halting.";
  print "Parsing package file: $filename\n";
  my $lines ="";
  my $line;
  # Iterate through each line of the file.
  while (defined ($line = <FILE>))
    {
        # If we find a ;DEPLOYMENT line, get the lines below it up to (and including) a blank line
        if ($line =~ /; DEPLOYMENT/ ) {
            my $recorded = 0;
            $lines .= $line;
              while ($recorded == 0) {
                  #get lines until we find a blank line
                  my $record_line = <FILE>;
                  if ($record_line =~ /^$/) {
                        $recorded = 1;
                        $lines .= $record_line;
                  }
                  else {
                      $lines .= $record_line;
                  }
              }
        }
    }

  close FILE;
  return $lines;
}

open(OUTPUTPKG, ">QtQuick3D_apps_template.pkg") or die "cannot open file for reading: $!";

# Print the pkg file heading for our collective pkg file
print OUTPUTPKG <<PKGHEADER;

; Language
&EN

#{"Qt Quick3D Examples"},(0xA89FAE20),1,0,0
; SIS header: name, uid, version

; Localised Vendor name
%{"Nokia"}

; Unique Vendor name
:"Nokia"

; Default dependency to Qt libraries
(0x2001E61C), 4, 7, 3, {"Qt"}

; Default dependency to QtQuick3D libraries
(0x2002AC89), 1, 0, 0, {"Qt Quick 3D"}

; Default HW/platform dependencies
[0x20022E6D],0,0,0,{"S60ProductID"}

PKGHEADER

# Collect the pkg file path for each test application and library needed for system testing
my @files = pkg_template_files;

my $pkgcontent;

# Parse the collected pkg files and append the parsed result to the content of the collective one
foreach (@files) {
    $pkgcontent .= "; $_\n";
    $pkgcontent .= parse_pkg $_;
    $pkgcontent .= "\n";
}

print OUTPUTPKG $pkgcontent;
