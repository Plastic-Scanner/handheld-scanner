# MachineLearning model

# What we need to do when measurering, the device should scan untill the values have stabalized for up to X amount of seconds.
# The shorter the better obviously.


import binascii
import os
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import tensorflow as tf
import plotly.express as px
import seaborn as sns
from os import system
from everywhereml.code_generators.tensorflow import tf_porter


# the list of PLASTICTYPES that data is available for
PLASTICTYPES = [
    #"Reference",
    "PP",
    "PS",
    "PET",
    "HDPE",
    "PVC",
    "LDPE",
    # "verify",
]
# Define arrays
corrected = []
outputs = []
name_list = []

NUM_PLASTICTYPES = len(PLASTICTYPES)
# Creates a unit matrix for the number of plastic types.
ONE_HOT_ENCODED_PLASTICS = np.eye(NUM_PLASTICTYPES)

# read each csv file and combine the data in one array
for PLASTICTYPE_index in range(NUM_PLASTICTYPES):
    PLASTICTYPE = PLASTICTYPES[PLASTICTYPE_index]
    print(PLASTICTYPE)

    output = ONE_HOT_ENCODED_PLASTICS[PLASTICTYPE_index]
    print(output)
    # print(f"Processing index {PLASTICTYPE_index} for plastic type '{PLASTICTYPE}'.")

    df = pd.read_csv(
        # "/Users/gustavgregersen/Desktop/plasticScans/" + PLASTICTYPE + ".csv") 
        # "/Users/gustavgregersen/Desktop/plasticScans3/" + PLASTICTYPE + ".csv") # Only white readings
        "/Users/gustavgregersen/Desktop/plasticScans2/" + PLASTICTYPE + ".csv") # All samples

    # calculate the number of PLASTICTYPE recordings in the file
    num_recordings = int(df.shape[0])

    print(
        f"\tThere are {num_recordings} recordings of the {PLASTICTYPE} plastic type.")
    for i in range(num_recordings):
        wavelength_corrected = []
        wavelength_corrected += [
            (df['NoLights'][i]),
            (df['LED2'][i]),
            (df['LED3'][i]),
            (df['LED4'][i]),
            (df['LED5'][i]),
            (df['LED6'][i]),
            (df['LED7'][i]),
            (df['LED8'][i]),
            (df['415nm'][i]),
            (df['445nm'][i]),
            (df['480nm'][i]),
            (df['515nm'][i]),
            (df['555nm'][i]),
            (df['590nm'][i]),
            (df['630nm'][i]),
            (df['680nm'][i]),
            (df['Clear'][i]),
            (df['NIR'][i]),
        ]
        corrected.append(wavelength_corrected)
        # name_list.append(df['SampleID'][i])
        name_list.append(df['Material'][i])
        outputs.append(output)
corrected = np.array(corrected)
outputs = np.array(outputs)


########### PLOT PLOT ############
#green, 950, 1050,1200,1300,1450,1550,1650
# Displaying clusters
#ratio1 = corrected[0::, 1]/corrected[0::, 2]  # 950/1050 y axis
#ratio2 = corrected[0::, 3]/corrected[0::, 4]  # 1200/1300 x axis
#ratio3 = corrected[0::, 5]/corrected[0::, 6]  # 1450/1550 z axis

# ratio1 = corrected[0::, 3]/corrected[0::, 4]  # 1200/1300 y axis
ratio1 = corrected[0::, 2]/corrected[0::, 3]  # 1050/1200 y axis
ratio2 = corrected[0::, 5]/corrected[0::, 6]  # 1450/1550 x axis
ratio3 = corrected[0::, 6]/corrected[0::, 7]  # 1550/1650 z axis

fig = px.scatter_3d(df, x=ratio2, y=ratio1, z=ratio3, color=name_list)
# fig = px.scatter_3d(df, x=ratio1, y=ratio2, z=ratio3, color=name_list)

#fig.ylabel('950/1050', fontsize=18)
#fig.xlabel('1050/1200', fontsize=18)
#fig.zlabel('1300/1450', fontsize=18)

