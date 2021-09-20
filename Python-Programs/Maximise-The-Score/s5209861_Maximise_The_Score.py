import time
import sys

def popq(ball_scores, i):
    ball_scores[i] = ball_scores[int(len(ball_scores) - 1)] 
    # swap the ball scores at i with the last element of ball scores
    # now that ball_scores[i] is the new last element, it can be popped easily with .pop()
    ball_scores.pop() 
    heapify(ball_scores, len(ball_scores), i) # run heapify again to restore heap structure 
    return ball_scores

def heapify(ball_scores, num_balls, i):
    largest = i
    leftchild = (2*i)+1
    rightchild = (2*i)+2

    if leftchild < num_balls:  # when left exists
        if (ball_scores[leftchild][1] > ball_scores[largest][1]) or ((ball_scores[leftchild][1] ==
                                                                    ball_scores[largest][1]) and (ball_scores[leftchild][0] > ball_scores[largest][0])):
            largest = leftchild

    if rightchild < num_balls:  # when right exists
        if (ball_scores[rightchild][1] > ball_scores[largest][1]) or ((ball_scores[rightchild][1] ==
                                                                     ball_scores[largest][1]) and (ball_scores[rightchild][0] > ball_scores[largest][0])):
            largest = rightchild

    if largest != i:
        # swap the the values of index i and index "largest" if i isn't the largest 
        ball_scores[i], ball_scores[largest] = ball_scores[largest], ball_scores[i]
        heapify(ball_scores, num_balls, largest) # run heapify recursively again

    return ball_scores

def process_inputs(lines, i):
    testlines = []
    for line in lines[3*(i-1) + 1:3*i + 1]:
        line.strip()
        contents = line.split()
        testlines.append(contents)
    num_balls = int(testlines[0][0])  # n value
    max_turns = int(testlines[0][1])  # k value
    # n space separated integers (a1, a2, ... an). Here, ai indicates the score written on 𝑖th ball.
    ball_scores = testlines[1]
    toss_result = testlines[2][0]
    return num_balls, max_turns, ball_scores, toss_result

def run_algorithm(num_balls, max_turns, ball_scores, toss_result):
    # initialise scott and rusty's scores
    Scott_Score = 0
    Rusty_Score = 0
    # initialise the priority queues 
    ScottQ = []
    RustyQ = []

    sums = []
    for i in range(num_balls): # find the sum of digits for each value in ball_scores
        total = 0
        num = int(ball_scores[i])
        while (num > 0):
            digit = num % 10
            total = total+digit
            num = num//10
        sums.append(total) # add each digit sum to sums array 

    for i in range(num_balls):
        # Rusty's queue will have the ball scores first, then the ball sums
        RustyQ.append([int(ball_scores[i]), int(sums[i])])
        # Scott's queue will have the ball sums first, then the ball scores 
        ScottQ.append([int(sums[i]), int(ball_scores[i])]) 
       
    index = int((num_balls//2)-1)
    for i in range(index, -1, -1): # arrange both queues into a binary heap structure 
        heapify(ScottQ, len(ScottQ), i)
        heapify(RustyQ, len(RustyQ), i)

    while len(RustyQ) != 0 and len(ScottQ) != 0: # either both queues aren't empty
        if toss_result == "HEADS": # if the toss result is heads, its Scott's turn
            for i in range(max_turns): # for each turn
                if len(RustyQ) != 0 and len(ScottQ) != 0:
                    Scott_Score += ScottQ[0][1] # add the maximum value from Scott's queue to Scott's score
                    for i in range(len(RustyQ)): # for each value in Rusty's queue
                        if RustyQ[i][0] == ScottQ[0][1]:  # find Scott's maximum value in Rusty's queue
                            popq(RustyQ, i) # then pop it 
                            break
                    popq(ScottQ, 0) # then also pop the maximum value from Scott's queue
            toss_result = "TAILS" # switch toss result to Tails to signify a new turn 
        else:
            for i in range(max_turns): 
                if len(RustyQ) != 0 and len(ScottQ) != 0:
                    for i in range(len(ScottQ)):
                        if ScottQ[i][0] == RustyQ[0][1]: # find Rusty's maximum value in Scott's queue
                            Rusty_Score += ScottQ[i][1]  # add Rusty's maximum value to Rusty's score
                            popq(ScottQ, i) # pop the max value of Rusty's queue from Scott's queue
                            break
                    popq(RustyQ, 0) # pop the max value from Rusty's queue
            toss_result = "HEADS" # switch toss results back to heads to signify a new turn

    return Scott_Score, Rusty_Score

def output(solution):  # adds a solution to output.txt each time one is found
    outputfile = open("output.txt", "a")
    outputfile.write(str(solution) + "\n")
    outputfile.close()

def main():  
    # USAGE: Enter "python s5209861_Maximise_The_Score.py inputLeScore.txt" to run the code
    # If the the program is not run from terminal, then it'll automatically assume filename = inputLeScore.txt

    outputfile = open("output.txt", "w")  # open and reset output file for new solutions
    outputfile.close()

    arguments = sys.argv # receive command line arguments
    argument_list = arguments[1:]
    if len(argument_list) == 0: # if no cmd line arguments are given, automatically set file name
        print("No command line input entered!")
        print("Enter 'python s5209861_Maximise_The_Score.py inputLeScore.txt' into terminal to run the code manually")
        print("Setting filename as inputLeScore.txt...\n")
        filename = 'inputLeScore.txt'
    else:
        filename = argument_list[0] 

    inputfile = open(filename, 'r')  # open input file for reading
    lines = inputfile.readlines()
    inputfile.close()

    # First line contains an integer T denoting the number of test-cases.
    testcasenum = int(lines[0])
    for i in range(1, testcasenum + 1):  # for each test case
        starttime = time.time()
        # get the relevant parameters from the input document
        num_balls, max_turns, ball_scores, toss_result = process_inputs(lines, i)
        # pass parameters into the algorithm
        Scott_Score, Rusty_Score = run_algorithm(num_balls, max_turns, ball_scores, toss_result)
        endtime = time.time()
        # print results for the current test input case
        print("Case " + str(i) + " Results:")
        print("Scott: " + str(Scott_Score) + " | Rusty: " + str(Rusty_Score))
        solution = str(Scott_Score) + " " + str(Rusty_Score)
        timetaken = (endtime - starttime)
        print("Algorithm Run Time Is " + str(timetaken) + " Seconds")
        output(solution) # write the solution to output.txt file
        print()

main()
