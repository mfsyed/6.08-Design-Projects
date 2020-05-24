import requests
import math
import pprint
import sqlite3
import random
import string




trivia_table_db = '__HOME__/trivia/trivia_table.db'

trivia_url = "https://opentdb.com/api.php?amount=1&type=boolean"

trivia_question = ""
trivia_answer = ""


def update_score(correct, wrong):
    conn = sqlite3.connect(trivia_table_db)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()

    total_wrong += wrong
    total_correct += correct

    return (total_correct, total_wrong)


def request_handler(request):

    if request["method"] == "GET": #getting question and correct answer
        r = requests.get(trivia_url)
        question = r.json()

        trivia_question = question["results"][0]["question"]
        trivia_answer = question["results"][0]["correct_answer"]

        return trivia_question + "`^`" +trivia_answer

    else:
        #i wanna take in values given by the user (score of win and loss for that round), and add it to the total wrong and right columns and then return the user name, total wins and total losses
        correct = int(request["form"]["correct"])
        wrong = int(request["form"]["wrong"])
        user = request['form']['user']
        conn = sqlite3.connect(trivia_table_db)  # connect to that database (will create if it doesn't already exist)
        c = conn.cursor()  # move cursor into database (allows us to execute commands)
        outs = ""
        c.execute('''CREATE TABLE IF NOT EXISTS trivia_table (user text, correct int, wrong int);''') # run a CREATE TABLE command


        y = c.execute('''SELECT * FROM trivia_table WHERE user == ?; ''',(user,)).fetchone()

        if not y:
            c.execute('''INSERT INTO trivia_table VALUES (?,?,?); ''',(user, 0, 0)) #checking if user exists




        c.execute('''UPDATE trivia_table SET correct = correct + ?, wrong = wrong + ? WHERE user == ?;''',(correct, wrong, user))


        to_return = c.execute('''SELECT * FROM trivia_table WHERE user == ?; ''',(user,)).fetchone() #fetching what I will return

        tot_score = int(to_return[1]) - int(to_return[2])

        to_print = f"{to_return[0]}  C:{to_return[1]}  W:{to_return[2]}   T:{tot_score}" #what I will return to the user

        conn.commit() # commit commands
        conn.close()

        return to_print
