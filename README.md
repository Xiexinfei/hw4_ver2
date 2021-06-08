# hw4_ver2

#1 parking lot
當按下鍵盤的向下建，python（XBee.py）會把呼叫RPC的指令由PC端的XBee傳到板子上的XBee，使車子執行RPC function，
RPC function會讓車子先倒退(d2+0.5停車格長)使車子中心在停車格的中心軸上，再根據輸入的參數z(1:west, 0:east)決定要左轉或右轉，旋轉之後再倒退(d1+停車格深)的距離
（倒退的距離會有誤差所以在數字上有做一些調整以降低誤差）
