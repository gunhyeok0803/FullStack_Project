# 간단한 실행 스크립트 - 프로젝트 루트에서 실행
$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
if ($projectRoot -eq "") {
    $projectRoot = Get-Location
}

# .exe 파일 목록
$exeFiles = Get-ChildItem -Path $projectRoot -Filter "lec-03-prg-*.exe" | Sort-Object Name

Write-Host "`n=== 실행 가능한 프로그램 ===" -ForegroundColor Cyan
Write-Host ""

$index = 1
$menu = @{}
foreach ($exe in $exeFiles) {
    $menu[$index] = $exe.FullName
    Write-Host "$index. $($exe.Name)" -ForegroundColor Green
    $index++
}

Write-Host ""
$choice = Read-Host "번호를 선택하세요"

if ($menu.ContainsKey([int]$choice)) {
    $exePath = $menu[[int]$choice]
    Write-Host "`n실행 중: $exePath" -ForegroundColor Yellow
    Write-Host ("=" * 60) -ForegroundColor Gray
    Write-Host ""
    
    # 프로젝트 루트로 이동해서 실행
    Push-Location $projectRoot
    & $exePath
    Pop-Location
    
    Write-Host ""
    Write-Host ("=" * 60) -ForegroundColor Gray
} else {
    Write-Host "잘못된 선택입니다." -ForegroundColor Red
}


