import time
from copy import deepcopy
import signal
import heapq
import random

class PriorityQueue: # give a priority value to an element

    def __init__(self): # constructor to initialise pqueue elements
        self.location = 0 # index of elements
        self.prioqueue = [] # the actual queue

    def empty(self):
        return len(self.prioqueue) == 0 
        
    def pop(self): # Returns the value situated at the end of the queue, then pops it 
        return heapq.heappop(self.prioqueue)[-1]

    def push(self, item, priority):
        # Pushes a priority value, index and “item” which consists of a list containing 
        # the current moves and states of the board. It then increments the index by 1. 
        heapq.heappush(self.prioqueue, (-priority, self.location, item))
        self.location += 1

class TimeoutException(Exception):
    pass

def timeout_handler(signum, frame): 
    # used to ensure that function terminates if a solution isn't found in time
    raise TimeoutException

def heuristic(state):  # calculates the number of vehicles blocking the exit
    substring = state[12:18]
    # similar to is_solved function, it only looks at 
    # the line of the board where XX is
    for i in range(len(substring)):
        if substring[i] == 'X':
            end_x = i
            break

    cars_blocking = 0
    for j in range(end_x + 2, 6):
        if substring[j].isalpha() and substring[j] != '.':
            cars_blocking += 1
           
    return cars_blocking

def hill_climbing(start_state, found_states):
    current = start_state
    neighbors = expand(current, found_states) 
    randindex = random.randrange(0, len(neighbors))
    rand_neighbor = neighbors[randindex] # pick a random neighbour
    return rand_neighbor[-1]

def random_restart(start_state, found_states):
    if len(start_state) < 37:
        start_state = [''.join(start_state[0:])]
        start_state = list(start_state[0])
    current = start_state
    
    while is_solved(current) == False:
        current = hill_climbing(start_state, found_states)
        #printboard(current)
    return current

def getbest(states):
    bestboard = states[0]
    bestboard = bestboard[-1]
    bestcost = heuristic(bestboard) 

    for neighbor in states: # get the best neighbouring node
        currentcost = heuristic(neighbor[-1]) 
        if currentcost < bestcost:
            bestcost = currentcost 
            bestboard = neighbor
    return bestboard, bestcost 

def greedy_hillclimbing(start_state, found_states):
    if len(start_state) < 37:
        start_state = [''.join(start_state[0:])]
        start_state = list(start_state[0])
    current = start_state
    currentcost = heuristic(current)
    child_states = expand(current, found_states)
    bestchild, bestcost = getbest(child_states)
    boards = []
    boards.append(bestchild)
    steps = 0
    while True:
        #printboard(current)
        if is_solved(current):
            printboard(current)
            print("Number of Steps: ", steps)
            return True

        found_states = []
        child_states = expand(current, found_states)
        bestchild, bestcost = getbest(child_states)
        boards.append(bestchild)

        if boards[-1] == boards[-2]:
            print("Algorithm Stuck")
            #printboard(boards[-1])
            return False

        if bestcost <= currentcost:
            current = bestchild
            currentcost = bestcost

        steps += 1

    return False

# ALTERNATE ITERATIVE DEEPENING IMPLEMENTATION ATTEMPT
"""def iterative_deepening(start_state, found_states):
    if len(start_state) < 37: # make the first state into a list 
        start_state = [''.join(start_state[0:])]
        start_state = list(start_state[0])
    depth = 1
    current = start_state
    queue = [[[], [current]]]  # initialise a queue
    visited = []
    while True:
        temp = depth
        final = dls(start_state, found_states, temp, visited, queue)
        print("Returned: ", final)
        if final == True:  # then solution was found we can stop searching
            print("Depth:", depth)  # show current depth
            break
        print("INCREMENT DEPTH TO", depth + 1)
        depth += 1
    return True
def dls(start_state, found_states, depth, visited, queue):
    #print("Depth", depth)
    #printboard(start_state)
    if depth >= 0:
        moves, current = queue.pop()  # pop the next state, which is the LAST item (LIFO)

        if is_solved(current[-1]):  # if a goal state has been found
            print("# of moves " + str(len(moves)))
            printboard(current[-1])
            print(moves)
            exit()  
            return True

        #print(moves)
        found_states = [] # reset the states that have been found by expand for the next child node
        # create child nodes
        for new_moves, child_node in expand(current, found_states):
            queue.append([moves + new_moves, [child_node]])
            dls(child_node, found_states, depth - 1, visited, queue) 
    return False"""


