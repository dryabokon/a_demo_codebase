# ccc
import utils_experiment_ops
import config
# ---------------------------------------------------------------------------------------------------------------------
config = config.cnfg_experiment()
E = utils_experiment_ops.Experimentor(folder_runs='./runs/',config=config)
# ---------------------------------------------------------------------------------------------------------------------
if __name__ == "__main__":

    E.run_experiment()