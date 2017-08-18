# TopCoder Marathon Match: 94. ConnectedComponent

-   Problem Statement: <https://community.topcoder.com/longcontest/?module=ViewProblemStatement&compid=57152&rd=16958>
-   Standings: <https://community.topcoder.com/longcontest/?module=ViewStandings&rd=16958>
-   Submit: <https://community.topcoder.com/longcontest/?module=Submit&compid=57152&rd=16958&cd=15248>

## memo


-   何もしない: 平均 10000 点
-   shuffle乱択: 平均 50000 点
-   swap山登り: 平均 440000 点
-   swap焼き鈍し: 平均 480000 点
    -   bdd6c583db66dd0eb5fe57396bc93586ae6698b2
    -   5位 675000.00点ぐらい 08.17.2017 11:34:08
    -   でもバグってた

点数はケース差が大きいのでその平均にあまり意味がなかった。乱択での点数との比の平均を見ることにする。

-   swap焼き鈍し: 平均 64 点
-   swap焼き鈍し + 評価値に第3位までの連結部分: 平均 57 点
    -   提出してみたら実際に下がってた
    -   12位 589166.67点 08.17.2017 20:54:24
    -   でもバグってた
    -   でも直しても変わらなさそう
