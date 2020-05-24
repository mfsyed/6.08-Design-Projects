import requests
import math
import pprint




api_key = "da1fd15a2a78a77cd8d92d689ec192f2" #my key to get info
city = "Cambridge,MA,US"#my place
payload = {'q':city,'appid':api_key} #will be concatenated to get request
weather_url = "https://api.openweathermap.org/data/2.5/weather"
date_link = "http://worldclockapi.com/api/json/est/now"



def request_handler(request):
    r_weather = requests.get(weather_url,payload)
    weather = r_weather.json()

    r_date = requests.get(date_link)
    dtme = r_date.json()

    if request["values"]["input"] == "TEMP": #getting temp, converting celsius
        temp = int(weather["main"]["temp"] - 273.15)
        return "TEMP:" + str(temp) + " deg C"

    elif request["values"]["input"] == "TIME":
        time = str(dtme['currentDateTime']) #getting all of date
        return "TIME: " + time[11:16] #getting the hours/mins

    elif request["values"]["input"] == "DATE":
        date = str(dtme['currentDateTime']) #grabs entire date time
        month = str(int(date[5:7]))
        day = str(date[8:10])
        year = str(date[0:4])
        return "DATE: " + month + "/" + day  + "/" + year # giving date in mm/dd/yyyy

    elif request["values"]["input"] == "WEATHER":
        weather = str(weather["weather"][0]["description"])
        return "WEATHER: " + weather

    elif request["values"]["input"] == "HUMIDITY":
        humidity = str(weather["main"]["humidity"])
        return "HUMIDITY: " + humidity + "%."

    elif request["values"]["input"] == "PRESSURE":
        pressure = str(weather["main"]['pressure'])
        return "PRESSURE: " + pressure + " hPa"

    else:
        return "INVALID REQUEST" #if it's not one of the options then do nothing
