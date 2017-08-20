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
-   初期配置を中央付近に寄せる + swap焼き鈍し: 平均 86 点
-   初期配置を中央付近に寄せる + swap焼き鈍し + score計算時に最大連結成分の周囲しか見ない + swapする行の選択で端を優先: 平均 121 点
    -   57110f469a2052fa5fc3d8cc77ad91ba2bdef666
    -   5位 提出時810416.67点 08.17.2017 23:41:17
-   初期配置を中央付近に寄せる + swap焼き鈍し + score計算時に最大連結成分の周囲しか見ない: 平均 93 点
    -   効果検証
    -   わりと効いていたっぽい
-   初期配置を中央付近に寄せる + swap焼き鈍し + score計算時に最大連結成分の周囲しか見ない + swapする行の選択で端を優先: 平均 146 点
    -   a3b89387485750dae3eaccf2c47eefca7d4d2540
    -   3位 提出時860000.00点 08.18.2017 09:53:33
    -   調整して提出

方針転換について

    -   この時点でRedCoderの提出はwleiteさんひとり
    -   最初からずっと1位のEvbCFfp1XBさんは2回目の提出で992258.06点
        -   1ケースでTLEしてるの除いて満点とかありそう
    -   乱択でないいい感じの方針が隠れてそう

visualizer改良して手で遊んでみたけど分からない

-   近傍として swap / rotate / reverse: 平均 170 点
    -   1a43f9be971f7e53a422d0a8ea90ef5c50ffc832
    -   変わらず3位 提出時 847741.94 -> 898064.52点 08.18.2017 16:00:21

plotして図形で見比べるようにする
