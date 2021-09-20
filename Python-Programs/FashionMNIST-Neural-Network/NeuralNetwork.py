import numpy as np
import random
import math
import sys
import time

class NN:
    def __init__(self, num_input, num_hidden, num_output, epoch_num, learning_rate, minibatch_size, altercost):
        # first, initialise the inputs/hyperparameters in the constructor
        self.NInput = num_input
        self.NHidden = num_hidden
        self.NOutput = num_output
        self.epoch_num = epoch_num
        self.learning_rate = learning_rate
        self.minibatch_size = minibatch_size
        self.alternate_cost = altercost

        # next, create and assign random biases and weights based on normal distribution using numpy functions
        self.biases = [np.random.randn(
            num_hidden, 1), np.random.randn(num_output, 1)]
        self.weights = [np.random.randn(
            num_hidden, num_input), np.random.randn(num_output, num_hidden)]

    def train_network(self, training, testing):
        for epoch in range(1, self.epoch_num + 1):  # for each epoch
            starttime = time.time()
            print("Starting Epoch " + str(epoch))
            random.shuffle(list(training))  # shuffle the training list
            batches = []
            # initialise a batches array and divide up the training list based on
            # the specified mini batch size
            for i in range(0, len(training), self.minibatch_size):
                batches.append(training[i: i + self.minibatch_size])
            for mini_batch in batches:  # process each of the mini batches, tuning weights/biases
                self.evaluate_batch(mini_batch)
            num_correct = 0
            for datapoint in testing:  # for each of the data points in the testing array
                # find the neuron that has the highest activation with the weights and
                # biases found through evaluate_batch/backprop
                highest_activation = self.output_neuron(datapoint[0])
                # if brightest neuron value = label value
                if highest_activation == int(datapoint[1]):
                    num_correct += 1  # correct prediction found
            endtime = time.time()
            timetaken = endtime-starttime
            print("Epoch Accuracy: " + str(num_correct) + "/" +
                  str(len(testing)) + " | Time (s): " + str(round(timetaken, 2)))
        print("-=+=- NEURAL NETWORK COMPLETED -=+=-\n ")

    def evaluate_batch(self, mini_batch):
        # initialise array of zeros to store bias and weight errors
        bias_errors = [(np.zeros(self.biases[0].shape)),
                       (np.zeros(self.biases[1].shape))]
        weight_errors = [(np.zeros(self.weights[0].shape)),
                         (np.zeros(self.weights[1].shape))]

        for features, label in mini_batch:  # for each mini batch and its label, backpropagate
            data = np.array(features)
            data = data/255  # convert data between 0 and 255 to be between 0 and 1 by dividing each num in array by 255
            new_biaserrors, new_weighterrors = self.back_propagation(
                data, label)
            # once the errors for all biases/weights are found with back propagation
            # update the originally bias_errors array for the current batch
            # first iteration = hidden layer, second iteration = output layer
            for index, errors in enumerate(new_biaserrors):
                bias_errors[index] = bias_errors[index] + errors
            for index, errors in enumerate(new_weighterrors):
                weight_errors[index] = weight_errors[index] + errors

        learning_factor = (float(self.learning_rate) / self.minibatch_size)

        # biases in hidden/output layers
        self.biases[0] = self.biases[0] - (learning_factor * bias_errors[0])
        self.biases[1] = self.biases[1] - (learning_factor * bias_errors[1])

        for index, weight in enumerate(self.weights):
            # updates for all weights in the class
            updated_value = weight - (learning_factor * weight_errors[index])
            self.weights[index] = updated_value

    def back_propagation(self, features, label):
        features = np.array(features).reshape((len(features), 1))

        # feedforward for hidden layer
        hidden_perceptron = np.dot(self.weights[0], features) + self.biases[0]
        hiddenlayer_neurons = self.sigmoid(hidden_perceptron)

        # then feedforward to the output layer
        output_perceptron = np.dot(
            self.weights[1], hiddenlayer_neurons) + self.biases[1]
        # squash total net input with activation function
        outputlayer_neurons = self.sigmoid(output_perceptron)

        # Next, calculate loss for each output neuron based on a given error function
        if self.alternate_cost:
            derived_costfunction = self.cross_entropy(
                outputlayer_neurons, label)
        else:
            derived_costfunction = self.quadratic_cost(
                outputlayer_neurons, label)

        # calculate the slope given output neuron value
        outputlayer_derivative = self.deriv_sigmoid(outputlayer_neurons)
        output_error_bias = derived_costfunction * outputlayer_derivative

        # initialise output error weights np array
        output_error_weights = np.zeros(self.weights[1].shape)
        # calculate the weights from hidden layer to output layer
        for i in range(0, self.NOutput):
            output_error_weight = hiddenlayer_neurons * output_error_bias[i]
            output_error_weights[i] = output_error_weight.reshape(
                (len(output_error_weight),))

        # calculate the slope given hidden neuron value:
        hidden_error_bias = []
        hiddenlayer_derivative = self.deriv_sigmoid(hiddenlayer_neurons)
        for neuron in range(0, self.NHidden): # for each hidden neuron 
            weights = []
            for i in range(0, self.NOutput): # for each output neuron
                weights.append(self.weights[1][i][neuron])
            error = np.dot(weights, output_error_bias) * \
                hiddenlayer_derivative[neuron]
            hidden_error_bias.append(error)
        hidden_error_bias = np.array(
            hidden_error_bias).reshape((self.NHidden, 1))

        hidden_error_weights = np.zeros(self.weights[0].shape)
        for neuron in range(0, self.NHidden):  # calculate weights from input-hidden
            hidden_error_weight = features * hidden_error_bias[neuron]
            hidden_error_weights[neuron] = hidden_error_weight.reshape(len(hidden_error_weight))

        # return the hidden and output errors for the biases/weights
        return [hidden_error_bias, output_error_bias], [hidden_error_weights, output_error_weights]

    def output_neuron(self, features):
        # run the feed forward process with the weights+biases that were tuned with the mini batches
        data = np.array(features).reshape((len(features), 1)) / 255

        # run sigmoid function on the hidden layer/output layer neurons
        hidden_perceptron = np.dot(self.weights[0], data) + self.biases[0]
        hiddenlayer_neurons = self.sigmoid(hidden_perceptron)

        output_perceptron = np.dot(
            self.weights[1], hiddenlayer_neurons) + self.biases[1]
        outputlayer_neurons = self.sigmoid(output_perceptron)

        highest_activation = np.argmax(outputlayer_neurons)
        # return the highest output layer neuron
        return highest_activation

    def sigmoid(self, x):
        answer = 1.0 / (1.0 + np.exp(-x))
        return answer

    def deriv_sigmoid(self, x):
        answer = x * (1 - x)
        return answer
    
    # COST FUNCTIONS: See report for derivations on both
    def quadratic_cost(self, outputlayer_neurons, label): # returns the derived quadratic cost
        test_label_vector = np.zeros((10, 1))
        test_label_vector[int(label)] = 1 # set the "i"th value corresponding to label in np array of 0s to be 1
        test_instance_vector = np.copy(outputlayer_neurons)
        output = test_instance_vector - test_label_vector
        return output

    def cross_entropy(self, outputlayer_neurons, label): # returns the derived cross entropy cost
        test_instance_vector = np.copy(outputlayer_neurons)
        test_label_vector = np.zeros((10, 1))
        test_label_vector[int(label)] = 1.0
        output = (test_label_vector / test_instance_vector) + ((test_label_vector - 1.0) / (1.0 - test_instance_vector))
        return -1*output