# Actual Iterative Deepening Used For Times
def iterative_deepening(start_state, found_states):
    if len(start_state) < 37:
        start_state = [''.join(start_state[0:])]
        start_state = list(start_state[0])
    depth = 0
    current = start_state
    queue = [[[], [current]]]  # initialise a queue
    visited = set()
    while True:
        final, moves = dls(start_state, found_states, depth, visited, queue)
        if final == True:  # then solution was found we can stop searching
            return moves
        depth += 1

    
def dls(start_state, found_states, depth, visited, queue):
    while depth >= 0: #stop se
        moves, current = queue.pop()  # pop the next state, which is the LAST item (LIFO)

        if is_solved(current[-1]):  # if a goal state has been found
            print("SOLVED after traversing " + str(len(visited)) + " nodes")
            print("Depth: " + str(len(moves)))
            printboard(current[-1])
            return True, moves

        found_states = []
        # create child nodes
        for new_moves, child_node in expand(current, found_states):
            if hash(str(child_node)) not in visited:
                visited.add(hash(str(child_node)))
                queue.append([moves + new_moves, [child_node]])
                dls(child_node, found_states, depth - 1, visited, queue) 

    return False


def astar(start_state, found_states):
    pqueue = PriorityQueue()
    start_state = [''.join(start_state[0:])]
    start_state = list(start_state[0])
    current = start_state
    closed_set = set()
    pqueue.push([[], current], 0)
    while not pqueue.empty():
        moves, current = pqueue.pop()
        if is_solved(current):
            print("Number of Searched Nodes: " + str(len(closed_set)))
            print("Depth Reached: " + str(len(moves)))
            print("Final Board State: ")
            printboard(current)
            return moves

        found_states = []
        for new_moves, new_states in expand(current, found_states):
            g = len(new_moves)
            # f = g(u) + h(u)
            priority = g + heuristic(new_states)
            #print(priority)
            if hash(str(new_states)) not in closed_set:
                pqueue.push([moves + new_moves, new_states], priority)
                closed_set.add(hash(str(new_states)))
    return False


def bfs(start_state, found_states):
    if len(start_state) < 37:
        start_state = [''.join(start_state[0:])]
        start_state = list(start_state[0])

    current = start_state
    visited = set()
    queue = [[[], [current]]]  # initialise a queue
    while len(queue) > 0:
        moves, current = queue.pop(0)  # pop the next state which is the FIRST queue item (FIFO)
        if is_solved(current[-1]):  # if a goal state has been found
            print("Number of Searched Nodes: " + str(len(visited)))
            print("Depth Reached: " + str(len(moves)))
            print("Final Board State: ")
            printboard(current[-1])
            return moves  # return the path

        # create child nodes
        found_states = [] # reset found states
        for new_moves, child_node in expand(current, found_states):
            if hash(str(child_node)) not in visited:
                visited.add(hash(str(child_node)))
                queue.append([moves + new_moves, [child_node]])
    return None


def is_solved(state):
    substring = state[12:18] # isolate the substring where the target car is
    end_x = 0
    for i in range(len(substring)):
        if substring[i] == 'X':
            end_x = i + 1
            break
    solved = False
    end_x = end_x + 1
    for j in range(end_x, 6): # check if there are any cars still blocking the exit 
        if substring[j] == '.':
            solved = True
        elif substring[j] != '.':
            solved = False
            break
    return solved


