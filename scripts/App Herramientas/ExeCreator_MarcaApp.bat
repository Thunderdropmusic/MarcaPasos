@echo off
title Compilador PyInstaller - AppSecuenciador
REM Cambia al directorio donde está ubicado este archivo .bat
cd /d "%~dp0"

echo ===================================================
echo Compilando AppSecuenciador.exe (Incluyendo Hairless)
echo ===================================================
echo.

pyinstaller --noconsole --onefile --add-data "hairless-midiserial-0.4-windows;hairless-midiserial-0.4-windows" MarcaApp.py

echo.
echo ===================================================
echo Proceso terminado.
echo Tu ejecutable unico esta listo en la carpeta \dist
echo ===================================================
pause