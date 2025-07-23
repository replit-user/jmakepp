param(
    [Parameter(Mandatory=$true)]
    [ValidateScript({
        if (-not (Test-Path -Path $_ -IsValid)) {
            throw "Invalid destination directory provided"
        }
        $true
    })]
    [string]$Destination
)

# Ensure Git is available
$gitInstalled = $false
$gitWasPresent = $null -ne (Get-Command git -ErrorAction SilentlyContinue)

if (-not $gitWasPresent) {
    Write-Host "📦 Installing Git via winget..."
    winget install --id Git.Git -e --accept-package-agreements --accept-source-agreements
    $gitInstalled = $true
}

# Ensure g++ is available
if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) {
    Write-Host "📦 Installing g++ via winget..."
    winget install --id "GnuWin32.Gcc" -e --accept-package-agreements --accept-source-agreements
}

# Ensure WSL for cross-compiling to Linux
if (-not (Get-Command wsl -ErrorAction SilentlyContinue)) {
    Write-Host "❌ WSL not found. Linux cross-compilation requires WSL (Windows Subsystem for Linux)."
    Write-Host "Please install WSL with: wsl --install"
}

# Clone repo
$repoUrl = "https://github.com/replit-user/jmakepp"
$tempDir = Join-Path -Path $env:TEMP -ChildPath "jmakepp-clone-$(Get-Date -Format 'yyyyMMddHHmmss')"

try {
    git clone $repoUrl "`"$tempDir`""
    if ($LASTEXITCODE -ne 0) {
        throw "Git clone failed with exit code $LASTEXITCODE"
    }

    $possiblePaths = @(
        "$tempDir\jmake.exe",
        "$tempDir\jmake-main\jmake.exe",
        "$tempDir\jmakepp\jmake.exe",
        "$tempDir\bin\jmake.exe"
    )

    $jmakeppPath = $null
    foreach ($path in $possiblePaths) {
        if (Test-Path -Path $path -PathType Leaf) {
            $jmakeppPath = $path
            break
        }
    }

    if (-not $jmakeppPath) {
        throw "jmakepp.exe not found. Checked: $($possiblePaths -join ', ')"
    }

    if (-not (Test-Path -Path $Destination)) {
        New-Item -ItemType Directory -Path $Destination -Force | Out-Null
    }

    $finalPath = Join-Path $Destination "jmake.exe"
    Copy-Item -Path $jmakeppPath -Destination $finalPath -Force

    Write-Host "✅ jmakepp.exe installed to $finalPath" -ForegroundColor Green
}
finally {
    if (Test-Path $tempDir) {
        Remove-Item -LiteralPath $tempDir -Recurse -Force -ErrorAction SilentlyContinue
    }
}

# Add to PATH if needed
if (-not ($env:Path -split ';' -contains $Destination)) {
    $newPath = [Environment]::GetEnvironmentVariable("Path", "User") + ";" + $Destination
    [Environment]::SetEnvironmentVariable("Path", $newPath, "User")
    $env:Path += ";$Destination"
    Write-Host "🛠️ Added $Destination to PATH (user)" -ForegroundColor Yellow
}

# Uninstall Git if it was auto-installed
if ($gitInstalled) {
    try {
        winget uninstall --id Git.Git -e --accept-source-agreements
        Write-Host "✅ Git uninstalled"
    } catch {
        Write-Warning "⚠️ Git uninstallation failed. Uninstall manually via winget or Control Panel."
    }
}

# Final check
try {
    $jmakeppCmd = Get-Command jmakepp.exe -ErrorAction Stop
    Write-Host "🎉 jmakepp is available at $($jmakeppCmd.Source)" -ForegroundColor Green
    Write-Host "Try it with: jmake version, should output 1.7.3"
} catch {
    Write-Warning "⚠️ jmakepp not found in PATH. You may need to restart the terminal or run it with full path."
}
