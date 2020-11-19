# Spam Filter with Naive Bayes Classifier and NLP (C++)

## Train Your Model
In order to train your model a training dataset must be provided, preferably in csv format that has two columns. The first column must contain the label spam or ham. Spam is spam and ham is not spam. The second column is the message. A sample dataset has been provided in the repository. training.cc will output two files the represents that is used for calculating the probability of spam or ham. A list of all significant words in the database along with their cumulative frequency of classifications are in these files. The following is an example of running the executable for training.cc. 
'''
./training -i spam.csv -os spam.prob -oh ham.prob
'''

## Classify Your Test
In order to classify your message(s), a database must be provided, and again, preferably in csv format. Only one column must be provided. The classifier will build the model from the existing training files and output a new file listing the classifications in order. The following is an example of running the executable for classify.cc
'''
./classify -i test.csv -is spam.prob -ih ham.prob -o classifications.txt
'''