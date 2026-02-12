@echo off
set /p ver="Introduce la version (ej. 1.0): "

echo 1. Limpiando seleccion previa...
git reset

echo 2. Seleccionando SOLO los archivos de la version v%ver%...
:: Usamos comodines por si acaso hay espacios
git add "MarcaPasos v%ver%.pde"
git add "MarcaPasos v%ver%.txt"

echo 3. Creando commit y etiqueta v%ver%...
git commit -m "Publicando version %ver%"
git tag -a v%ver% -m "Release %ver%"

echo 4. Subiendo SOLO esta version...
:: Empujamos la rama main
git push origin main
:: Empujamos SOLAMENTE el tag de esta version, no todos
git push origin v%ver%

echo ¡Hecho!
pause