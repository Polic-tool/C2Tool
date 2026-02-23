# Remove Chinese comments from source files
Write-Host "Removing Chinese comments..." -ForegroundColor Green

$files = Get-ChildItem -Path . -Include *.h,*.cpp -Recurse | Where-Object { $_.FullName -notlike "*\bin\*" -and $_.FullName -notlike "*\obj\*" }

foreach ($file in $files) {
    Write-Host "Processing: $($file.FullName)" -ForegroundColor Yellow

    try {
        $content = Get-Content $file.FullName -Raw -Encoding UTF8

        # Remove single-line comments with Chinese characters
        $content = $content -replace '//[^\r\n]*[\u4e00-\u9fa5]+[^\r\n]*', ''

        # Remove multi-line comments with Chinese characters
        $content = $content -replace '/\*[\s\S]*?[\u4e00-\u9fa5]+[\s\S]*?\*/', ''

        # Write as ASCII/UTF-8 without BOM
        $utf8NoBom = New-Object System.Text.UTF8Encoding $false
        [System.IO.File]::WriteAllText($file.FullName, $content, $utf8NoBom)

        Write-Host "  Fixed!" -ForegroundColor Green
    }
    catch {
        Write-Host "  Error: $_" -ForegroundColor Red
    }
}

Write-Host "`nDone!" -ForegroundColor Green
