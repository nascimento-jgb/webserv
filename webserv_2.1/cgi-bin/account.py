#!/usr/bin/python3
from http import cookies
import os
import cgi
import time
import hashlib
import pickle
import sys
import os

class Session:
    def __init__(self, name):
        self.name = name
        self.sid = hashlib.sha1(str(time.time()).encode("utf-8")).hexdigest()
        session_dir = 'sessions'
        os.makedirs(session_dir, exist_ok=True)
        session_file_path = session_dir + '/session_' + self.sid
        with open(session_file_path, 'wb') as f:
            pickle.dump(self, f)

    def getSid(self):
        return self.sid

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
    print("Content-Type: text/html\r\n\r")
    print("<html>")
    print("<head>")
    print("<title>Account Page</title>")
    print("</head>")
    print("<body>")
    print("<h1>Welcome Again", session.name, "!</h1>")
    print("<p>Your Session ID is: ", session.getSid(), "</p>")
    print("<a href=\"/cgi-bin/account.py?logout=1\">Logout</a>")
    print("</body>")
    print("<a href=\"../index.html\"> Click here to go back to homepage </a>")
    print("</html>")

def printUserMsg(msg):
	print("Content-Type: text/html\r\n\r")
	print("<html>")
	print("<head>")
	print("<title>USER MSG</title>")
	print("</head>")
	print("<body>")
	print("<h1>", msg ,"</h1>")
	print("</body>")
	print("<a href=\"account.py\"> Click here to go back to login page </a>")
	print("</html>")

def printLogin():
    print("Content-Type: text/html\r\n\r")
    print("<!DOCTYPE html>")    
    print("<html lang=\"eng\">")
    print("<head>")
    print("<meta charset=\"UTF-8\">")
    print("<meta name=\"viewport\" content=\"width=device-width, intial-scale=1.0\">")
    print("<title>Login Page</title>")
    print("<!-- Link the external CSS file -->")
    print("<link rel=\"stylesheet\" type=\"text/css\" href=\"../css/styles.css\">")
    print("</head>")
    print("<body>")
    print("<header>")
    print("<!-- Site Name -->")
    print("<h1>CLJ WEBSERV</h1>")
    print("<!-- Main Menu -->")
    print("<nav>")
    print("<ul class=\"nav-pills\">")
    print("<li><a href=\"../index.html\">Home</a></li>")
    print("<li><a href=\"account.py\">Account</a></li>")
    print("<li><a href=\"../services/index.html\">Services</a></li>")
    print("<li><a href=\"../gallery/index.html\">Gallery</a></li>")
    print("</ul>")
    print("</nav>")
    print("</header>")
    print("<main>")
    print("<center> <h1> CLJ Login Form </h1> </center>")
    print("<form action = \"account.py\" method = \"GET\">")
    print("<div class=\"container\">")
    print("<label>Username : </label>")
    print("<input type=\"text\" placeholder=\"Enter Username\" name=\"username\" required>")
    print("<label>Password : </label>")
    print("<input type=\"password\" placeholder=\"Enter Password\" name=\"password\" required>")
    print("<button type=\"submit\">Login</button>")
    print("No Account?<a href=\"../account/register.html\"> Register Here! </a>")
    print("</div>")
    print("</form>")
    print("</main>")
    print("<footer>")
    print("<p>&copy; 2023 CLJ Development Team</p>")
    print("</footer>")
    print("</body>")
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

def handleLogout():
    cookies_header = "Set-Cookie: SID=; expires=Thu, 01 Jan 1970 00:00:00 GMT; path=/cgi-bin\r\n"
    print(cookies_header, end="")
    print("Content-Type: text/html\r\n\r")
    print("<html>")
    print("<head>")
    print("<title>Logout Page</title>")
    print("</head>")
    print("<body>")
    print("<h1>Logout Successful</h1>")
    print("<p>You have been successfully logged out.</p>")
    print("<a href=\"account.py\">Login Again</a>")
    print("</body>")
    print("</html>")

form = cgi.FieldStorage()

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
            cookies.clear()
            cookies["SID"] = session.getSid()
            cookies["SID"]["expires"] = 120 # Session Expires after 2 minutes
            print("HTTP/1.1 302 Found\r")
            print("Content-Type: text/plain\r")
            print(cookies.output())
            print("location: account.py\r")
            print("\r\n\r")
            print("Correct Credentials :D", file=sys.stderr)
    else:
        if os.path.exists('user_database'):
            with open('user_database', 'rb') as f:
                database = pickle.load(f)
                if username in database.user_pass:
                    printUserMsg("Username is already registered!")
                else:
                    database.addUser(username, password, firstname)
                    printUserMsg("Account Registered Successfully!")
        else:
            database = UserDataBase()
            if username in database.user_pass:
                printUserMsg("Username is already registered!")
            else:
                database.addUser(username, password, firstname)
                printUserMsg("Account Registered Successfully!")

if 'HTTP_COOKIE' in os.environ:
    cookies = cookies.SimpleCookie()
    cookies.load(os.environ["HTTP_COOKIE"])
    if "SID" in cookies:
        if "logout" in form:
            cookies["SID"]["expires"] = 0
            handleLogout()
            sys.exit(0)
        else:
            print("Your SPECIAL Session ID is", cookies["SID"].value, file=sys.stderr)
            with open('sessions/session_'+ cookies["SID"].value, 'rb') as f:
                sess = pickle.load(f)
            printAccPage(sess)
    else:
        handleLogin()
else:
    handleLogin()

