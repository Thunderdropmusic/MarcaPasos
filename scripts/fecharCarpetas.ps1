# 1. Definimos la carpeta que queremos organizar (puedes cambiar "research" por el nombre que sea)
$targetFolder = "research"

# Nos movemos a la ubicación del script y luego subimos niveles para llegar a la raíz del proyecto
Set-Location "$PSScriptRoot\.."

# Comprobamos si la carpeta existe antes de seguir
if (Test-Path $targetFolder) {
    Set-Location $targetFolder
    Write-Host "Organizando proyectos dentro de: $((Get-Location).Path)" -ForegroundColor Yellow
} else {
    Write-Host "ERROR: No se encuentra la carpeta '$targetFolder' en: $((Get-Location).Path)" -ForegroundColor Red
    Pause
    exit
}

# 2. Ahora sí, ejecutamos la lógica sobre las subcarpetas de 'research'
Get-ChildItem -Directory | ForEach-Object {
    $currentDirName = $_.Name
    $datePrefix = $_.LastWriteTime.ToString("yyyy-MM-dd")
    
    # Asegurar que la CARPETA tenga la fecha
    if ($currentDirName -notmatch "^\d{4}-\d{2}-\d{2}_") {
        $newName = "$datePrefix`_$currentDirName"
        Rename-Item -Path $_.FullName -NewName $newName
        $currentDirName = $newName
    }

    # Asegurar que el .INO se llame igual que la carpeta actual
    $currentDirPath = Join-Path $_.Parent.FullName $currentDirName
    $inoFiles = Get-ChildItem -Path $currentDirPath -Filter "*.ino"
    
    foreach ($ino in $inoFiles) {
        if ($ino.BaseName -ne $currentDirName) {
            Write-Host "Renombrando $($ino.Name) a $currentDirName.ino" -ForegroundColor Cyan
            Rename-Item -Path $ino.FullName -NewName "$currentDirName.ino"
        }
    }
}

Write-Host "`n¡Organización de $targetFolder completada!" -ForegroundColor Green
Pause