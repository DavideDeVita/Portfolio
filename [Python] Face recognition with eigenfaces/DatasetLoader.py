
import numpy as np
import random
import os

class DatasetLoader:

    path = "./dataset/"
    n_directories = 0
    n_images_per_directory = 0
    width_images = 0
    height_images = 0
    training_set = None
    test_set = None
    training_set_labels = None
    test_set_labels = None

    def __init__(self, path="./dataset/", width_image=92, height_image=112, n_directories=40, n_images_per_directory=10):
        print("create the dataset Loader\n")
        self.path = path
        self.setupDirectoryFormat(n_directories, n_images_per_directory)
        self.setupImgFormat(width_image, height_image)
        self.training_set, self.test_set, self.training_set_labels, self.test_set_labels = self.extractTrainingsetTestset(80)

        #self.showImage(self.training_set[:,0], 112, 92)

    def setupImgFormat(self, width_img=92, height_img=112):
        self.width_images = width_img
        self.height_images = height_img

    def setupDirectoryFormat(self, n_directories=40, n_images_per_directory=10):
        self.n_directories = n_directories
        self.n_images_per_directory = n_images_per_directory


    def extractTrainingsetTestset(self, trainingPercentage):
        training_set = []
        test_set = []
        training_set_labels = []
        test_set_labels = []

        for i in range(1, self.n_directories+1):
            opening_failed = False
            path = self.path + 's'+str(i)+'/'
            n_images_per_directory = self.computeNumberOfImagesPerDirectory(path)
            n_images_per_directory = 10
            data_list = [i for i in range(1, n_images_per_directory + 1)]
            number_of_training_images_per_directories = int((n_images_per_directory * trainingPercentage) / 100)
            number_of_testing_images_per_directories = n_images_per_directory - number_of_training_images_per_directories
            test_list = random.sample(data_list, number_of_testing_images_per_directories)
            #test_list = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
            train_list = list(set(data_list) - set(test_list))
            for j in train_list:
                vectorialized_image, opening_failed = self.readPgm(path+str(j)+'.pgm', j, i)
                if(not opening_failed):
                    training_set.append(vectorialized_image)
                    training_set_labels.append(i)

            for k in test_list:
                vectorialized_image, opening_failed = self.readPgm(path+str(k)+'.pgm', k, i)
                if(not opening_failed):
                    test_set.append(vectorialized_image)
                    test_set_labels.append(i)

        return np.stack(training_set, axis=-1), np.stack(test_set, axis=-1), training_set_labels, test_set_labels


    def computeNumberOfImagesPerDirectory(self, path):
        counter = 0
        for filename in os.listdir(path):
            if( filename.endswith(".pgm") ):
                counter = counter + 1
        return counter

    def readPgm(self, path, number_of_image, number_of_diretory):
        opening_failed = True
        pgmf = open(path, 'rb')
        try:
            magic_number = pgmf.readline()
            width, height = [int(i) for i in pgmf.readline().split()]
            max_val = int(pgmf.readline())
            assert max_val <= 255

            vectorialized_image = []
            for k in range(height):
                for j in range(width):
                    vectorialized_image.append(ord(pgmf.read(1)))
            pgmf.close()
            opening_failed = False
            return np.array(vectorialized_image, dtype='float') / 255.0, opening_failed
        except:
            print("Problemi con l'immagine ",number_of_image ,"della directory", number_of_diretory)
            return np.zeros(self.width_images*self.height_images), opening_failed

    def getTrainingSet(self):
        return self.training_set

    def getTestSet(self):
        return self.test_set

    def getTrainingSetLabels(self):
        return  self.training_set_labels

    def getTestSetLabels(self):
        return  self.test_set_labels
