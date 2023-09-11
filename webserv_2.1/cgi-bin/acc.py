#! /usr/bin/python3
from http import cookies
import os
import cgi
import time
import hashlib
import pickle
import sys
import os

# -fsanitize=address -g -static-libsan
class Session:
	def __init__(self, name):
		self.name = name
		self.sid = hashlib.sha1(str(time.time()).encode("utf-8")).hexdigest()
		session_dir = 'sessions'
		os.makedirs(session_dir, exist_ok=True)  # This line creates the directory if it doesn't exist
		session_file_path = session_dir + '/session_' + self.sid
		print("Session ID:", self.sid)
		print("Session File Path:", session_file_path)
		with open(session_file_path, 'wb') as f:
			pickle.dump(self, f)
	def getSid(self):
		return self.sid

""" Stores Users and their data """
class UserDataBase:
	def __init__(self):
		self.user_pass = {}
		self.user_firstname = {}
	def addUser(self, username, password, firstname):
		self.user_pass[username] = password
		self.user_firstname[username] = firstname
		with open('user_database', 'wb') as f:
			pickle.dump(self, f)

def printAccPage(session):
	print("Content-Type: text/html\r\n")
	print("<html>")
	print("<head>")
	print("<title>Account Page</title>")
	print("</head>")
	print("<body>")
	print("<h1>Welcome Again", session.name, "!</h1>")
	print("<p>Your Session ID is: ", session.getSid(), "</p>")
	print("</body>")
	print("<a href=\"../index.html\"> Click here to go back to homepage </a>")
	print("</html>")

def printUserMsg(msg):
	print("Content-Type: text/html\r\n")
	print("<html>")
	print("<head>")
	print("<title>USER MSG</title>")
	print("</head>")
	print("<body>")
	print("<h1>", msg ,"</h1>")
	print("</body>")
	print("<a href=\"/account/index.html\"> Click here to go back to login page </a>")
	print("</html>")

def printLogin():
	print("Content-Type: text/html\r\n")
	print("<html> ")
	print("<head>")
	print("<meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\">")
	print("<link rel=\"stylesheet\" href=\"../css/accstyle.css\">")
	print("<title> Login Page </title>")
	print("</head>")
	print("<body>  ")
	print("<center> <h1> CLJ Login Form </h1> </center> ")
	print("<form action = \"../../cgi-bin/acc.py\" method = \"GET\">")
	print("<div class=\"container\"> ")
	print("<label>Username : </label> ")
	print("<input type=\"text\" placeholder=\"Enter Username\" name=\"username\" required>")
	print("<label>Password : </label> ")
	print("<input type=\"password\" placeholder=\"Enter Password\" name=\"password\" required>")
	print("<button type=\"submit\">Login</button> ")
	print("No Account?<a href=\"/register.html\"> Register Here </a> ")
	print("</div> ")
	print("</form>   ")
	print("</body>   ")
	print("</html>")

def authUser(name, password):
	if os.path.exists('user_database'):
		with open('user_database', 'rb') as f:
			database = pickle.load(f)
			if name in database.user_pass and database.user_pass[name] == password:
				session = Session(database.user_firstname[name])
				return session
			else:
				return None
	else:
		return None

def handleLogin():
	username = form.getvalue('username')
	password = form.getvalue('password')
	firstname = form.getvalue('firstname')
	if username == None:
		printLogin()
	elif firstname == None:
		session = authUser(form.getvalue('username'), form.getvalue('password'))
		if(session == None):
			printUserMsg("Failed To Login, Username or Password is wrong!")
		else:
			print("Correct Crenditals :D", file=sys.stderr)
			cookies.clear()
			cookies["SID"] = session.getSid()
			cookies["SID"]["expires"] = 120 # Session Expires after 2 mins
			print("HTTP/1.1 200 OK")
			print(cookies.output())
			print("location: acc.py")
			print("\r\n")
	else :
		if os.path.exists('user_database'):
			with open('user_database', 'rb') as f:
				database = pickle.load(f)
				if username in database.user_pass:
					printUserMsg("Username is already Registerd !")
				else:
					database.addUser(username, password, firstname)
					printUserMsg("Account Registered Successfully!")
		else:
			database = UserDataBase()
			if username in database.user_pass:
				printUserMsg("Username is already Registerd !")
			else:
				database.addUser(username, password, firstname)
				printUserMsg("Account Registered Successfully!")

form = cgi.FieldStorage()
if 'HTTP_COOKIE' in os.environ:
	cookies = cookies.SimpleCookie()
	cookies.load(os.environ["HTTP_COOKIE"])

	if "SID" in cookies:
		print("Your Session ID is", cookies["SID"].value,file=sys.stderr)
		with open('sessions/session_'+ cookies["SID"].value, 'rb') as f:
			sess = pickle.load(f)
		printAccPage(sess)
	else:
		handleLogin()
else:
	handleLogin()
