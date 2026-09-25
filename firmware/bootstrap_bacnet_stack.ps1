$ErrorActionPreference = "Stop"

$Revision = "6bfb0108d4d68835fd0c1062731b54f559cb1375"
$Repository = "https://github.com/bacnet-stack/bacnet-stack.git"
$Vendor = Join-Path $PSScriptRoot "vendor\bacnet-stack"

if (Test-Path $Vendor) {
    if (-not (Test-Path (Join-Path $Vendor ".git"))) {
        throw "$Vendor exists but is not a git checkout; move it aside and rerun this bootstrap."
    }
} else {
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
