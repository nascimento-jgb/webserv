import sys
input_data = sys.stdin.read()
print("Content-Type: text/html\r\n\r")
print("<html>")
print("<head>")
print("<title>Input Printer</title>")
print("</head>")
print("<body>")
print("<h1>What the user wrote:</h1>")
print("<p>")
print(input_data)
print("</p>")
print("</body>")
print("</html>")
