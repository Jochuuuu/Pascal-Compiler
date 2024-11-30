import os
import subprocess
from flask import Flask, request, jsonify
from flask_cors import CORS
from concurrent.futures import ThreadPoolExecutor

app = Flask(__name__)
CORS(app)

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
COMPILER_FOLDER = os.path.join(BASE_DIR, '../compiler')
INPUT_FILE = os.path.join(COMPILER_FOLDER, 'input.txt')
SM_FILE = os.path.join(COMPILER_FOLDER, 'input.txt.sm')
PILA_OUTPUT_FILE = os.path.join(COMPILER_FOLDER, 'pila_output.txt')  

executor = ThreadPoolExecutor()

COMPILE_COMMAND = (
    "g++ -o out exp.cpp parser.cpp environment.hh imp_codegen.cpp "
    "imp_type_checker.cpp scanner.cpp token.cpp visitor.cpp main.cpp "
    "imp_type.cpp imp_interpreter.cpp imp_value.cpp imp_value_visitor.hh"
)
EXECUTE_COMMAND = "out input.txt"
SVM_COMPILE_COMMAND = "g++ -o svm svm.cpp svm_parser.cpp svm_run.cpp"
SVM_EXECUTE_COMMAND = "./svm input.txt.sm"


def compile_and_execute(pascal_code):
    try:
        with open(INPUT_FILE, 'w') as file:
            file.write(pascal_code)

        compile_result = subprocess.run(
            COMPILE_COMMAND, shell=True, capture_output=True, cwd=COMPILER_FOLDER, text=True
        )
        if compile_result.returncode != 0:
            return {"error": compile_result.stderr}

        execute_result = subprocess.run(
            EXECUTE_COMMAND, shell=True, capture_output=True, cwd=COMPILER_FOLDER, text=True
        )
        if execute_result.returncode != 0:
            return {"error": execute_result.stderr}

        sm_output = ""
        if os.path.exists(SM_FILE):
            with open(SM_FILE, 'r') as sm_file:
                sm_output = sm_file.read()

        return {"output": execute_result.stdout, "sm_output": sm_output}

    except Exception as e:
        return {"error": str(e)}


def generate_stack_output():
    try:
        svm_compile_result = subprocess.run(
            SVM_COMPILE_COMMAND, shell=True, capture_output=True, cwd=COMPILER_FOLDER, text=True
        )
        if svm_compile_result.returncode != 0:
            return {"error": svm_compile_result.stderr}

        svm_execute_command = "svm.exe input.txt.sm" if os.name == "nt" else "./svm input.txt.sm"
        svm_execute_result = subprocess.run(
            svm_execute_command, shell=True, capture_output=True, cwd=COMPILER_FOLDER, text=True
        )
        if svm_execute_result.returncode != 0:
            return {"error": svm_execute_result.stderr}

        stack_output = ""
        if os.path.exists(PILA_OUTPUT_FILE):
            with open(PILA_OUTPUT_FILE, 'r') as pila_file:
                stack_output = pila_file.read()

        return {"stack_output": stack_output}

    except Exception as e:
        return {"error": str(e)}



@app.route('/compile', methods=['POST'])
def compile_pascal():
    try:
        data = request.json
        pascal_code = data.get("code", "")

        if not pascal_code.strip():
            return jsonify({"error": "El código Pascal está vacío"}), 400

        future = executor.submit(compile_and_execute, pascal_code)
        result = future.result()

        if "error" in result:
            return jsonify(result), 500

        return jsonify(result)

    except Exception as e:
        return jsonify({"error": str(e)}), 500


@app.route('/stack', methods=['GET'])
def get_stack_output():
    try:
        future = executor.submit(generate_stack_output)
        result = future.result()
        print(result)

        if "error" in result:
            return jsonify(result), 500

        return jsonify(result)

    except Exception as e:
        return jsonify({"error": str(e)}), 500


if __name__ == '__main__':
    app.run(debug=True)
