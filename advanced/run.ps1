#requires -Version 5.1
<#
.SYNOPSIS
    Kompilyaciya i zapusk odnogo .cpp-fajla kursa advanced.
.DESCRIPTION
    Naxodit kompilyator (g++ po umolchaniyu), dobavlyaet ego katalog v PATH
    (inache cc1plus.exe ne startuet), sobiraet v advanced/.build/ i zapuskaet.
.PARAMETER Target
    Put' k .cpp-fajlu, k zadaniyu (papke) ili k imeni bez rasshireniya.
.PARAMETER Std
    Standart yazyka: c++20 | c++23 | c++26. Po umolchaniyu c++23.
.PARAMETER Compiler
    Yavnyj put' k kompilyatoru ili imya v PATH.
.PARAMETER NoRun
    Tol'ko sobran', ne zapuskat'.
.PARAMETER Sanitize
    Vklyuchit' AddressSanitizer + UndefinedBehaviorSanitizer (tol'ko g++/clang).
.PARAMETER Extra
    Dopolnitel'nye flagi odnoj strokoj, naprimer: -Extra '-Werror -Wconversion'
.EXAMPLE
    ./run.ps1 m01_syntax/ex01_types_literals
.EXAMPLE
    ./run.ps1 m02_ownership/ex04_move_semantics -Std c++26 -Sanitize
.EXAMPLE
    ./run.ps1 m00_tooling/ex02_compiler_flags -Extra '-Werror -Wconversion'
#>
[CmdletBinding()]
param(
    [Parameter(Position = 0, Mandatory = $true)]
    [string]$Target,

    [ValidateSet('c++20', 'c++23', 'c++26')]
    [string]$Std = 'c++23',

    [string]$Compiler = '',

    [switch]$NoRun,
    [switch]$Sanitize,

    [string]$Extra = ''
)

$ErrorActionPreference = 'Stop'
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

function Resolve-CompilerPath([string]$explicitName) {
    if ($explicitName) {
        $cmd = Get-Command $explicitName -ErrorAction SilentlyContinue
        if ($cmd) { return $cmd.Source }
        if (Test-Path -LiteralPath $explicitName) { return (Resolve-Path -LiteralPath $explicitName).Path }
        throw "Kompilyator ne najden: $explicitName"
    }
    if ($env:ADVANCED_CXX) {
        if (Test-Path -LiteralPath $env:ADVANCED_CXX) { return $env:ADVANCED_CXX }
        $cmd = Get-Command $env:ADVANCED_CXX -ErrorAction SilentlyContinue
        if ($cmd) { return $cmd.Source }
    }
    # Kanonicheskij dlya kursa: UCRT-toolchain MSYS2 (GCC >= 14, est' <print>).
    $known = @(
        'C:\msys64\ucrt64\bin\g++.exe',
        'C:\msys64\mingw64\bin\g++.exe',
        'C:\tools\msys64\ucrt64\bin\g++.exe'
    )
    foreach ($p in $known) { if (Test-Path -LiteralPath $p) { return $p } }
    $cmd = Get-Command g++ -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    $cmd = Get-Command clang++ -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    throw 'Ne najden g++ ili clang++. Ukazhite -Compiler <put> ili $env:ADVANCED_CXX.'
}

function Resolve-SourcePath([string]$target) {
    $candidates = @($target)
    if (-not [System.IO.Path]::IsPathRooted($target)) {
        $candidates += (Join-Path $scriptDir $target)
    }
    foreach ($cand in $candidates) {
        if (Test-Path -LiteralPath $cand -PathType Leaf) {
            return (Resolve-Path -LiteralPath $cand).Path
        }
        if (Test-Path -LiteralPath $cand -PathType Container) {
            $cands = @(Get-ChildItem -LiteralPath $cand -Filter *.cpp -File)
            if ($cands.Count -eq 0) { throw "V kataloge net .cpp: $cand" }
            $leaf = Split-Path -Leaf $cand.TrimEnd('\', '/')
            $exact = $cands | Where-Object { $_.BaseName -eq $leaf } | Select-Object -First 1
            if ($exact) { return $exact.FullName }
            if ($cands.Count -eq 1) { return $cands[0].FullName }
            throw "Neskol'ko .cpp v $cand - ukazhite fajl yavno."
        }
        if (Test-Path -LiteralPath "$cand.cpp" -PathType Leaf) {
            return (Resolve-Path -LiteralPath "$cand.cpp").Path
        }
    }
    throw "Ne najden fajl dlya sborki: $target"
}

$compilerPath = Resolve-CompilerPath $Compiler
$compilerDir = Split-Path -Parent $compilerPath
$env:PATH = "$compilerDir;$env:PATH"

$src = Resolve-SourcePath $Target
$buildDir = Join-Path $scriptDir '.build'
New-Item -ItemType Directory -Force -Path $buildDir | Out-Null

$isWindows = ($env:OS -eq 'Windows_NT')
$name = [System.IO.Path]::GetFileNameWithoutExtension($src)
$exe = Join-Path $buildDir ($name + $(if ($isWindows) { '.exe' } else { '' }))

$isGcc = $compilerPath -match 'g\+\+'
$isMingw = $compilerPath -match '(?i)(msys|mingw|ucrt64|winlibs)'
$flags = @("-std=$Std", '-O2', '-Wall', '-Wextra', '-Wpedantic', '-Wshadow')
if ($Sanitize) {
    if ($isMingw) {
        # MinGW-GCC ne postavlyaet libasan/libubsan. Vklyuchaem proverki STL.
        Write-Warning 'Sanitizers (ASan/UBSan) nedostupny v etom MinGW-toolchain. Vklyucheny _GLIBCXX_ASSERTIONS; dlya ASan ispolzujte WSL/Linux ili clang.'
        $flags += @('-g', '-O1', '-D_GLIBCXX_ASSERTIONS')
    }
    else {
        $flags += @('-g', '-fno-omit-frame-pointer', '-fsanitize=address,undefined')
    }
}

# Biblioteki - posle ishodnikov (poryadok linkovki vazhen).
$libs = @()
if ($isGcc) {
    # std::print / std::stacktrace nuzhdayutsya v libstdc++exp (GCC >= 14).
    $libs += '-lstdc++exp'
}

$compileArgs = @() + $flags + @($src, '-o', $exe) + $libs
if ($Extra.Trim()) {
    $compileArgs += ($Extra -split '\s+' | Where-Object { $_ })
}
Write-Host ">> $compilerPath $($compileArgs -join ' ')" -ForegroundColor DarkGray
& $compilerPath @compileArgs
if ($LASTEXITCODE -ne 0) {
    throw "Sborka ne udalas' (exit $LASTEXITCODE): $src"
}

if (-not $NoRun) {
    Write-Host ">> $exe" -ForegroundColor DarkGray
    & $exe
    exit $LASTEXITCODE
}
