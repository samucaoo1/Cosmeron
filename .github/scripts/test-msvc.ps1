$ErrorActionPreference = "Stop"

# Keep the Windows suite selection identical to every other CI platform.
$Manifest = ".github/tests.txt"
if (-not (Test-Path $Manifest)) {
  throw "Missing test manifest: $Manifest"
}

function Invoke-CosmeronTest {
  param(
    [string]$Module,
    [string]$Name
  )

  $Directory = Join-Path "Codespace/Tests" $Module
  $Source = Join-Path $Directory "$Name.c"
  if (-not (Test-Path $Source)) {
    throw "Manifest entry does not exist: $Source"
  }

  $Build = Join-Path $Directory ".msvc"
  New-Item -ItemType Directory -Force -Path $Build | Out-Null
  $Executable = Join-Path (Resolve-Path $Build).Path "$Name.exe"

  Push-Location $Directory
  try {
    $CompilerArgs = @(
      "/nologo", "/std:c11", "/Zc:preprocessor", "/W4", "/WX",
      "/wd4127", "/D_CRT_SECURE_NO_WARNINGS", "/D_POSIX_C_SOURCE=200809L",
      "$Name.c", "/Fe:$Executable"
    )
    & cl @CompilerArgs
    if ($LASTEXITCODE -ne 0) {
      throw "cl failed: $Module/$Name"
    }

    & $Executable
    if ($LASTEXITCODE -ne 0) {
      throw "test failed: $Module/$Name ($LASTEXITCODE)"
    }
  } finally {
    Pop-Location
  }
}

Get-Content $Manifest | ForEach-Object {
  $Line = $_.Trim()
  if (-not $Line -or $Line.StartsWith("#")) {
    return
  }

  $Parts = $Line -split "\s+"
  if ($Parts.Count -ne 2) {
    throw "Invalid manifest entry: $Line"
  }

  Invoke-CosmeronTest -Module $Parts[0] -Name $Parts[1]
}
