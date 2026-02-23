# Fix UTF-8 BOM encoding issues
Write-Host "Fixing file encoding issues..." -ForegroundColor Green

$files = Get-ChildItem -Path . -Include *.h,*.cpp -Recurse | Where-Object { $_.FullName -notlike "*\bin\*" -and $_.FullName -notlike "*\obj\*" }

foreach ($file in $files) {
    Write-Host "Processing: $($file.FullName)" -ForegroundColor Yellow

    try {
        # Read content
        $content = Get-Content $file.FullName -Raw -Encoding UTF8

        # Write without BOM
        $utf8NoBom = New-Object System.Text.UTF8Encoding $false
        [System.IO.File]::WriteAllText($file.FullName, $content, $utf8NoBom)

        Write-Host "  Fixed!" -ForegroundColor Green
    }
    catch {
        Write-Host "  Error: $_" -ForegroundColor Red
    }
}

Write-Host "`nDone! All files have been converted to UTF-8 without BOM." -ForegroundColor Green
Write-Host "You can now build the solution in Visual Studio." -ForegroundColor Cyan
