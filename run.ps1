# 프로젝트 루트 디렉토리 찾기
$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Path
if ($scriptPath -eq "") {
    $scriptPath = Get-Location
}

# .cpp 파일 목록 가져오기 (프로젝트 루트에서)
$cppFiles = Get-ChildItem -Path $scriptPath -Filter "*.cpp" | Where-Object { $_.Name -notlike "*FullStack*" } | Sort-Object Name

param(
    [string]$FileName = ""
)

if ($FileName -eq "") {
    Write-Host "`n=== 실행 가능한 프로그램 목록 ===" -ForegroundColor Cyan
    Write-Host ""
    
    $index = 1
    foreach ($file in $cppFiles) {
        $exeName = $file.BaseName + ".exe"
        $exeExists = Test-Path $exeName
        
        if ($exeExists) {
            Write-Host "[$index] $($file.Name) -> $exeName" -ForegroundColor Green
        } else {
            Write-Host "[$index] $($file.Name) -> $exeName (빌드 필요)" -ForegroundColor Yellow
        }
        $index++
    }
    
    Write-Host ""
    $choice = Read-Host "실행할 프로그램 번호를 입력하세요 (또는 파일명)"
    
    # 번호로 선택한 경우
    if ($choice -match '^\d+$') {
        $fileIndex = [int]$choice - 1
        if ($fileIndex -ge 0 -and $fileIndex -lt $cppFiles.Count) {
            $selectedFile = $cppFiles[$fileIndex]
        } else {
            Write-Host "잘못된 번호입니다." -ForegroundColor Red
            exit 1
        }
    } else {
        # 파일명으로 선택한 경우
        $selectedFile = $cppFiles | Where-Object { $_.Name -like "*$choice*" } | Select-Object -First 1
        if (-not $selectedFile) {
            Write-Host "파일을 찾을 수 없습니다." -ForegroundColor Red
            exit 1
        }
    }
} else {
    # 파일명이 인자로 전달된 경우
    $selectedFile = $cppFiles | Where-Object { $_.Name -like "*$FileName*" } | Select-Object -First 1
    if (-not $selectedFile) {
        Write-Host "파일을 찾을 수 없습니다: $FileName" -ForegroundColor Red
        exit 1
    }
}

$exeName = $selectedFile.BaseName + ".exe"
$exePath = Join-Path $scriptPath $exeName

# .exe 파일이 없으면 빌드
if (-not (Test-Path $exePath)) {
    Write-Host "`n빌드 중: $($selectedFile.Name)..." -ForegroundColor Yellow
    
    $vcvarsPath = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    $buildCmd = "cl.exe /Zi /EHsc /nologo /Fe`"$exePath`" `"$($selectedFile.FullName)`" ws2_32.lib"
    Write-Host "빌드 명령: $buildCmd" -ForegroundColor Gray
    
    & cmd /c "`"$vcvarsPath`" && $buildCmd"
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "빌드 실패!" -ForegroundColor Red
        exit 1
    }
    
    Write-Host "빌드 완료!" -ForegroundColor Green
}

# 프로그램 실행
Write-Host "`n실행 중: $exeName" -ForegroundColor Cyan
Write-Host "=" * 50 -ForegroundColor Gray
Write-Host ""

& $exePath

Write-Host ""
Write-Host "=" * 50 -ForegroundColor Gray
Write-Host "프로그램 종료 (종료 코드: $LASTEXITCODE)" -ForegroundColor Gray