fig.update_layout(scene=dict(
    yaxis_title='1200/1300 nm',
    xaxis_title='1450/1550 nm',
    zaxis_title='1550/1650 nm'),
)

fig.update_scenes(yaxis_autorange="reversed")
fig.show()


input("VENTER")

####################


print(corrected)
print("")
print(output)

# Beer's law only applied on IR values. 
law = []
for i in range(len(corrected)):
    beers_law = []
    beers_law += [
        (corrected[i][1]/corrected[i][0]),
        (corrected[i][2]/corrected[i][1]),
        (corrected[i][3]/corrected[i][2]),
        (corrected[i][4]/corrected[i][3]),
        (corrected[i][5]/corrected[i][4]),
        (corrected[i][6]/corrected[i][5]),
        (corrected[i][7]/corrected[i][6]),

        (corrected[i][8]),
        (corrected[i][9]),
        (corrected[i][10]),
        (corrected[i][11]),
        (corrected[i][12]),
        (corrected[i][13]),
        (corrected[i][14]),
        (corrected[i][15]),
        (corrected[i][16]),
        (corrected[i][17]), ]
    law.append(beers_law)
law = np.array(law)
print("HERES THE LAW")
print(law)
print("")
print("")

# Find the max and minimums for normalizing
min = []
max = []
for i in range(17):
    max.append(np.max(law[0::, i]))
    min.append(np.min(law[0::, i]))
print("min")
print(min)
print("max")
print(max)


# Calculate the minimum and maximum values
min_val = np.min(law)
max_val = np.max(law)

# Normalize the values
normalized_law = (law - min_val) / (max_val - min_val)
print("Normalized law manual")
print(normalized_law)


# input("HEST0")

# Now normalize
normalized = []
for i in range(len(law)):
    item_normalized = []
    item_normalized += [
        (law[i][0]-min[0])/(max[0]-min[0]),
        (law[i][1]-min[1])/(max[1]-min[1]),
        (law[i][2]-min[2])/(max[2]-min[2]),
        (law[i][3]-min[3])/(max[3]-min[3]),
        (law[i][4]-min[4])/(max[4]-min[4]),
        (law[i][5]-min[5])/(max[5]-min[5]),
        (law[i][6]-min[6])/(max[6]-min[6]),
        (law[i][7]-min[7])/(max[7]-min[7]),
        (law[i][8]-min[8])/(max[8]-min[8]),
        (law[i][9]-min[9])/(max[9]-min[9]),
        (law[i][10]-min[10])/(max[10]-min[10]),
        (law[i][11]-min[11])/(max[11]-min[11]),
        (law[i][12]-min[12])/(max[12]-min[12]),
        (law[i][13]-min[13])/(max[13]-min[13]),
        (law[i][14]-min[14])/(max[14]-min[14]),
        (law[i][15]-min[15])/(max[15]-min[15]),
        (law[i][16]-min[16])/(max[16]-min[16]),
    ]
    normalized.append(item_normalized)
inputs = np.array(normalized)
np.set_printoptions(suppress=True, precision=4)
# print(inputs)

print("The normalized data!")
print(item_normalized)
# In order to replicate the same results, a seed is set.
SEED = 1338
np.random.seed(SEED)
tf.random.set_seed(SEED)
print("Data set parsing and preparation complete.")


# Randomize the order of the inputs, so they can be evenly distributed for training, testing, and validation
# https://stackoverflow.com/a/37710486/2020087

#######
# USE RAW DATA AS INPUT FOR TRAINING
# inputs = corrected
print(inputs)
# input("HEST")
#######

num_inputs = len(inputs)
randomize = np.arange(num_inputs)
np.random.shuffle(randomize)

# Swap the consecutive indexes (0, 1, 2, etc) with the randomized indexes
inputs = inputs[randomize]
outputs = outputs[randomize]

# Split the recordings (group of samples) into three sets: training, testing and validation
TRAIN_SPLIT = int(0.6 * num_inputs)
TEST_SPLIT = int(0.2 * num_inputs + TRAIN_SPLIT)

inputs_train, inputs_test, inputs_validate = np.split(
    inputs, [TRAIN_SPLIT, TEST_SPLIT])
