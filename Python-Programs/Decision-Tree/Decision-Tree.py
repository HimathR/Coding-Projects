import math
import numpy as np # used for the np.unique function to quickly find number of yesno votes in an attribute
import pandas as pd # used for data manipulation 

def build_tree(data, current_attribute, f)
    root_node = {} # intiliase root node 
    attribute = data.columns.drop(current_attribute) # isolate the columns of current attribute
    highest_gain = get_highest_gain(data, current_attribute) # get the highest gain on the current attribute

    data_split = split_data(data, highest_gain) # split the data where the highest gain occurs 
    root_node = {highest_gain {}}

    for vote in data_split.keys()  # splits into a branch here for each outcome (01)
        tree_branch = data.where(data[highest_gain] == vote).dropna() # remove the data points equal to current key in data split
        purity = get_entropy(tree_branch[current_attribute]) # get the purity of the data 
        if purity == 0 # if all outcomes are the same (pure)
            root_node[highest_gain][vote] = data_split[vote][current_attribute].value_counts(
            ).idxmax() # update with new branch
        else # while theres more potential for branching
            if len(attribute) - 1 == 0 # if theres no more attributes to split on
                root_node[highest_gain][vote] = data_split[vote][current_attribute].value_counts(
                ).idxmax()
                return root_node # return the root node
            else # otherwise, recursively call the function again to expand the tree with new branches
                root_node[highest_gain][vote] = build_tree(
                    data_split[vote].drop(highest_gain, axis=1), current_attribute, f)
    return root_node


def get_highest_gain(data, attribute)
    attribute_column = data.columns.drop(attribute) # get the column in question
    information_gains = {}
    total_entropy = get_entropy(data[attribute]) # get the total entropy for the attribute

    for item in attribute_column # find the split entropy
        split_entropy = 0 
        classes, counts = np.unique(data[item], return_counts=True) # find the number of yesno (01) votes
        for i in range(len(classes)) 
            x = counts[i]np.sum(counts) 
            split = data.where(data[item] == classes[i]).dropna()[attribute] # remove the items in the class equal to current vote (0 or 1)
            split_entropy += x  get_entropy(split) # substitute the split data into equation 9.8 (see report)
        information_gains[item] = (total_entropy - split_entropy) # update dictionary with information gain
    highest_gain = max(information_gains, key=information_gains.get) # find the highest information gain from the dictionary 
    return highest_gain

def get_entropy(attribute_column)
    entropy = 0 
    classes, counts = np.unique(attribute_column, return_counts=True) # find the number of yesno (01) votes
    for i in range(len(classes))
        x = counts[i]np.sum(counts) # substitute into equation 9.3
        entropy += -x  math.log2(x)
    return entropy

def split_data(data, attribute) 
    new_branch = {} # split the data frame on a given attribute and return the split data
    for key in data.groupby(attribute).groups.keys() # group by the attribute
        new_branch[key] = data.groupby(attribute).get_group(key) # place into the dictionary by key (key being 0 or 1)
    return new_branch

def predict_decision(tree, data)
    split = list(tree.keys())[0] # go deeper into the tree based on current split
    try
        branch = tree[split][data[split]]
    except KeyError # return None if the attribute couldnt be found
        return None
    if not isinstance(branch, dict) 
        return branch # decision found
    return predict_decision(branch, data) # call recursively to go further into the dictionary

def find_stats(tree, data)
    TP, TN, FP, FN = 0, 0, 0, 0 # True Positive, True Negative, False Positive, False Negative
    for index, item in data.iterrows() # for each row in the data
        prediction = (predict_decision(tree, item)) # get the prediction
        actual = item[len(item)-1] 
        if actual == prediction # when the actual value and the predicted value are the same
            if actual == 'republican' 
                TP += 1
            else
                TN += 1
        else # when they aren't the same
            if prediction == 'republican'
                FP += 1
            else
                FN += 1
    # equations elaborated on in report software design section 
    num_correct = TP + TN
    total = TP + TN + FP + FN
    accuracy = (num_correcttotal)100
    precision = TP  (TP + FP)
    recall = TP  (TP + FN)
    f1 = 2(recall  precision)  (recall + precision)
    return round(accuracy, 2), round(precision, 2), round(recall, 2), round(f1, 2)

def split_traintestdata(data, percentage)  # split data into training and testing
    amount = round(len(data)percentage)
    training = (data.sample(n=amount)) # use .sample, which will RANDOMLY assign the training set n amount of data points
    testing = data.drop(training.index) # remove the data points used in training to initialise testing data frame
    return training, testing

def main()
    # initialise split% here (the number will be the % size of the testing set)
    split_percentage = 0.7
    data = pd.read_csv('votes.csv', dtype=str) # load all data into pandas data frame

    # rearrange data so that the party (which is what we're predicting for) is last
    # this will make the tree building later on easier
    attributes = []
    for i, column in enumerate(data.columns)
        if i == 0
            target = column 
        else
            attributes.append(column)
    attributes.append(target)

    data = data[attributes] # rearranged data frame
    training, testing = split_traintestdata(
        data, split_percentage)  # split the data
    features = training.columns[-1] # the features are everything except last column
    tree = build_tree(training, 'party', features) # build the tree with training set based on these features
    print(-=+=- Decision Tree -=+=-)
    accuracy, precision, recall, f1 = find_stats(tree, training) 
    print(fTRAINING SET Accuracy = {accuracy}%  Precision = {precision}  Recall = {recall}  F1 = {f1})
    accuracy, precision, recall, f1 = find_stats(tree, testing)
    print(fTESTING  SET Accuracy = {accuracy}%  Precision = {precision}  Recall = {recall}  F1 = {f1})

main()
