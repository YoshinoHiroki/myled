# ロボットシステム学　課題１

## 概要
デバイスドライバを用いて Raspberry Pi4 ModelB で２つのLEDを操作する

## プログラムの説明
0 : led1の消灯

1 : led1の点灯

2 : led2の消灯

3 : led2の点灯

4 : led1とled2が交互に点滅

5 : led1が3回点滅してからled2が点灯

## 使用方法
1. ledの+側をGPIO24,25に繋げる
2. make を実行する
3. sudo insmod myled.c を実行する
4. sudo chmod 666 /dev/myled0 を実行する
5. echo x >> /dev/myled0　を実行でledを操作できる (xは0~5の任意の数)
6. sudo rmmod myled を実行して終了
