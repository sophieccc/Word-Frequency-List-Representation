import pandas as pd
from scipy import stats
import matplotlib.pyplot as plt


# Gets average difference between pairs in columns.
def average_difference(column1, column2):
    sum = 0
    for val1, val2, in zip(column1, column2):
        sum += abs((val1 - val2))
    sum = (sum / len(column1))
    return sum


# Gets average difference between pairs in columns.
def average_percent_difference(column1, column2):
    sum = 0
    for val1, val2, in zip(column1, column2):
        sum += abs((val1 - val2)) / val1
    sum = (sum / len(column1))
    return sum


def main():
    df = pd.read_csv("Data/probaOrig.csv")
    columns = ['Log with 4 places', 'Log with 3 places', 'Log with 2 places']
    print(df[[
        'Original', 'Log with 4 places', 'Log with 3 places',
        'Log with 2 places'
    ]].describe())

    #df.plot()
    #plt.show()

    for column in columns:
        print(column)
        # Checking distribution of data.
        # print(stats.shapiro(df['Original']))
        # print(stats.shapiro(df[column]))
        # plt.hist(df[column], range=(0,0.00005))
        # plt.show()

        res = stats.linregress(df['Original'], df[column])
        print("R-squared: {}".format(res.rvalue))     
        tinv = lambda p, df: abs(stats.t.ppf(p/2, df))
        ts = tinv(0.05, len(df['Original'])-2)
        print("slope (95%): {} +/- {}".format(res.slope,ts*res.stderr))

        # plt.plot(df['Original'], df[column], 'o', label='modified data')
        # plt.plot(df['Original'], res.intercept + res.slope*df['Original'], 'r', label='original data')
        # plt.legend()
        # plt.show()

        # Getting Wilcoxon results.
        result = stats.wilcoxon(df['Original'], df[column])
        print(result)
        alpha = 0.05
        if result[1] > alpha:
            print('Same distribution (fail to reject H0)')
        else:
            print('Different distribution (reject H0)')

        # The p-value is the probability that a given result would occur under the null hypothesis.
        # If the p value is 0.05 or lower, we reject the null hypothesis.
        # This means they do not have the same distribution.
        # AKA if p is smaller, we reject H0 (aka there is diff).
        # If p is bigger, we don't reject H0 (aka there is no diff).
        print('Average Diff: {0:.14f}'.format(
            average_difference(df['Original'], df[column])))
        print('Average % Diff: {0:.14f}\n'.format(
            average_percent_difference(df['Original'], df[column])))


if __name__ == "__main__":
    main()