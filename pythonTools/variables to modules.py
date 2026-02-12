import re

# Pega aquí tus errores del compilador
log_errores = """
In file included from D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\Subdivision_subdivisiones_compuestas_modules.ino:4:0:
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:15:1: error: expected unqualified-id before 'bool'
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h: In function 'void checkButtons()':
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:33:12: error: 'repeated' was not declared in this scope
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:33:12: note: suggested alternative: 'rename'
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:45:14: error: 'repeated' was not declared in this scope
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:45:14: note: suggested alternative: 'rename'
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:55:14: error: 'repeated' was not declared in this scope
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:55:14: note: suggested alternative: 'rename'
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:65:14: error: 'repeated' was not declared in this scope
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:65:14: note: suggested alternative: 'rename'
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:76:39: error: 'repeated' was not declared in this scope
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:76:39: note: suggested alternative: 'rename'
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:79:12: error: 'repeated' was not declared in this scope
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:79:12: note: suggested alternative: 'rename'
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:91:14: error: 'repeated' was not declared in this scope
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:91:14: note: suggested alternative: 'rename'
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:101:14: error: 'repeated' was not declared in this scope
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:101:14: note: suggested alternative: 'rename'
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:112:39: error: 'repeated' was not declared in this scope
D:\DATOS DEL USUARIO THUNDERDROP\Documents\MEGA\MEGAsync Uploads\ESMUC 2º Any\Labso II\MarcaPasos\research\Subdivision_subdivisiones_compuestas_modules\menus_buttons.h:112:39: note: suggested alternative: 'rename'
exit status 1
"""

def extraer_variables_faltantes(texto):
    # Buscamos el patrón: error: 'variable' was not declared
    patron = r"error: '([^']+)' was not declared in this scope"
    
    # Usamos un set para que no se repitan
    variables = set(re.findall(patron, texto))
    
    print("--- VARIABLES QUE DEBES DECLARAR COMO 'EXTERN' ---")
    for var in sorted(variables):
        print(f"extern TIPO {var}, ")
    print("\n(Nota: Cambia [TIPO] por int, bool, unsigned long, etc.)")

extraer_variables_faltantes(log_errores)