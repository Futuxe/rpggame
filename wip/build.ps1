# Build Emberfall v2 — concatenates the wip/p01..p20 parts into index.html
$ErrorActionPreference = 'Stop'
$parts = Get-ChildItem "$PSScriptRoot\p*.html" | Sort-Object Name
Write-Host "Parts:"; $parts | ForEach-Object { Write-Host "  $($_.Name)" }
$out = ($parts | ForEach-Object { [IO.File]::ReadAllText($_.FullName) }) -join "`n"
$dest = Join-Path (Split-Path $PSScriptRoot) 'index.html'
[IO.File]::WriteAllText($dest, $out, (New-Object System.Text.UTF8Encoding($false)))
Write-Host "Wrote $dest ($([math]::Round((Get-Item $dest).Length/1KB,1)) KB)"
