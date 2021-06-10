# hw4_ver2

#1 parking lot
當按下鍵盤的向下建，python（XBee.py）會把呼叫RPC的指令由PC端的XBee傳到板子上的XBee，使車子執行RPC function，
RPC function會讓車子先倒退(d2+0.5停車格長)使車子中心在停車格的中心軸上，再根據輸入的參數z(1:west, 0:east)決定要左轉或右轉，旋轉之後再倒退(d1+停車格深)的距離
（倒退的距離會有誤差所以在數字上有做一些調整以降低誤差）

#2 line detection
先測量車子和直線偏離的角度以及直線的長度，旋轉車子至與直線平行，再前進直線的長度
由openMV傳給板子x1y1x2y2的座標，以arctan算出角度，以x1, x2判斷車子在直線的左邊還是右邊，以決定要向右轉還是向左轉，
完成轉彎後，再前進直線距離

#3 Apriltag
由openmv傳送處理過(由Tx換算成實際公分)的L(車子與AprilTag中心的水平距離)以及Ry（車子與Apriltag法向量的偏移角度）
先將車子轉九十度，前進L再轉九十度，便會面向Apriltag（與平面法向量平行）
移至Apriltag面前後，用Ping取車子和Apriltag的距離

（詳細的打在code的comment）
