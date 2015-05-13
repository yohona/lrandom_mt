# lrandom_mt
 A pseudorandom number generator for lua. 
 
 It will return a table used for generate random number.
 
 http://en.wikipedia.org/wiki/Mersenne_Twister
 
 $ lrandom = require("lrandom");
 
 $ random:initmt(123)  # 123 is the seed
 
 $ a = random:random(50)  # generate a random number between 1, 50
 
 $ b = random:random(50, 100)  # generate a random number between 50, 100
 
 $ random:setseed(500)  # set the seed to 50
