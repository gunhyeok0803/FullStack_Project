# Windows Debugging Tools 설치 후 launch.json 자동 설정 스크립트

Write-Host "Windows Debugging Tools 경로 검색 중..." -ForegroundColor Yellow

$searchPaths = @(
    "${env:ProgramFiles(x86)}\Windows Kits\10\Debuggers\x64\cdb.exe",
    "${env:ProgramFiles}\Windows Kits\10\Debuggers\x64\cdb.exe"
)

# 버전별 경로도 검색
$kitVersions = Get-ChildItem "${env:ProgramFiles(x86)}\Windows Kits\10\bin" -ErrorAction SilentlyContinue | Where-Object { $_.PSIsContainer } | Sort-Object Name -Descending
foreach ($version in $kitVersions) {
    $cdbPath = Join-Path $version.FullName "x64\cdb.exe"
    if (Test-Path $cdbPath) {
        $searchPaths += $cdbPath
    }
}

$foundPath = $null
foreach ($path in $searchPaths) {
    if (Test-Path $path) {
        $foundPath = $path
        Write-Host "Found: $foundPath" -ForegroundColor Green
        break
    }
}

if ($foundPath) {
    Write-Host "`nlaunch.json 업데이트 중..." -ForegroundColor Yellow
    
    $launchJsonPath = ".vscode\launch.json"
    if (Test-Path $launchJsonPath) {
        $content = Get-Content $launchJsonPath -Raw
        $escapedPath = $foundPath.Replace('\', '\\')
        
        # miDebuggerPath 업데이트
        $content = $content -replace '("miDebuggerPath":\s*")[^"]*(")', "`$1$escapedPath`$2"
        
        Set-Content $launchJsonPath -Value $content -NoNewline
        Write-Host "launch.json이 업데이트되었습니다!" -ForegroundColor Green
        Write-Host "경로: $foundPath" -ForegroundColor Cyan
    } else {
        Write-Host "launch.json을 찾을 수 없습니다." -ForegroundColor Red
    }
} else {
    Write-Host "`nWindows Debugging Tools를 찾을 수 없습니다." -ForegroundColor Red
    Write-Host "`n설치 방법:" -ForegroundColor Yellow
    Write-Host "1. Visual Studio Installer를 실행하세요" -ForegroundColor Cyan
    Write-Host "2. '수정' 버튼을 클릭하세요" -ForegroundColor Cyan
    Write-Host "3. '개별 구성 요소' 탭을 선택하세요" -ForegroundColor Cyan
    Write-Host "4. 'Windows 10/11 SDK'를 검색하고 '디버깅 도구'를 체크하세요" -ForegroundColor Cyan
    Write-Host "5. '수정' 버튼을 클릭하여 설치하세요" -ForegroundColor Cyan
    Write-Host "`n설치 후 이 스크립트를 다시 실행하세요." -ForegroundColor Yellow
}