def main():
    # Input Format:
    # python ISTask3.py 784 30 10 fashion-mnist_train.csv.gz fashion-mnist_test.csv.gz

    # Initialise Parameters Here:
    epoch_num = 30      # number of epochs
    learning_rate = 3   # learning rate
    minibatch = 20      # mini batch size
    altercost = False    # cost function
    # alternate cost uses cross entropy cost if true, quadratic cost if false
    
    # Start Algorithm
    print("-=+=- STARTING READING -=+=-")
    print("Training File: {trainfile}\nTesting File: {testfile}\n".format(
        trainfile=sys.argv[4], testfile=sys.argv[5]))

    # NInput  = number of neurons in the input layer
    NInput = int(sys.argv[1])   # 784
    # NHidden = number of neurons in the hidden layer
    NHidden = int(sys.argv[2])  # 30
    # NOutput = number of neurons in the output layer
    NOutput = int(sys.argv[3])  # 10

    trainfile = sys.argv[4] # "fashion-mnist_train.csv.gz"  # (the training set)
    testfile = sys.argv[5]  # "fashion-mnist_test.csv.gz"   # (the test set)

    # read in training and testing files
    trainingset = np.loadtxt(trainfile, skiprows=1,
                             dtype=float, delimiter=',')
    trainingdata = trainingset[:, 1:] # separate data values from labels 
    traininglabels = trainingset[:, 0]

    testingset = np.loadtxt(testfile, skiprows=1, dtype=float, delimiter=',')
    testingdata = testingset[:, 1:]
    testinglabels = testingset[:, 0]

    # turn training data and labels into a list of tuples
    training = list(zip(trainingdata, traininglabels))
    testing = list(zip(testingdata, testinglabels))

    # create the network model
    print("HYPERPARAMETERS | Learning Rate: " +
          str(learning_rate) + " | Batch Size: " + str(minibatch), end = ' | ')
    if altercost:
        print("Cost Function: Cross Entropy")
    else:
        print("Cost Function: Quadratic")
    print("-=+=- READING COMPLETED: GENERATING NEURAL NETWORK -=+=- ")
    model = NN(NInput, NHidden, NOutput, epoch_num,
               learning_rate, minibatch, altercost)
    
    # test the network
    model.train_network(training, testing)


main()
