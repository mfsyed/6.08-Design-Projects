import requests
import math
import pprint
import sqlite3
import random
import string



color_db = '__HOME__/Lamp/lamp.db'


color_url = "http://608dev-2.net/sandbox/sc/mfsyed/Lamp/lamp.html" #where I post stuff


def request_handler(request):

    if request["method"] == "POST": #getting colors/user name
        if not request["form"]["user"]: #checking if there is no username because then I'll give error
            return "ERROR! Type in a username."
        else:
            user = request["form"]["user"]

        try:
            int(request["form"]["red"]) #checking if color's a valid int
        except:
            return "ERROR! Type in a valid int."
        if int(request["form"]["red"]) < 0: # checking if color val is less than 0
            red = 0
        elif int(request["form"]["red"]) > 255: #checking if color val is greater than 255
            red = 255
        else:
            red = int(request["form"]["red"])

        try:
            int(request["form"]["blue"])
        except:
            return "ERROR! Type in a valid int."
        if not request["form"]["blue"] or int(request["form"]["blue"]) < 0:
            blue = 0
        elif int(request["form"]["blue"]) > 255:
            blue = 255
        else:
            blue = int(request["form"]["blue"])


        try:
            int(request["form"]["green"])
        except:
            return "ERROR! Type in a valid int."

        if not request["form"]["green"] or int(request["form"]["green"]) < 0:
            green = 0
        elif int(request["form"]["green"]) > 255:
            green = 255
        else:
            green = int(request["form"]["green"])


        conn = sqlite3.connect(color_db)  # connect to that database (will create if it doesn't already exist)
        c = conn.cursor()  # move cursor into database (allows us to execute commands)
        outs = ""
        c.execute('''CREATE TABLE IF NOT EXISTS color_table (user text, red int, green int, blue int);''') # run a CREATE TABLE command
        y = c.execute('''SELECT * FROM color_table WHERE user == ?; ''',(user,)).fetchone()
        if not y:
            c.execute('''INSERT INTO color_table VALUES (?,?,?,?); ''',(user, red, green, blue)) #checking if user exists
        c.execute('''UPDATE color_table SET red = ?, green = ?, blue = ? WHERE user == ?;''',(red, green, blue, user))
        to_return = c.execute('''SELECT * FROM color_table WHERE user == ?; ''',(user,)).fetchone() #fetching what I will return
        to_print = f"{to_return[0]}  {to_return[1]}  {to_return[2]} {to_return[3]}" #what I will return to the user
        conn.commit() # commit commands
        conn.close()

        return to_print



    else:
        user = request["values"]["user"] #getting color vals

        conn = sqlite3.connect(color_db)  # connect to that database (will create if it doesn't already exist)
        c = conn.cursor()  # move cursor into database (allows us to execute commands)
        outs = ""
        to_return = c.execute('''SELECT * FROM color_table WHERE user == ?; ''',(user,)).fetchone() #fetching what I will return
        if not to_return:
            return "ERROR! Nothing was found. Enter a valid username."
        to_print = f"{to_return[0]}`^`{to_return[1]}`^`{to_return[2]}`^`{to_return[3]}" #what I will return to the user
        conn.commit() # commit commands
        conn.close()

        return to_print
