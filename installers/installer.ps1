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

# Clone repository - using quoted paths for spaces
$repoUrl = "https://github.com/replit-user/sbuild.git"
$tempDir = Join-Path -Path $env:TEMP -ChildPath "sbuild-clone-$(Get-Date -Format 'yyyyMMddHHmmss')"

try {
    Write-Host "Cloning repository from $repoUrl..."
    
    # Use quoted paths to handle spaces
    git clone $repoUrl "`"$tempDir`""
    if ($LASTEXITCODE -ne 0) {
        throw "Git clone failed with exit code $LASTEXITCODE"
    }
    
    # Verify sbuild.py exists - handle repository structure
    $possiblePaths = @(
        (Join-Path -Path $tempDir -ChildPath "sbuild.py"),
        (Join-Path -Path $tempDir -ChildPath "sbuild-main\sbuild.py"),
        (Join-Path -Path $tempDir -ChildPath "sbuild\sbuild.py"),
    (Join-Path -Path $tempDir -ChildPath "\bin\sbuild.py")
    )
    
    $sbuildPath = $null
    foreach ($path in $possiblePaths) {
        if (Test-Path -Path $path -PathType Leaf) {
            $sbuildPath = $path
            break
        }
    }
    
    if (-not $sbuildPath) {
        throw "sbuild.py not found in repository. Checked locations: $($possiblePaths -join ', ')"
    }

    # Create destination directory if needed
    if (-not (Test-Path -Path $Destination -PathType Container)) {
        New-Item -ItemType Directory -Path $Destination -Force | Out-Null
    }

    # Copy sbuild.py to destination directory
    $finalPath = Join-Path -Path $Destination -ChildPath "sbuild.py"
    Write-Host "Copying sbuild.py to $finalPath"
    Copy-Item -Path $sbuildPath -Destination $finalPath -Force
    
    # Verify copy succeeded
    if (-not (Test-Path -Path $finalPath -PathType Leaf)) {
        throw "Copy operation failed. Source: $sbuildPath, Destination: $finalPath"
    }

    Write-Host "sbuild.py successfully installed to $finalPath" -ForegroundColor Green
}
finally {
    # Cleanup temporary files with error handling
    if (Test-Path -Path $tempDir) {
        try {
            Write-Host "Cleaning up temporary files..."
            Remove-Item -LiteralPath $tempDir -Recurse -Force -ErrorAction Stop
        }
        catch {
            Write-Warning "Cleanup failed: $_"
            Write-Warning "Please manually remove: $tempDir"
        }
    }
}

# Add to PATH
if (-not ($env:Path -split ';' -contains $Destination)) {
    Write-Host "Adding $Destination to user PATH..."
    $newPath = [Environment]::GetEnvironmentVariable("Path", "User") + ";" + $Destination
    [Environment]::SetEnvironmentVariable("Path", $newPath, "User")
    
    # Update current session PATH
    $env:Path += ";$Destination"
    Write-Host "Directory added to PATH for current session" -ForegroundColor Green
}
else {
    Write-Host "$Destination is already in PATH" -ForegroundColor Yellow
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
    Write-Host "Try it with: sbuild.py help"
}
catch {
    Write-Warning "sbuild.py is not in PATH. You may need to start a new terminal session."
    Write-Warning "Alternatively, run manually with: $(Join-Path $Destination 'sbuild.py')"
}
