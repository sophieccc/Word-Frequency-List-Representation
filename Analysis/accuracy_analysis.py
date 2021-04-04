import pandas as pd
from scipy import stats
import matplotlib.pyplot as plt


def average_difference(column1, column2):
    sum = 0
    for val1, val2, in zip(column1, column2):
        sum += (val1 - val2)
    sum = (sum / len(column1))
    return sum

def main():
    df = pd.read_csv("curr1.csv")
    columns = ['Log with 4 places', 'Log with 3 places', 'Log with 2 places']
    print(df[['Original','Log with 4 places', 'Log with 3 places', 'Log with 2 places']].describe())

    print(stats.shapiro(df['Original']))
    for column in columns:
        print(stats.shapiro(df[column]))
    print("\n")

    for column in columns:
            print(stats.wilcoxon(df['Original'], df[column]))
    print("\n")

    # The p-value is the probability that a given result would occur under the null hypothesis.
    # If the p valie is 0.05 or lower, we reject the null hypothesis.
    # This means they do not have the same distribution. If p is bigger, we don't reject H0.

    for column in columns:
        print('Average: {0:.14f}'.format(average_difference(df['Original'], df[column])))
    print("\n")

    for column in columns:
        stat, p = stats.mannwhitneyu(df['Original'], df[column])
        print('Statistics=%.5f, p=%.5f' % (stat, p))
        alpha = 0.05
        if p > alpha:
            print('Same distribution (fail to reject H0)')
        else:
            print('Different distribution (reject H0)')

if __name__ == "__main__":
    main()