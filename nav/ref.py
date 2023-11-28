map = []
countryColor = []
colorCode = ['Red','Green','Blue','Yellow']

# This function initailize the 2-dimensional map array
def createMap():
    global map
    mapString = ("0,1,1,1,0,0,0\n"
                 "1,0,0,1,1,1,0\n"
                 "1,0,0,1,0,0,1\n"
                 "1,1,1,0,1,0,1\n"
                 "0,1,0,1,0,1,1\n"
                 "0,1,0,0,1,0,0\n"
                 "0,0,1,1,1,0,0"
                )
               
    for r in mapString.split("\n"):
        map.append([int(x) for x in r.split(',')])
    
# This function initialize the color of each country to 0 = no color
def initializeColor():
    global map
    N = len(map)
    for i in range(N):
        countryColor.append(0)

# print the solution in the format required
def printSolution():
    global colorCode
        
    output = "Country\n"
    for i in range(len(countryColor)):
        output += str(i) + "\t"
    output += "\n"
    for i in range(len(countryColor)):
        output += colorCode[countryColor[i]-1] + "\t"
    output += "\n"
    print(output)
 
# This function tries to recursively color a country
# Here the parameter country is the index to the list of country from 0...N-1
# The skeleton of the program has been provided to you
# Remove the passes and fill in the missing codes for this function
def color(country):
    global countryColor
    global map
    N = len(map) # total no of countries
  
    # if country = number of countries to color (base case)
    # print solution and exit
    if(country == N):
        printSolution()
        return

    availableColor = set([1,2,3,4])  
    # remove the color of any neighbour of country i from the set availableColor
    for i in range(N):
        if(i != country and map[country][i] == 1 and countryColor[i] in availableColor):
            availableColor.remove(countryColor[i])

    # for each remaining color in the set availableColor
    # color country i with the color and then recursively color the next country
    for available_color in availableColor:
        countryColor[country] = available_color
        color(country + 1)
        # reset the color of this country to none before backtracking
        countryColor[country] = 0
        
        # # OR
        # if(country + 1 != N and countryColor[country + 1] == 0):
        #     # reset the color of this country to none before backtracking
        #     countryColor[country] = 0

# read the map array 
createMap()
# set all countries to no color
initializeColor()
# first try to color country 0
color(0)