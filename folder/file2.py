from flask import Flask, request, session

app = Flask(name)
app.secret_key = 'your_secret_key'  # Set a secret key for session encryption


@app.route('/', methods=['GET', 'POST'])
def counter():
    if request.method == 'POST' and 'increment' in request.form:
        if 'counter' in session:
            session['counter'] += 1
        else:
            session['counter'] = 1
            print("Counter set to 1")  # Debug statement

    counter_value = session.get('counter', 0)
    return """
        <html>
        <head>
            <title>Increment Counter Example</title>
        </head>
        <body>
            <h1>Counter: {}</h1>
            <form method="POST">
                <button type="submit" name="increment">Increment Counter</button>
            </form>
        </body>
        </html>
    """.format(counter_value)


if name == 'main':
    try:
        app.run()
    except Exception as e:
        print("An error occurred:", str(e))