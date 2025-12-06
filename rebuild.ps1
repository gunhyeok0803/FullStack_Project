# 강제 재빌드 스크립트 - 실행 중인 프로세스 종료 후 빌드
param(
    [string]$FileName = ""
)

$vcvarsPath = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

# 파일명이 지정되지 않으면 현재 열려있는 파일 또는 모든 파일 빌드
if ($FileName -eq "") {
    Write-Host "사용법: .\rebuild.ps1 -FileName 'lec-03-prg-08-tcp-echo-server-multithread-chat'" -ForegroundColor Yellow
    Write-Host "또는: .\rebuild.ps1 -FileName 'all'  # 모든 파일 재빌드" -ForegroundColor Yellow
    exit 1
}

if ($FileName -eq "all") {
    # 모든 파일 재빌드
    Write-Host "모든 .exe 파일 재빌드 중..." -ForegroundColor Cyan
    
    # 실행 중인 프로세스 종료
    Get-Process | Where-Object { $_.Path -like "*FullStack_Project*lec-03-prg-*.exe" } | ForEach-Object {
        Write-Host "프로세스 종료: $($_.ProcessName) (PID: $($_.Id))" -ForegroundColor Yellow
        Stop-Process -Id $_.Id -Force -ErrorAction SilentlyContinue
    }
    
    Start-Sleep -Milliseconds 500
    
    # 모든 .cpp 파일 빌드
    $cppFiles = Get-ChildItem -Path . -Filter "lec-03-prg-*.cpp" | Sort-Object Name
    
    foreach ($cppFile in $cppFiles) {
        $exeName = $cppFile.BaseName + ".exe"
        $exePath = Join-Path $cppFile.DirectoryName $exeName
        
        Write-Host "`n빌드 중: $($cppFile.Name) -> $exeName" -ForegroundColor Yellow
        
        # 기존 .exe 파일 삭제
        if (Test-Path $exePath) {
            Remove-Item $exePath -Force -ErrorAction SilentlyContinue
        }
        
        # 빌드 실행
        $buildCmd = "cl.exe /Zi /EHsc /nologo /Fe`"$exePath`" `"$($cppFile.FullName)`" ws2_32.lib"
        & cmd /c "`"$vcvarsPath`" && $buildCmd"
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✓ 성공: $exeName" -ForegroundColor Green
        } else {
            Write-Host "✗ 실패: $($cppFile.Name)" -ForegroundColor Red
        }
    }
    
    Write-Host "`n빌드 완료!" -ForegroundColor Green
} else {
    # 단일 파일 재빌드
    $cppFile = Get-Item "$FileName.cpp" -ErrorAction SilentlyContinue
    
    if (-not $cppFile) {
        Write-Host "파일을 찾을 수 없습니다: $FileName.cpp" -ForegroundColor Red
        exit 1
    }
    
    $exeName = $cppFile.BaseName + ".exe"
    $exePath = Join-Path $cppFile.DirectoryName $exeName
    
    Write-Host "재빌드 중: $($cppFile.Name) -> $exeName" -ForegroundColor Cyan
    
    # 실행 중인 프로세스 종료
    $processes = Get-Process | Where-Object { $_.Path -like "*$exeName" }
    foreach ($proc in $processes) {
        Write-Host "프로세스 종료: $($proc.ProcessName) (PID: $($proc.Id))" -ForegroundColor Yellow
        Stop-Process -Id $proc.Id -Force -ErrorAction SilentlyContinue
    }
    
    Start-Sleep -Milliseconds 500
    
    # 기존 .exe 파일 삭제
    if (Test-Path $exePath) {
        Remove-Item $exePath -Force -ErrorAction SilentlyContinue
        Write-Host "기존 .exe 파일 삭제 완료" -ForegroundColor Gray
    }
    
    # 빌드 실행
    $buildCmd = "cl.exe /Zi /EHsc /nologo /Fe`"$exePath`" `"$($cppFile.FullName)`" ws2_32.lib"
    Write-Host "빌드 명령 실행 중..." -ForegroundColor Yellow
    & cmd /c "`"$vcvarsPath`" && $buildCmd"
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "`n✓ 빌드 성공: $exeName" -ForegroundColor Green
    } else {
        Write-Host "`n✗ 빌드 실패" -ForegroundColor Red
        Write-Host "에러 코드: $LASTEXITCODE" -ForegroundColor Red
    }
}