def expand(state, found_states):
    if len(state) < 36: # if the start state is not a list of individual car values
        state = state[0]

    cars = get_cars(state) # get the different cars in the current board, and their indexes

    for car in cars:
        index = cars[car] # get the car indexes from the cars dictionary 
        count = state.count(car)

        horizontal = False
        if state[index] == state[index + 1]: # if the same car letter appears two indexes in a row, its horizontal
            horizontal = True

        move_by = 1
        if horizontal:
            orientation = -1 
        else:
            orientation = -6

        new_location = index + orientation * move_by

        while new_location >= 0 and new_location < len(state) and (state[new_location] == '.'):
            substring = int(new_location/6)
            substring = list(state[6*substring:6*substring+6]) 
            # ensures that horizontal substrings don't go into
            # a different line from where it originally was, 
            # as board is in a list so it is prone to this happening
            if ((horizontal and car in substring) or not horizontal):
                action = str(car + ('L' if horizontal else 'U') + str(move_by))
                found_states.append([[action], new_state(state, index, count, action)])
            move_by += 1
            new_location = index + orientation * move_by

        move_by = 1
        orientation = -orientation

        reversed_state = state[::-1] # reverse the state as we are now going the opposite way (Right and Down)
        j = len(state) - 1 - reversed_state.index(car)
        new_location = j + orientation * move_by
        while new_location >= 0 and new_location < 36 and (state[new_location] == '.'):
            substring = int(new_location/6)
            substring = list(state[6*substring:6*substring+6])
            if ((horizontal and car in substring) or not horizontal):
                action = str(car + ('R' if horizontal else 'D') + str(move_by))
                found_states.append([[action], new_state(state, index, count, action)])
            move_by += 1
            new_location = j + orientation * move_by
    return found_states


def new_state(state, index, count, action): 
    car = action[0]
    direction = action[1]
    move_by = action[2]
    horizontal = False
    new_state = deepcopy(state) 
    if new_state[index] == new_state[index + 1]: 
        horizontal = True

    if horizontal and direction == 'L' or direction == 'R':
        for i in range(index, index+count):  # remove the instances of that letter
            if new_state[i] == car:
                new_state[i] = '.'

    elif direction == 'U' or direction == 'D':
        index = new_state.index(car)
        temp = index
        new_state[index] = '.'
        for i in range(count):  # remove the instances of that letter
            index += 6
            if index < 36 and new_state[index] == car:
                new_state[index] = '.'
        index = temp # as index value was modified, return it back to its original value

    orientation = 1
    if direction == 'R' or direction == 'L': 
        if direction == 'L':
            orientation = -1 # when orientation is negative you are going to the left of the list 

        start_new_place = int(index) + orientation * int(move_by) # multiplying orientation by moveby determines direction
        end_new_place = int(start_new_place) + int(count) # the end of the car will be at the car start + its length 
        for i in range((start_new_place), end_new_place):
            new_state[i] = car # populate new car indexes with the car

    elif direction == 'D':  
        for i in range(0, count):
            new_place = int(index) + int(6)*int(move_by) + i*6 # i multiplied by 6 to move it down horizontally
            if (new_place < 36) and new_state[new_place] == '.': # ensure that board bounds aren't crossed
                new_state[new_place] = car

    elif direction == 'U':
        endindex = 0
        endindex = index + 6*(int(count)-1) 
        for i in range(0, count):
            new_place = int(endindex) - int(6)*int(move_by) - i*6 # when going up, subtraction must be down instead of addition
            if (direction == 'U' and new_place >= 0) and new_state[new_place] == '.':
                new_state[new_place] = car

    return new_state


def printboard(new_state): # used to output the board as a 2D matrix instead of as a list
    for i in range(len(new_state)):
        if i % 6 == 0:
            substring = new_state[i:i+6]
            matrix = []
            for j in substring:
                matrix.append(j)
            board = ' '.join(matrix)
            print(board)