outputs_train, outputs_test, outputs_validate = np.split(
    outputs, [TRAIN_SPLIT, TEST_SPLIT])

print(inputs_test)
# input("HEST2")

print("Data set randomization and splitting complete.")

# Define the Tensor Flow model.
model = tf.keras.Sequential()
# relu is used for performance
model.add(tf.keras.layers.Dense(30, activation='relu'))
model.add(tf.keras.layers.Dense(30, activation='relu'))
model.add(tf.keras.layers.Dense(15, activation='relu'))
# softmax is used, because we only expect one PLASTICTYPE to occur per input
model.add(tf.keras.layers.Dense(NUM_PLASTICTYPES, activation='softmax'))
# model.compile(optimizer='rmsprop', loss='mse', metrics=['mae'])
model.compile(optimizer='SGD', loss='mse', metrics=[
              'mae'])  # Trying SGD as optimizer
history = model.fit(inputs_train, outputs_train, epochs=1200,
                    batch_size=1, validation_data=(inputs_validate, outputs_validate))


# Use the model to predict the test inputs
predictions = model.predict(inputs_test)

# print the predictions and the expected ouputs
print("predictions =\n", np.round(predictions, decimals=3))
print("actual =\n", outputs_test)

test_accuracy = tf.keras.metrics.Accuracy()
test_accuracy(predictions, outputs_test)

print("Test set accuracy: {:.3%}".format(test_accuracy.result()))


fromScannerTestPPPS = [
    [0.36411407589912, 0.87139588594437, 0.31658437848091, 0.15997105836868, 0.78538304567337, 0.32645702362061, 0.44710183143616, 0.00000000000000, 1.00000000000000,
        0.38778707385063, 0.46999090909958, 0.47538965940475, 0.90465152263641, 0.66085833311081, 0.24207453429699, 0.79517394304276, 0.03895216062665],
    [0.30648157000542, 0.57267540693283, 0.27675411105156, 0.18273611366749, 0.55909007787704, 0.29688769578934, 0.36690530180931, 0.00000000000000, 1.00000000000000,
        0.30894136428833, 0.26388061046600, 0.54956573247910, 0.67616117000580, 0.68793195486069, 0.21822381019592, 0.63497424125671, 0.02392266318202],
]
singlePredictionTestPPPS = model.predict(fromScannerTestPPPS)
print("Prediction from a red cup PP scan, followed by a red cup PS scan.")
print(singlePredictionTestPPPS)

#print("another one but HDPE this time")
#fromScannerTestHDPE = [[0.14268784224987,0.41947510838509,0.11952741444111,0.05588787794113,0.35205727815628,0.15265330672264,0.18896181881428,0.00000000000000,1.00000000000000,0.17040336132050,0.03717001900077,0.78630179166794,0.13381703197956,0.13977921009064,0.09612206369638,0.66020208597183,0.00893210899085]]
#singlePredictionTestHDPE = model.predict(fromScannerTestHDPE)
#print("Prediction from a single red cup PP scan")
# print(singlePredictionTestHDPE)


cpp_code = tf_porter(model, inputs_train, outputs_train).to_cpp(
    instance_name='barbie')
print("")
print("ML CODE")
print("")


f = open("barbie.h", "a")
f.write(cpp_code)
f.close()

# from tinymlgen import port
# tf_model = create_tf_model()


# print(cpp_code)

# # Convert the model to the TensorFlow Lite format without quantization
# converter = tf.lite.TFLiteConverter.from_keras_model(model)
# tflite_model = converter.convert()
#
# # Save the model to disk
# open("plastic_model.tflite", "wb").write(tflite_model)
#
# basic_model_size = os.path.getsize("plastic_model.tflite")
# print("Model is %d bytes" % basic_model_size)
#
#
# #######
# with open('model.h', 'wb') as f:
#     f.write(tflite_model)
#
# #####
#
#
# model_h_size = os.path.getsize("model.h")
# print(f"Header file, model.h, is {model_h_size:,} bytes.")
# print("\nOpen the side panel (refresh if needed). Double click model.h to download the file.")
#
# tf.saved_model.save(model, "test")
#
