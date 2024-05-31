from flask import Flask, request, redirect, url_for, render_template
import sqlite3
import os
from PIL import Image

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = 'uploads'
app.config['DATABASE'] = 'images.db'

# Ensure the upload folder exists
if not os.path.exists(app.config['UPLOAD_FOLDER']):
    os.makedirs(app.config['UPLOAD_FOLDER'])

# Initialize the database
def init_db():
    with sqlite3.connect(app.config['DATABASE']) as conn:
        c = conn.cursor()
        c.execute('''
            CREATE TABLE IF NOT EXISTS images (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                filename TEXT NOT NULL,
                rows INTEGER NOT NULL,
                cols INTEGER NOT NULL,
                comps INTEGER NOT NULL,
                data BLOB NOT NULL
            )
        ''')
        conn.commit()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/upload', methods=['POST'])
def upload_file():
    files = request.files.getlist('files[]')
    for file in files:
        if file:
            filename = file.filename
            file_path = os.path.join(app.config['UPLOAD_FOLDER'], filename)
            file.save(file_path)

            with open(file_path, 'rb') as f:
                file_data = f.read()

            im = Image.open(file_path)
            im = im.convert('RGBA')
            pixels = im.tobytes()
            rows, cols = im.size
            comps = 4

            with sqlite3.connect(app.config['DATABASE']) as conn:
                c = conn.cursor()
                c.execute('INSERT INTO images (filename, data, rows, cols, comps) VALUES (?, ?, ?, ?, ?)', (filename, pixels, rows, cols, comps))
                conn.commit()

    return redirect(url_for('index'))

@app.route('/uploads/<filename>')
def uploaded_file(filename):
    with sqlite3.connect(app.config['DATABASE']) as conn:
        c = conn.cursor()
        c.execute('SELECT data FROM images WHERE filename=?', (filename,))
        row = c.fetchone()
        if row:
            return row[0]
    return 'File not found', 404

@app.route('/list')
def list_files():
    with sqlite3.connect(app.config['DATABASE']) as conn:
        c = conn.cursor()
        c.execute('SELECT filename FROM images')
        files = c.fetchall()
    return render_template('list.html', files=files)

@app.route('/mosaify')
def mosaify_files():
    with sqlite3.connect(app.config['DATABASE']) as conn:
        c = conn.cursor()
        c.execute('SELECT filename, data, rows, cols, comps FROM images')
        files = c.fetchall()
        for file in files:
            # print(file)
            filename = file[0]
            data = file[1]
            rows = file[2]
            cols = file[3]
            img = Image.frombytes("RGBA", (rows, cols), data) 
            img.show()
    return render_template('list.html', files=files)


if __name__ == '__main__':
    init_db()
    app.run(debug=True)
