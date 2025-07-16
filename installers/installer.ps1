param(
    [Parameter(Mandatory=$true)]
    [ValidateScript({
        if (-not (Test-Path -Path $_ -IsValid)) {
            throw "Invalid destination path provided"
        }
        $true
    })]
    [string]$Destination
)

# Track if we installed Git
$gitInstalled = $false
$gitWasPresent = $null -ne (Get-Command git -ErrorAction SilentlyContinue)

# Install Git if not already present
if (-not $gitWasPresent) {
    try {
        Write-Host "Installing Git via winget..."
        winget install --id Git.Git -e --accept-package-agreements --accept-source-agreements
        $gitInstalled = $true
    }
    catch {
        throw "Git installation failed: $_"
    }
}

# Verify Git installation
if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    throw "Git not found after installation attempt"
}

# Clone repository
$repoUrl = "https://actual-repository-url/yourproject.git"  # REPLACE WITH ACTUAL URL
$tempDir = Join-Path -Path $env:TEMP -ChildPath "sbuild-clone-$(Get-Date -Format 'yyyyMMddHHmmss')"

try {
    Write-Host "Cloning repository..."
    git clone $repoUrl $tempDir
    
    # Verify sbuild.py exists
    $sbuildPath = Join-Path -Path $tempDir -ChildPath "bin\sbuild.py"
    if (-not (Test-Path -Path $sbuildPath -PathType Leaf)) {
        throw "sbuild.py not found in repository"
    }

    # Create destination directory if needed
    $destDir = [System.IO.Path]::GetDirectoryName($Destination)
    if (-not (Test-Path -Path $destDir)) {
        New-Item -ItemType Directory -Path $destDir -Force | Out-Null
    }

    # Copy with verification
    Write-Host "Copying sbuild.py to $Destination"
    Copy-Item -Path $sbuildPath -Destination $Destination -Force
    
    # Verify copy succeeded
    if (-not (Test-Path -Path $Destination)) {
        throw "Copy operation failed"
    }

    Write-Host "sbuild.py successfully installed to $Destination" -ForegroundColor Green
}
finally {
    # Cleanup temporary files
    if (Test-Path -Path $tempDir) {
        Write-Host "Cleaning up temporary files..."
        Remove-Item -Path $tempDir -Recurse -Force -ErrorAction SilentlyContinue
    }
}

# Add to PATH
$installDir = [System.IO.Path]::GetDirectoryName($Destination)
$pathVar = [Environment]::GetEnvironmentVariable("Path", "User")

if (-not $pathVar.Split(';').Contains($installDir)) {
    Write-Host "Adding $installDir to PATH..."
    $newPath = $pathVar + ";" + $installDir
    [Environment]::SetEnvironmentVariable("Path", $newPath, "User")
    
    # Update current session PATH
    $env:Path += ";$installDir"
    Write-Host "Directory added to PATH for current session" -ForegroundColor Green
}
else {
    Write-Host "$installDir is already in PATH" -ForegroundColor Yellow
}

# Uninstall Git if we installed it
if ($gitInstalled) {
    try {
        Write-Host "Uninstalling Git..."
        winget uninstall --id Git.Git -e --accept-source-agreements
        Write-Host "Git successfully uninstalled" -ForegroundColor Green
    }
    catch {
        Write-Warning "Git uninstallation failed: $_"
        Write-Warning "You may need to uninstall manually: winget uninstall Git.Git"
    }
}