param(
    [Parameter(Mandatory=$true)]
    [string]$GameName,

    [string]$DisplayName
)

# Lint checks
if (-not $DisplayName) {
    $DisplayName = $GameName
}

if ($GameName -notmatch '^[a-zA-Z_]+$') {
    Write-Host "Error: GameName can only contain letters (a-z, A-Z) and underscores (_)"
    exit 1
}

if ($DisplayName -notmatch '^[a-zA-Z_ ]+$') {
    Write-Host "Error: DisplayName can only contain letters (a-z, A-Z), underscores (_), and spaces"
    exit 1
}

if ($DisplayName -match '[\r\n]') {
    Write-Host "Error: DisplayName cannot contain line breaks"
    exit 1
}

$GameNameUpper = $GameName.ToUpper()

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Resolve-Path (Join-Path $ScriptDir '..\..')
$TemplateDir = Join-Path $ProjectRoot 'config\games'
$GamesDir = Join-Path $ProjectRoot 'games'
$TargetDir = Join-Path $GamesDir $GameName

if (Test-Path $TargetDir) {
    Write-Host "Error: Game '$GameName' already exists in $TargetDir"
    exit 1
}

Write-Host "Creating new game: $GameName ($DisplayName)"
Write-Host "Target directory: $TargetDir"
Write-Host ""

New-Item -ItemType Directory -Force -Path $TargetDir | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $TargetDir 'config') | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $TargetDir 'assets') | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $TargetDir 'shaders') | Out-Null

Get-ChildItem -Path $TemplateDir -Recurse -File | ForEach-Object {
    $file = $_.FullName
    $relPath = $file.Substring($TemplateDir.Length + 1)
    $targetFile = Join-Path $TargetDir $relPath
    $targetDirPath = Split-Path $targetFile

    New-Item -ItemType Directory -Force -Path $targetDirPath | Out-Null

    if ($file.EndsWith('.in')) {
        $targetClean = $targetFile.Substring(0, $targetFile.Length - 3)
        (Get-Content $file) -replace '@GAME_NAME@', $GameName `
                            -replace '@GAME_DISPLAY_NAME@', $DisplayName `
                            -replace '@GAME_NAME_UPPER@', $GameNameUpper | 
            Set-Content -Path $targetClean -Encoding UTF8
        Write-Host "  Generated: $targetClean"
    } else {
        Copy-Item -Path $file -Destination $targetFile -Force
        Write-Host "  Copied: $targetFile"
    }
}

Write-Host ""
Write-Host "Game '$GameName' created successfully!"
Write-Host "Location: $TargetDir"
Write-Host ""
Write-Host "To build specific game (native):"
Write-Host "  cmake -B build-native -S ."
Write-Host "  cmake --build build-native --target $GameName"
Write-Host ""
Write-Host "To build specific game (WASM):"
Write-Host "  cmake -B build-wasm -S ."
Write-Host "  cmake --build build-wasm --target ${GameName}_launcher"
Write-Host ""
Write-Host "To build all games:"
Write-Host "  cmake -B build-native -S ."
Write-Host "  cmake --build build-native"
