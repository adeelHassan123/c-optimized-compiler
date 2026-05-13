import os
import subprocess
from flask import Flask, render_template, request, jsonify
from werkzeug.utils import secure_filename

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = 'uploads'
app.config['MAX_CONTENT_LENGTH'] = 16 * 1024 * 1024  # 16MB limit

if not os.path.exists(app.config['UPLOAD_FOLDER']):
    os.makedirs(app.config['UPLOAD_FOLDER'])

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/compile', methods=['POST'])
def compile_code():
    if 'file' not in request.files:
        return jsonify({'error': 'No file part'}), 400
    
    file = request.files['file']
    if file.filename == '':
        return jsonify({'error': 'No selected file'}), 400
    
    if file and file.filename.endswith('.c'):
        filename = secure_filename(file.filename)
        filepath = os.path.join(app.config['UPLOAD_FOLDER'], filename)
        file.save(filepath)
        
        try:
            # Execute the compiler. Path is relative to the project root.
            compiler_path = os.path.join('bin', 'compiler.exe')
            
            # Using subprocess.run to capture output
            result = subprocess.run(
                [compiler_path, filepath],
                capture_output=True,
                text=True,
                check=False # We want to capture errors too
            )
            
            output = result.stdout
            if result.stderr:
                output += "\n--- ERRORS ---\n" + result.stderr
            
            return jsonify({
                'success': True,
                'output': output
            })
            
        except Exception as e:
            return jsonify({'error': str(e)}), 500
        finally:
            # Clean up the uploaded file
            if os.path.exists(filepath):
                os.remove(filepath)
    
    return jsonify({'error': 'Invalid file type. Please upload a .c file.'}), 400

if __name__ == '__main__':
    app.run(debug=True)
