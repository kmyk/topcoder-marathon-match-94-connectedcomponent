#!/usr/bin/env python2
import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
import os

plt.style.use('ggplot')

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('tsv', nargs='+')
    args = parser.parse_args()

    acc = pd.DataFrame()
    for path in args.tsv:
        df = pd.read_csv(path, delimiter='\t', index_col=0)
        df = df.rename(columns={ 'score': os.path.basename(path) })
        acc = pd.concat([ acc, df ], axis=1)
    cnt = dict(map(lambda column: (column, 0), acc.columns))
    for s in acc.index:
        for column in acc.columns:
            if acc.loc[s, column] == max(acc.loc[s]):
                cnt[column] += 1.0 / list(acc.loc[s]).count(acc.loc[s, column])
    for column in acc.columns:
        cnt[column] /= len(acc.index)
    cnt = list(cnt.items())
    cnt.sort(key=lambda x: - x[1])
    for column, value in cnt:
        print column, value
    acc.plot(y=acc.columns, linestyle='-', marker='o')
    plt.show()
