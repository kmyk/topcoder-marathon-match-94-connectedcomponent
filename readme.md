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

-   順位表
    -   この時点でRedCoderの提出はwleiteさんひとり
    -   最初からずっと1位のEvbCFfp1XBさんは2回目の提出で992258.06点
        -   1ケースでTLEしてるの除いて満点とかありそう
    -   乱択でないいい感じの方針が隠れてそう

visualizer改良して手で遊んでみたけど分からない

-   近傍として swap / rotate / reverse: 平均 170 点
    -   1a43f9be971f7e53a422d0a8ea90ef5c50ffc832
    -   変わらず3位 提出時 847741.94 -> 898064.52点 08.18.2017 16:00:21

plotして図形で見比べるようにする

-   結果
    -   SEED = 3732, S = 300: 162802
    -   SEED = 3725, S = 300: 328173
    -   SEED = 3856, S = 495: 2290
    -   SEED = 3584, S = 495: 4404759

-   だめなケース潰しても全体に悪くなるんじゃだめ
    -   提出したら下がった
    -   6d46810d42e0ef032891ee120d4d6d9ff22fc1bb

TLEを10秒から100秒にして試したら全体に改善することが判明。高速化をやっていけばよさそう

-   中央付近から1回dfsするだけにして高速化
    -   297fb16aee3b7d432bfd82a8aa8ea060a3fa1008
    -   9位 826785.71点 08.21.2017 00:31:10
    -   1a43f9be971f7e53a422d0a8ea90ef5c50ffc832 の提出後に私のひとつ上にいた後輩を抜いているので、これより改善されている
-   `#pragma GCC target "avx"` したら SIGILL した
    -   EC2 m3.large で動いているらしい: <https://apps.topcoder.com/wiki/display/tc/Processing+server+specifications+(AWS)>
    -   Intel AVX は有効らしいが何故だ 利用方式のためか: <https://aws.amazon.com/jp/ec2/instance-types/>
-   DFSをstackでやるなどして高速化
    -   adba97cac209c228b160429b55fea74699886efe
    -   ちょっと悪くなった  802631.58点 -> 796315.79点
-   TopCoder鯖上で速くなるよう調整
    -   ab5d7922f8c1459aacd64da3bb52b74e43981b09
    -   良くなって ふたつ前より増加 793448.28点 -> 810000.00点
-   中心を起点として層状にメモ化
    -   手元で改善なし
    -   速くはなるがswapについてのみなのでほぼ生きない
    -   rotateで速くするにはさらに実装が必要だがやりたくない
-   微調整
    -   95dca04999667ec7764c741e34c6e9e8f9e1f07f
    -   9位に復帰 822166.67点 08.21.2017 12:49:05

終了時刻: 08.24.2017 10:00 JST
