# 모든 .cpp 파일을 빠르게 실행할 수 있는 메뉴 스크립트
$cppFiles = Get-ChildItem -Path . -Filter "*.cpp" | Where-Object { $_.Name -notlike "*FullStack*" } | Sort-Object Name

Write-Host "`n=== 빠른 실행 메뉴 ===" -ForegroundColor Cyan
Write-Host ""

$menu = @{}
$index = 1
foreach ($file in $cppFiles) {
    $exeName = $file.BaseName + ".exe"
    $exeExists = Test-Path $exeName
    
    $menu[$index] = @{
        File = $file
        ExeName = $exeName
        ExeExists = $exeExists
    }
    
    if ($exeExists) {
        Write-Host "$index. $($file.BaseName)" -ForegroundColor Green
    } else {
        Write-Host "$index. $($file.BaseName) (빌드 필요)" -ForegroundColor Yellow
    }
    $index++
}

Write-Host ""
Write-Host "0. 종료" -ForegroundColor Gray
Write-Host ""

while ($true) {
    $choice = Read-Host "선택"
    
    if ($choice -eq "0") {
        break
    }
    
    if ($menu.ContainsKey([int]$choice)) {
        $selected = $menu[[int]$choice]
        $file = $selected.File
        $exeName = $selected.ExeName
        $exePath = Join-Path $file.DirectoryName $exeName
        
        # .exe 파일이 없으면 빌드
        if (-not $selected.ExeExists) {
            Write-Host "`n빌드 중: $($file.Name)..." -ForegroundColor Yellow
            
            $vcvarsPath = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
            $buildCmd = "cl.exe /Zi /EHsc /nologo /Fe`"$exePath`" `"$($file.FullName)`" ws2_32.lib"
            
            & cmd /c "`"$vcvarsPath`" && $buildCmd" | Out-Null
            
            if ($LASTEXITCODE -ne 0) {
                Write-Host "빌드 실패!" -ForegroundColor Red
                continue
            }
            
            Write-Host "빌드 완료!" -ForegroundColor Green
        }
        
        # 프로그램 실행
        Write-Host "`n실행 중: $exeName" -ForegroundColor Cyan
        Write-Host ("=" * 50) -ForegroundColor Gray
        Write-Host ""
        
        & $exePath
        
        Write-Host ""
        Write-Host ("=" * 50) -ForegroundColor Gray
        Write-Host "프로그램 종료. 계속하려면 아무 키나 누르세요..." -ForegroundColor Gray
        $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
        Write-Host ""
    } else {
        Write-Host "잘못된 선택입니다." -ForegroundColor Red
    }
}


