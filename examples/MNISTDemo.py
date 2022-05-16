from random import getstate
from pyTsetlinMachineParallel.tm import MultiClassTsetlinMachine
import numpy as np
from time import time
import sys

# from keras.datasets import mnist

# (X_train, Y_train), (X_test, Y_test) = mnist.load_data()


path = '/root/.keras/datasets/mnist.npz'
np.set_printoptions(threshold=10000)

with np.load(path, allow_pickle=True) as f:
    X_train, Y_train = f['x_train'], f['y_train']
    X_test, Y_test = f['x_test'], f['y_test']


X_train = np.where(X_train.reshape((X_train.shape[0], 28*28)) > 75, 1, 0) 
X_test = np.where(X_test.reshape((X_test.shape[0], 28*28)) > 75, 1, 0) 
print("Program Starting ...\n")
original_stdout = sys.stdout # Save a reference to the original standard output
with open('dataResult_orignal_attempt.txt', 'a') as f:
	sys.stdout = f # Change the standard output to the file we created.
	print("r = 1,000,000 - 16/05/2020 - init once",flush=True)
	# print("TAstates \n", file=LogTAState)
	
	for clauses in [2000,2000,2000,2000]:
		for s in [10]:
			tm = MultiClassTsetlinMachine(clauses, 50, s)
			print("Clauses =", clauses, ",s =",s,flush=True)

			for i in range(250):
				start_training = time()
				tm.fit(X_train, Y_train, epochs=1, incremental=True)
				stop_training = time()

				start_testing = time()
				result = 100*(tm.predict(X_test) == Y_test).mean()
				stop_testing = time()
				
				# taStatesList = tm.get_state()
				print("#%d %.2f%%  %.2f %.2f" % (i+1, result, stop_training-start_training, stop_testing-start_testing), flush=True)
				# print("TAstates are: ", taStatesList, file=LogTAState, flush=True)
				
				# print("*** Epoch ", i, "***", flush=True, file=LogTAState)
				
				# for l in range(10):
				# 	print("Class", l, flush=True, file=LogTAState)
				# 	for k in range(1000):
				# 		print("\t" ,taStatesList[l][k], file=LogTAState)
						




sys.stdout = original_stdout # Reset the standard output to its original value

print("Program Finished\n")
