# Simulador de Compilador Pascal
Descripción: Este proyecto es un simulador de compilador en línea para el lenguaje de programación Pascal, diseñado para proporcionar una experiencia de compilación y ejecución interactiva a los estudiantes de programación. Permite a los usuarios escribir, compilar y ejecutar código Pascal directamente desde un navegador web, sin la necesidad de configurar herramientas adicionales en su máquina local.

Objetivo: El simulador tiene como propósito principal ayudar a los estudiantes a comprender mejor el proceso de compilación de un código Pascal, la ejecución del programa y el análisis de errores. Además, permite ver cómo el código se traduce en una representación de bajo nivel y obtener detalles sobre la pila de ejecución.

## Características:
Interfaz Web Intuitiva: Utiliza React y Monaco Editor para ofrecer una experiencia de escritura de código Pascal en un entorno amigable.
Compilación y Ejecución en Tiempo Real: El código Pascal se envía a un servidor backend (Flask) que lo compila y ejecuta. El resultado se muestra inmediatamente en la interfaz.
Visualización de Salida SM y Pila: Después de la ejecución, se muestran dos archivos de salida importantes: la salida SM (máquina virtual) y el contenido de la pila de ejecución, que ayudan a los estudiantes a entender cómo se maneja la memoria durante la ejecución del programa.
Mensajes de Error: Si el código tiene errores, se devuelve un mensaje detallado que describe el problema, lo que permite a los estudiantes aprender de sus errores y corregir el código.
Soporte para Ciclos y Funciones: El simulador soporta estructuras comunes de Pascal como ciclos for, if, y llamadas a funciones, con capacidad para ejecutar programas con múltiples bloques de código anidados.
Tecnologías Utilizadas:
Frontend:

React: Para crear una interfaz interactiva y reactiva.
Monaco Editor: Un editor de código que permite a los usuarios escribir código Pascal con características como autocompletado, resaltado de sintaxis y más.
CSS (Flexbox y Grid): Para el diseño de la interfaz.
Backend:

Flask: Un micro-framework de Python para manejar las peticiones HTTP, gestionar la compilación del código y la ejecución de los programas Pascal.
Subprocess: Para ejecutar los comandos de compilación y ejecución en el servidor, que llaman al compilador de C++ para transformar el código Pascal en código ejecutable.

## Guia:

Para deployar el backend ejecutar :  
cd backend
python app.py
Para deployar el frontend ejecutar :  
cd frontend
npm install
npm install next
npm run dev

Ejecutar codigo :  
Dirigirse a la carpeta compiler (cd compiler)
Comando para ejecutar el program y generar  el sm :
g++ -o out exp.cpp parser.cpp environment.hh imp_codegen.cpp 
    imp_type_checker.cpp scanner.cpp token.cpp visitor.cpp main.cpp 
    imp_type.cpp imp_interpreter.cpp imp_value.cpp imp_value_visitor.hh

Comando para ejecutar el sm y obtener la maqina de pila :
g++ -o svm svm.cpp svm_parser.cpp svm_run.cpp

el resultado de la pila se encuentra en :  compiler/pila_output

Lenguaje de Programación:
Pascal: El lenguaje objetivo que se compila y ejecuta en el simulador.
![Descripción de la imagen](./Screenshot%202024-11-30%20054256.png)
