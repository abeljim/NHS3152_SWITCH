@echo off

set SRCDIR=.
set BUILDDIR=build\debug

if exist "%BUILDDIR%" (
    echo Removing directory: %BUILDDIR%
    rmdir /s /q "%BUILDDIR%"
)

mkdir "%BUILDDIR%"


meson setup "%SRCDIR%" "%BUILDDIR%" --cross-file config\cross_windows.txt --buildtype=debugoptimized

cd "%BUILDDIR%"
