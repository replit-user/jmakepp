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
$gitWasPresent = (Get-Command git -ErrorAction SilentlyContinue) -ne $null

# Install Git if not already present
if (-not $gitWasPresent) {
    try {
        Write-Host "Installing Git via winget..."
        winget install --id Git.Git -e --accept-package-agreements --accept-source-agreements
        $gitInstalled = $true
        
        # Refresh PATH to ensure Git is available
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path", "Machine") + ";" + 
                    [System.Environment]::GetEnvironmentVariable("Path", "User")
    }
    catch {
        throw "Git installation failed: $_"
    }
}

# Verify Git installation
if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    throw "Git not found after installation. Please ensure Git is in your PATH."
}

# Clone repository
$repoUrl = "https://github.com/replit-user/sbuild.git"
$tempDir = Join-Path -Path $env:TEMP -ChildPath "sbuild-clone-$(Get-Date -Format 'yyyyMMddHHmmss')"

try {
    Write-Host "Cloning repository from $repoUrl..."
    git clone $repoUrl $tempDir
    
    # Verify sbuild.py exists (now in root directory)
    $sbuildPath = Join-Path -Path $tempDir -ChildPath "sbuild.py"
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
    Write-Host "Adding $installDir to user PATH..."
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
        
        # Refresh PATH after uninstallation
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path", "Machine") + ";" + 
                    [System.Environment]::GetEnvironmentVariable("Path", "User")
        
        Write-Host "Git successfully uninstalled" -ForegroundColor Green
    }
    catch {
        Write-Warning "Git uninstallation failed: $_"
        Write-Warning "You may need to uninstall manually: winget uninstall Git.Git"
    }
}

# Final verification
try {
    $sbuildCmd = Get-Command sbuild.py -ErrorAction Stop
    Write-Host "Verification successful: sbuild.py is available at $($sbuildCmd.Source)" -ForegroundColor Green
}
catch {
    Write-Warning "sbuild.py is not in PATH. You may need to start a new terminal session."
    Write-Warning "Alternatively, run manually with: $Destination"
}
