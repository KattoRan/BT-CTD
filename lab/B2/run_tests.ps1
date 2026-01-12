$totalTests = 7
$scannerPath = ".\scanner.exe"

Write-Host "=== BAT DAU KIEM TRA (Che do Line-by-Line) ===" -ForegroundColor Cyan

# Hàm đọc file: Xóa khoảng trắng đầu/cuối dòng, bỏ dòng trống
function Get-CleanLines ($filePath) {
    if (Test-Path $filePath) {
        return Get-Content $filePath | 
               ForEach-Object { $_.Trim() } | 
               Where-Object { $_ -ne "" }
    }
    return @()
}

1..$totalTests | ForEach-Object {
    $id = $_
    $inputFile = "test\example$id.kpl"
    $expectedFile = "test\result$id.txt"
    $myOutputFile = "test\my_output$id.txt"

    if (Test-Path $inputFile) {
        # 1. Chạy scanner, lưu kết quả
        & $scannerPath $inputFile | Out-File $myOutputFile -Encoding utf8

        # 2. Đọc dữ liệu sạch để so sánh
        $expected = Get-CleanLines $expectedFile
        $actual = Get-CleanLines $myOutputFile
        
        # 3. So sánh từng dòng
        $isFail = $false
        $maxLines = [Math]::Max($expected.Count, $actual.Count)

        # Nếu số lượng dòng khác nhau quá nhiều, báo ngay
        if ($expected.Count -ne $actual.Count) {
             # Vẫn tiếp tục so sánh để tìm dòng sai cụ thể
        }

        for ($i = 0; $i -lt $maxLines; $i++) {
            $lineE = if ($i -lt $expected.Count) { $expected[$i] } else { "[Het file]" }
            $lineA = if ($i -lt $actual.Count) { $actual[$i] } else { "[Het file]" }

            if ($lineE -ne $lineA) {
                if (-not $isFail) {
                    Write-Host "Test $id : FAIL" -ForegroundColor Red
                    $isFail = $true
                }
                # In ra dòng bị sai
                Write-Host "   [Sai o dong $($i+1)]" -ForegroundColor Yellow
                Write-Host "      Mong doi: $lineE" -ForegroundColor Gray
                Write-Host "      Thuc te : $lineA" -ForegroundColor Red
                
                # Chỉ in 3 lỗi đầu tiên để tránh spam màn hình nếu sai nhiều
                if ($i -gt 2) { 
                    Write-Host "      ... (Con nua) ..." -ForegroundColor Gray
                    break 
                }
            }
        }

        if (-not $isFail) {
            Write-Host "Test $id : PASS" -ForegroundColor Green
        }
    } else {
        Write-Host "Test $id : Khong tim thay file input" -ForegroundColor DarkGray
    }
}
Write-Host "=== KET THUC ===" -ForegroundColor Cyan