def get_cars(board):  # a function for finding all different unique cars in each state
    cars = {}
    i = 0
    for letter in board:
        i += 1
        if letter.isalpha() and letter not in cars:
            # scan through board, if its a letter not seen before, add it to cars matrix
            cars[letter] = i - 1
    return cars

def process_inputs(proposed_solutions, initial_state, current_problem):
    rhdoc = open('rh.txt', 'r')  # open file for reading
    solutionline = False
    propsol = proposed_solutions
    initial_states = initial_state
    current = current_problem
    temp = []
    currentboard = []
    solstart = "Sol: "
    for value in rhdoc:
        if len(value) == 37:  # the input line in the document is 37 characters long
            current.append(value)
            currentboard = []
            for i in range(len(value)):  # generate the board
                if i % 6 == 0:
                    substring = value[i:i+6]
                    matrix = []
                    for j in substring:
                        matrix.append(j)  # this is one row of the board
                    board = ''.join(matrix)
                    currentboard.append(board)
            currentboard[-1].replace("\n", "")
            # add the created board to the board_states array
            initial_states.append(currentboard)
        if solstart in value:  # check if the current line is a solution line
            solutionline = True
        if value == '\n' and solutionline:
            solutionline = False
            propsol.append(temp)  # for single line solution, just append
            temp = []
        elif solutionline:
            if solstart in value:
                # if there is a multiline solution, remove newline character
                value = value.split(solstart, 1)[1]
                value.replace("\n", "")
            else:
                value.replace("\n", "")
            for s in value.split():
                temp.append(s)
    if solutionline:
        solutionline = False  # when end of solution is reached, append temp to proposed solutions
        propsol.append(temp)
        temp = []
    rhdoc.close()
    return propsol, initial_states

# Printing Solution
def output(problem, solution, time, algorithm):
    outputfile = open("output.txt", "a")  # adds a solution each time 
    if algorithm == "BFS":
        outputfile.write("[BFS Problem " + str(problem + 1) + "]  Time Taken: " + str("{:.3f}".format(time)) + " || " + str(solution) + "\n")
    elif algorithm == "ID": 
        outputfile.write("[ID  Problem " + str(problem + 1) + "]  Time Taken: " + str("{:.3f}".format(time)) + " || " + str(solution) + "\n")
    elif algorithm == "A*":
        outputfile.write("[A*  Problem " + str(problem + 1) + "]  Time Taken: " + str("{:.3f}".format(time)) + " || " + str(solution) + "\n")
    elif algorithm == "SHC":
        outputfile.write("[SHC Problem " + str(problem + 1) + "]: " + str(solution) + "\n")
    elif algorithm == "RHC":
        outputfile.write("[RHC Problem " + str(problem + 1) + "]: " + str(solution) + "\n")
    outputfile.close()


