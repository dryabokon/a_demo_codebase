import config_base
# ----------------------------------------------------------------------------------------------------------------------
class cnfg_experiment(config_base.cnfg_base):

    folder_out = None
    host_mlflow = 'http://127.0.0.1'
    port_mlflow = 5000
    # ----------------------------------------------------------------------------------------------------------------------
    experiment_name = 'titanic'
    param_A = 1
    param_B = 2
