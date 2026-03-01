$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Resolve-Path (Join-Path $ScriptDir '..\..')
$EmsdkBat = Join-Path $ProjectRoot "vendor\emsdk\emsdk_env.bat"

Write-Host "Generating Visual Studio 2026 solution and Emscripten build files..."
Push-Location $ProjectRoot

$envLines = cmd.exe /c "`"$EmsdkBat`" & set"

foreach ($line in $envLines) {
    if ($line -match "^(.*?)=(.*)$") {
        $name = $matches[1]
        $value = $matches[2]
        [System.Environment]::SetEnvironmentVariable($name, $value, "Process")
    }
}

try {
    & emcmake cmake -B build-wasm -G "Ninja Multi-Config"
    & cmake -B build-native -G "Visual Studio 18 2026"
}
catch {
    Write-Host "Failed to generate project files!"
    exit 1
}
finally {
    Pop-Location
}

Write-Host ""
Write-Host "Project generation completed successfully!"
Write-Host "Solution file: build-native\Crimson.sln"
Write-Host "Build files are located in the 'build-native' directory."
Write-Host ""
Write-Host "To develop:"
Write-Host "  1. Open build-native\Crimson.sln in Visual Studio, or"
Write-Host "  2. Open folder in VS Code"
Write-Host ""
Write-Host "To build from command line:"
Write-Host "  cmake --build build-native --config Debug"
Write-Host "  cmake --build build-native --config Release"
Write-Host "WASM building:"
Write-Host "  cmake --build build-wasm --config Debug"
Write-Host "  cmake --build build-wasm --config Release"
Write-Host ""
