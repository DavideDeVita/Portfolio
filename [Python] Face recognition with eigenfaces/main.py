from DatasetLoader import DatasetLoader
from FaceSpace import FaceSpace
from Utility import showImage
from Utility import createRandomImage
import numpy as np

path_dataset_yale = "./Yale_Cropped_Dataset/" #images are 168 x 192, 38 dir
path_standard_dataset = "./dataset/" # images are 92 x 112, 40 dir
width_image, height_image, n_directories  = 92, 112, 40 #standard one
#width_image, height_image, n_directories  = 168, 192, 38 #for yale
vectorial_image_size = width_image * height_image
dataset = DatasetLoader( path_standard_dataset, width_image, height_image, n_directories )
training_set = dataset.getTrainingSet()
test_set = dataset.getTestSet()
training_set_labels = dataset.getTrainingSetLabels()
test_set_labels = dataset.getTestSetLabels()
print("training set imgs ", training_set[0, :].size)
print("test set imgs ", test_set[0, :].size)
face_space = FaceSpace(training_set, training_set_labels)
face_space.calculateTestsetAccuracy(test_set, test_set_labels)