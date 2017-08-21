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
    acc.plot(y=acc.columns, linestyle='-', marker='o')
    plt.show()
