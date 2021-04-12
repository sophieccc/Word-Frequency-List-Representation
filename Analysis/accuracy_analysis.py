import pandas as pd
from scipy import stats
import matplotlib.pyplot as plt
import numpy as np


# Gets average difference between pairs in columns.
def average_difference(column1, column2):
    sum = 0
    for val1, val2, in zip(column1, column2):
        sum += (val1 - val2)
    sum = (sum / len(column1))
    return sum

def main():
    df = pd.read_csv("Data/curr.csv")
    columns = ['Log with 4 places', 'Log with 3 places', 'Log with 2 places']
    print(df[['Original','Log with 4 places', 'Log with 3 places', 'Log with 2 places']].describe())

    for column in columns:
        # Checking distribution of data.
        # print(stats.shapiro(df['Original']))
        # print(stats.shapiro(df[column]))
        # plt.hist(df[column], range=(0,0.00005))

        # Getting Wilcoxon results.
        result = stats.wilcoxon(df['Original'], df[column])
        print(result)
        alpha = 0.05
        if result[1] > alpha:
            print('Same distribution (fail to reject H0)')
        else:
            print('Different distribution (reject H0)')

        # The p-value is the probability that a given result would occur under the null hypothesis.
        # If the p valie is 0.05 or lower, we reject the null hypothesis.
        # This means they do not have the same distribution. 
        # AKA if p is smaller, we reject H0 (aka there is diff). 
        # If p is bigger, we don't reject H0 (aka there is no diff).
        print('Average: {0:.14f}\n'.format(average_difference(df['Original'], df[column])))
        plt.show()

if __name__ == "__main__":
    main()