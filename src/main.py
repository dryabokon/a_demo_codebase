# ccc
import utils_experiment_ops
import config
import time
# ---------------------------------------------------------------------------------------------------------------------
config = config.cnfg_experiment()
E = utils_experiment_ops.Experimentor(folder_runs='./runs/',config=config)
# ---------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    start_time = time.time()
    E.run_experiment()
    end_time = time.time()
    duration = end_time - start_time
    print(f'Total duration: {duration:.2f} seconds')