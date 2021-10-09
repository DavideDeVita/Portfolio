import matplotlib.pyplot as plt
import numpy as np
import os

def showImage( vectorialized_image, height, width ):
    plt.imshow(vectorialized_image.reshape((height, width)), cmap='gray')
    plt.show()

def createRandomImage(size):
    return np.random.randint(0, 256, (size))

def renameFileInsideDirectories():
    path="./datasetYale/Yale_Cropped_Dataset/YaleB"
    for i in range(1,39):
        try:
            new_path = path+str(i).zfill(2)
            new_file_name = 1
            for filename in os.listdir(new_path):
                if(filename.endswith(".pgm")):
                    os.rename(new_path + "/" + filename, new_path + "/" + str(new_file_name) + ".pgm")
                    new_file_name = new_file_name + 1
        except:
            print("La cartella "+ path+str(i).zfill(2) +" non esiste")

def renameDirectories():
    path="./datasetYale/Yale_Cropped_Dataset/"
    for i in range(1,39):
        try:
            old_path = path+"YaleB"+str(i).zfill(2)
            os.rename(old_path, path+"s"+str(i))
        except:
            print("La cartella "+ path+str(i).zfill(2) +" non esiste")