def main():
    propsol = []
    initial_states = []
    current_problem = [] 
    TIMEOUT = int(input("Enter Maximal Time: "))  # Set the maximal time (recommended = 20 seconds)
    process_inputs(propsol, initial_states, current_problem)
    outputfile = open("output.txt", "w") # open and reset output file for  new solutions
    outputfile.write("-=+=- SOLUTIONS -=+=- \n")
    outputfile.close()

    for problem in range(0, 40): # iterate through each problem, and run the algorithms on the problem
        print("\nBoard Number " + str(problem+1))
        print("Solving: " + str(current_problem[problem]), end='')
        current_proposed = propsol[problem]
        print("Proposed Solution: ", end=' ')
        for x in current_proposed:
            print(x, end=' ')
        print("\nThe Initial Board State Is:\n")
        current_state = initial_states[problem]
        print(" ", end=' ')
        for i in range(1, 7):
            print(i, end=' ')
        print()
        for i in range(len(current_state)):
            if (i+1 <= 6):
                print(i + 1, end=' ')
            for j in range(len(current_state[i])):
                print(current_state[i][j], end=' ')
            print()

        
        print("-=+=- START BFS [P" + str(problem+1) + "] -=+=- ")
        found_states = []
        signal.signal(signal.SIGALRM, timeout_handler) # create a timeout handler
        signal.alarm(TIMEOUT)   
        try: # the following code will run as long as the timeout value has not been reached yet
            starttime = time.time()
            solution = bfs(initial_states[problem], found_states)
            print("Solution Found: " + str(solution))
            endtime = time.time()
            soldiff = abs((len(current_proposed) - 2) - len(solution))
            # NOTE: current proposed - 2 is to account for the the proposed which includes a "." and also the X movement 
            # In this implementation the board gets to the state where it is ABLE to do the 
            # X movement out the board
            print("Length Difference: " + str(soldiff))
            timetaken = endtime - starttime
            output(problem, solution, timetaken, "BFS")
            print(f"Algorithm Run-Time Is {timetaken} Seconds\n")
        except TimeoutException: # if the timeout value has been reached, the function will terminate
            print('Maximal Time Reached - FAILED')
            output(problem, "FAILED", "N/A", "BFS")
        
        
        print("-=+=- START Iterative Deepening [P" + str(problem+1) + "] -=+=- ")
        found_states = []
        signal.signal(signal.SIGALRM, timeout_handler)
        signal.alarm(TIMEOUT)   
        try:
            starttime = time.time()
            solution = iterative_deepening(initial_states[problem], found_states)
            print("Solution Found: ", end = '')
            sollength = len(solution)
            if sollength  > 10:
                print(str(solution[0:10]) +  " ... and " + str((sollength - 10)) + " more actions")
         
            endtime = time.time()

            soldiff = abs((len(current_proposed) - 2) - sollength )
            print("Length Difference: " + str(soldiff))
            timetaken = endtime - starttime
            output(problem, solution, timetaken, "ID")
            print(f"Algorithm Run-Time Is {timetaken} Seconds\n")
        except TimeoutException:
            print('Maximal Time Reached - FAILED')
            output(problem, "FAILED", "N/A", "ID")


        print("-=+=- START A* [P" + str(problem+1) + "] -=+=- ")
        found_states = []
        signal.signal(signal.SIGALRM, timeout_handler)
        signal.alarm(TIMEOUT)   
        try:
            starttime = time.time()
            solution = astar(initial_states[problem], found_states)
            print("Solution Found: " + str(solution))
            endtime = time.time()
            soldiff = abs((len(current_proposed) - 2) - len(solution))
            print("Length Difference: " + str(soldiff))
            timetaken = endtime - starttime
            output(problem, solution, timetaken, "A*")
            print(f"Algorithm Run-Time Is {timetaken} Seconds\n")
        except TimeoutException:
            print('Maximal Time Reached - FAILED')
            output(problem, "FAILED", "N/A", "A*")

        print("-=+=- START Steepest Ascent Hill Climbing [P" + str(problem+1) + "] -=+=- ")
        found_states = []
        signal.signal(signal.SIGALRM, timeout_handler)
        signal.alarm(TIMEOUT)   
        try:
            solution = greedy_hillclimbing(initial_states[problem], found_states)
            if solution == False:
                print("Steepest Ascent Failed")
                output(problem, "FAILED", "N/A", "SHC")
        except TimeoutException:
            print('Maximal Time Reached - FAILED')
            output(problem, "FAILED", "N/A", "SHC")
        

        print("-=+=- START Random Restart Hill Climbing [P" + str(problem+1) + "] -=+=- ")
        found_states = []
        signal.signal(signal.SIGALRM, timeout_handler)
        signal.alarm(TIMEOUT)   
        try:
            starttime = time.time()
            solution = random_restart(initial_states[problem], found_states)
            print("Some Solution Found: ")
            output(problem, "Certain Board State Found", "N/A", "RHC")
            endtime = time.time()
            timetaken = endtime - starttime
            print(f"Algorithm Run-Time Is {timetaken} Seconds\n")
        except TimeoutException:
            print('Maximal Time Reached - FAILED')
            output(problem, "FAILED", "N/A", "RHC")
        
    print("-=+=- Program Ended -=+=- \n")

main()
