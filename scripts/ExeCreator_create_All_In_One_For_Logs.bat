@echo off
REM Cambia al directorio donde está este .bat
cd /d "%~dp0"

echo ===============================
echo Compilando create_All_In_One_For_Logs.exe
echo ===============================

powershell -NoProfile -ExecutionPolicy Bypass -Command ^
 "pyinstaller --onefile 'create_All_In_One_For_Logs.py'"

echo.
echo ===============================
echo Proceso terminado
echo ===============================
pause