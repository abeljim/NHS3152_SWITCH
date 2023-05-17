$srcdir = "."
$buildir = "build/debug"

if (Test-Path $buildir) {
    Write-Host "removing directory: $buildir"
    Remove-Item -Recurse -Force $buildir
}

meson setup $srcdir $buildir --cross-file config/cross_windows.txt --buildtype=debugoptimized
Set-Location $buildir