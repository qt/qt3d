TEMPLATE = subdirs

!package: SUBDIRS += auto

# Benchmarks make sense in release mode only.
# Disable them for code coverage.
!testcocoon: contains(QT_CONFIG,release): SUBDIRS += benchmarks
