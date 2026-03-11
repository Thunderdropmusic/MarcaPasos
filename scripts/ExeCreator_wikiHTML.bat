@echo off
REM Cambia al directorio donde está este .bat
cd /d "%~dp0"

echo ===============================
echo Compilando wikiHTML.exe
echo ===============================

powershell -NoProfile -ExecutionPolicy Bypass -Command ^
 "pyinstaller --onefile 'wikiHTML.py'"

echo.
echo ===============================
echo Proceso terminado
echo ===============================
pause