$ErrorActionPreference = "Stop"

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$Revision = (Get-Content (Join-Path $Root "BACNET_STACK_REVISION") -Raw).Trim()
$Repository = "https://github.com/bacnet-stack/bacnet-stack"
$Vendor = Join-Path $Root "vendor\bacnet-stack"

if ([string]::IsNullOrWhiteSpace($Revision) -or $Revision -notmatch "^[0-9a-fA-F]{40}$") {
    throw "Invalid BACNET_STACK_REVISION: $Revision"
}

if (Test-Path $Vendor) {
    if (-not (Test-Path (Join-Path $Vendor ".git"))) {
        throw "$Vendor exists but is not a Git checkout; move it aside and retry."
    }
    $Remote = (git -C $Vendor remote get-url origin 2>$null).Trim()
    if ($Remote -ne $Repository) {
        throw "Unexpected upstream remote: $Remote"
    }
} else {
    New-Item -ItemType Directory -Force -Path (Join-Path $Root "vendor") | Out-Null
    git init $Vendor | Out-Host
    git -C $Vendor remote add origin $Repository
}

git -C $Vendor fetch --depth 1 origin $Revision | Out-Host
git -C $Vendor -c advice.detachedHead=false checkout --detach $Revision | Out-Host
$Actual = (git -C $Vendor rev-parse HEAD).Trim()
if ($Actual -ne $Revision) {
    throw "bacnet-stack revision mismatch: expected $Revision, got $Actual"
}
Write-Host "bacnet-stack pinned at $Actual"
