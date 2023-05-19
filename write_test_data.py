import numpy as np



N = 10
arr =  np.arange(N*2, dtype = np.int32)
arr.tofile('data/arr.bin')