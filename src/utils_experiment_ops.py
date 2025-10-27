import cv2
import numpy
import os
import time
#----------------------------------------------------------------------------------------------------------------------
class Experimentor():
    def __init__(self,folder_runs,config):
        self.config = config
        self.folder_runs = folder_runs
        self.folder_output_current_experiment = None
        self.prepare_output_folder()
        return
    # ----------------------------------------------------------------------------------------------------------------------
    def get_next_folder_out(self,base_folder_out):
        subfolders = [f.path for f in os.scandir(base_folder_out) if f.is_dir()]
        subfolders = sorted([subfolders[i].split(base_folder_out)[1] for i in range(0, len(subfolders))])

        if len(subfolders) > 0:
            subfolders = numpy.array([int(str(s)) for s in subfolders])
            subfolders = numpy.sort(subfolders)
            subfolders_out = '%03d' % (subfolders[-1] + 1)
        else:
            subfolders_out = '%03d' % (0)
        return base_folder_out + subfolders_out + '/'
    # ----------------------------------------------------------------------------------------------------------------------
    def prepare_output_folder(self):
        if not os.path.exists(self.folder_runs):
            os.mkdir(self.folder_runs)

        self.folder_output_current_experiment = self.get_next_folder_out(self.folder_runs)
        os.mkdir(self.folder_output_current_experiment)

        return self.folder_output_current_experiment
    # ----------------------------------------------------------------------------------------------------------------------
    def run_experiment(self):
        start_time = time.time()
        image = numpy.full((480, 640, 3), (127,125,127), dtype=numpy.uint8) + numpy.random.randint(0, 32, (480, 640, 3), dtype=numpy.uint8) - 32
        cv2.imwrite(self.folder_output_current_experiment + 'image.png', image)
        end_time = time.time()
        duration = end_time - start_time
        print(f'Chck results in {self.folder_output_current_experiment}')
        print(f'Total duration: {duration:.2f} seconds')
        return
    # ----------------------------------------------------------------------------------------------------------------------
