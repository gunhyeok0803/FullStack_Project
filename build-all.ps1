# 모든 .cpp 파일을 개별 실행 파일로 빌드하는 스크립트
$vcvarsPath = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

# vcvars64.bat를 실행하여 환경 변수 설정
& cmd /c "`"$vcvarsPath`" && set" | ForEach-Object {
    if ($_ -match '^([^=]+)=(.*)$') {
        [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2])
    }
}

# 현재 디렉토리의 모든 .cpp 파일 찾기
$cppFiles = Get-ChildItem -Path . -Filter "*.cpp" | Where-Object { $_.Name -notlike "*FullStack*" }

Write-Host "빌드할 파일 개수: $($cppFiles.Count)" -ForegroundColor Green

foreach ($cppFile in $cppFiles) {
    $exeName = $cppFile.BaseName + ".exe"
    $exePath = Join-Path $cppFile.DirectoryName $exeName
    
    Write-Host "`n빌드 중: $($cppFile.Name) -> $exeName" -ForegroundColor Yellow
    
    # 기존 .exe 파일이 있으면 삭제
    if (Test-Path $exePath) {
        Remove-Item $exePath -Force
    }
    
    # 빌드 실행
    $buildCmd = "cl.exe /Zi /EHsc /nologo /Fe`"$exePath`" `"$($cppFile.FullName)`" ws2_32.lib"
    
    & cmd /c "$buildCmd"
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ 성공: $exeName" -ForegroundColor Green
    } else {
        Write-Host "✗ 실패: $($cppFile.Name)" -ForegroundColor Red
    }
}

Write-Host "`n빌드 완료!" -ForegroundColor Green


