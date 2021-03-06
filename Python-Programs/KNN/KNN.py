import csv
import random
from math import sqrt
import operator
import matplotlib.pyplot as plt
import numpy as np

def split_data(data, percentage): # split the data given a split percentage 
    amount = round(len(data)*percentage)
    testing = (random.sample(data, k = amount))
    training = data.copy()
    for item in testing:
        if item in data: 
            training.remove(item)
    return training, testing

def euclidean_distance(training, test): # measures the distance between two flowers based on their features.
    # convert the training and test numbers into floats (except for final string for plant class)
    for i in range(len(training) - 1):
        training[i] = float(training[i])
    for i in range(len(test) - 1):
        test[i] = float(test[i])
    distance = 0
    for i in range(len(training) - 1):
        distance += ((training[i] - test[i])**2)
    e_dist = (sqrt(distance))
    #print(e_dist)
    return e_dist


def normalized_ED(training, test, data_mean, data_std):
    for i in range(len(training) - 1):
        training[i] = float(training[i])
    for i in range(len(test) - 1):
        test[i] = float(test[i])
    distance = 0
    for i in range(len(training) - 1): # use normalised euclidean distance eq
        distance = distance + ((((training[i] - data_mean[i])/data_std[i]) - ((test[i] - data_mean[i])/data_std[i]))**2)
    n_dist = sqrt(distance)
    return n_dist

def kNN(training, testpoint, k, alternate_distance, mean_training, std_training):
    distances = []
    for i in range(len(training)):  # find euclidean distance to all training data points
        if alternate_distance:
            distance = normalized_ED(training[i], testpoint, mean_training, std_training)
        else:
            distance = euclidean_distance(training[i], testpoint)   
        distances.append((training[i], distance))
    distances.sort(key=operator.itemgetter(1)) # sort the distances
    neighbours = [] 
    for i in range(k):
        neighbours.append(distances[i][0]) # find nearest k neighbours
    return neighbours

def classification(neighbours):
	classes = {} # dictionary to store the various plant types in the k nearest flowers
	for i in range(len(neighbours)):
		plant_type = neighbours[i][-1] # the last value will be the plant class
		if plant_type in classes: 
			classes[plant_type] += 1 # if the plant type has been seen before, increment key by 1
		else:
			classes[plant_type] = 1 # if not seen before, initialise key to 1
	sorted_votes = sorted(classes.items(), key=operator.itemgetter(1), reverse=True) # sort the dictionary items by descending order
	return sorted_votes[0][0] # return the plant class with the highest key (most common neighbour)

def find_accuracy(testing, predictions):
	correct = 0
	for i in range(len(testing)):
		if testing[i][-1] == predictions[i]: 
			correct += 1
	return (correct/float(len(testing))*100) 

def run_kNN(training, testing, k, alternate_distance, mean_training, std_training):
    predictions = []
    for i in range(len(testing)):
        neighbours = kNN(training, testing[i], k, alternate_distance, mean_training, std_training)
        result = classification(neighbours) # classification function
        predictions.append(result)
        print('Sample Class = ' + str(testing[i][-1]) + ' | Prediction Class = ' + str(result) + ' | Prediction Correct: ', end = '')
        if str(result) == str(testing[i][-1]):
            print("True")
        else:
            print("False")
    accuracy = find_accuracy(testing, predictions) # get the accuracy
    return accuracy


def find_stats(training, testing):
    training_copy = training.copy()
    testing_copy = testing.copy()

    training_avgs = []
    training_stds = []
    for i in range(len(training_copy)):
        data = training_copy[i]
        data = (data[:-1])
        data = list(map(float, data))
        #print(data)
        average = (sum(data))/(len(data))
        training_avgs.append(average)
        np.array(data)
        std = np.std(data)
        training_stds.append(std)

    testing_avgs = []
    testing_stds = []
    for i in range(len(testing_copy)):
        data = testing_copy[i]
        data = (data[:-1])
        data = list(map(float, data))
        average = (sum(data))/(len(data))
        testing_avgs.append(average)
        np.array(data)
        std = np.std(data)
        testing_stds.append(std)

    return training_avgs, testing_avgs, training_stds, testing_stds


def plot_graph(data1, data2, label, alternate_distance, mean_training, std_training):
    accuracies = []
    for k in range(1, len(data1) + 1): # k can't be greater than training set length (or length of bigger set)
        predictions = []
        for i in range(len(data2)):
            neighbours = kNN(data1, data2[i], k, alternate_distance, mean_training, std_training)
            result = classification(neighbours) # classification function
            predictions.append(result)
        accuracy = find_accuracy(data2, predictions) 
        accuracies.append(accuracy)
        #print('Set Accuracy: ' + "{:.5f}".format((accuracy)) + '% | K: ' + str(k))
        predictions = []
    kvals = list(range(len(accuracies))) # k values
    x = np.array(kvals)
    y = np.array(accuracies)
    labelname = str((label) + " Accuracy")
    plt.plot(x, y, label = labelname)
    
    
def main():
    data = []
    k = 7 # initialise k value here
    split_percentage = 0.3 # initialise split% here (the number will be the % size of the testing set)
    alternate_distance = False # specify whether the euclidean distance or the alternate function is to be used

    with open('iris.csv', 'r') as file: # read in data from file
        reader = csv.reader(file)
        line = 0
        for row in reader:
            if line == 0:
                line += 1
            else:
                data.append(row)
    # training's results are 'known', testing is 'unknown' technically
    # we use trainings to predict how accurate the testing ones will be
    training, testing = split_data(data, split_percentage) # split the data 
    
    mean_training, mean_testing, std_training, std_testing = find_stats(training, testing)
    #find the mean within one data set 
    #find std within one data set

    # Run the kNN with normal distance function 
    print("-=+=- Start Testing Set -=+=-")
    testing_accuracy = run_kNN(training, testing, k, alternate_distance, mean_training, std_training)
    print("-=+=- Start Training Set -=+=-")
    training_accuracy = run_kNN(training, training, k, alternate_distance, mean_training, std_training)
    print("Distance Function Run: ", end = ' ')
    if alternate_distance == True: 
        print("NORMALIZED Euclidean")
    else:
        print("Euclidean")
    print('Training Set Accuracy: ' + "{:.5f}".format((training_accuracy)) + '%')  
    print('Testing Set Accuracy: ' + "{:.5f}".format((testing_accuracy)) + '%')
    print("Training Length: " + str(len(training)) + " | Testing Length: " + str(len(testing)))

    # print the values when using normal distance function 
    print("\nGenerating Graph...")
    plot_graph(training, training, "Training", False, mean_training, std_training)
    plot_graph(training, testing, "Testing", False, mean_training, std_training)
    # Alternate Distance Function
    plot_graph(training, training, "AlterTraining", True, mean_training, std_training)
    plot_graph(training, testing, "AlterTesting", True, mean_training, std_training)
    print("Graph Generated")
    plt.title('k-NN: Accuracy With Varying Number of neighbours Of K')
    plt.legend()
    plt.xlabel('K Value')
    plt.ylabel('Accuracy (%)')
    plt.show() 

    # run the kNN with the alternate distance function 



main()
