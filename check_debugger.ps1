# Windows Debugging Tools 확인 스크립트

Write-Host "Windows Debugging Tools 경로 확인 중..." -ForegroundColor Yellow

$possiblePaths = @(
    "${env:ProgramFiles(x86)}\Windows Kits\10\Debuggers\x64\cdb.exe",
    "${env:ProgramFiles}\Windows Kits\10\Debuggers\x64\cdb.exe",
    "${env:ProgramFiles(x86)}\Windows Kits\10\bin\10.0.*\x64\cdb.exe"
)

$found = $false
foreach ($path in $possiblePaths) {
    if (Test-Path $path) {
        Write-Host "✓ 발견: $path" -ForegroundColor Green
        $found = $true
        break
    }
}

if (-not $found) {
    Write-Host "✗ Windows Debugging Tools가 설치되어 있지 않습니다." -ForegroundColor Red
    Write-Host ""
    Write-Host "설치 방법:" -ForegroundColor Yellow
    Write-Host "1. Visual Studio Installer 실행" -ForegroundColor Cyan
    Write-Host "2. '수정' 클릭" -ForegroundColor Cyan
    Write-Host "3. '개별 구성 요소' 탭 선택" -ForegroundColor Cyan
    Write-Host "4. 'Windows 10/11 SDK' 검색 후 '디버깅 도구' 체크" -ForegroundColor Cyan
    Write-Host "5. '수정' 클릭하여 설치" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "또는 Windows SDK를 직접 다운로드:" -ForegroundColor Yellow
    Write-Host "https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/" -ForegroundColor Cyan
}

