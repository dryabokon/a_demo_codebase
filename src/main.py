import os
import pandas as pd
import subprocess
import json
# ----------------------------------------------------------------------------------------------------------------------
filename_in = "./input/dataset_titanic.csv"
folder_out = "./output/"
# ----------------------------------------------------------------------------------------------------------------------
import config
import tools_ML_v2
import tools_DF
import tools_MLflower
import tools_IO
import classifier_Gauss
# ----------------------------------------------------------------------------------------------------------------------
config = config.cnfg_experiment()
params = {k: getattr(config, k) for k in dir(config) if not k.startswith('__') and not callable(getattr(config, k)) and k != 'parser'}
# ----------------------------------------------------------------------------------------------------------------------
Classifier = classifier_Gauss.classifier_Gauss()
ML = tools_ML_v2.ML(Classifier,folder_out=folder_out)
MLFlower = tools_MLflower.MLFlower(config.host_mlflow, config.port_mlflow)
# ----------------------------------------------------------------------------------------------------------------------
def save_artifacts_to_MLFlow():
    if MLFlower.is_available:
        with open(folder_out + 'config.md', 'w') as f:
            f.write(tools_DF.prettify(pd.DataFrame([(k, v) for k, v in zip(*config.get_keys_values())if k != 'parser'], columns=['param', 'value'])))

        params['git_commit'] = subprocess.check_output(['git', 'rev-parse', 'HEAD']).decode().strip()
        params['git_branch'] = subprocess.check_output(['git', 'rev-parse', '--abbrev-ref', 'HEAD']).decode().strip()
        dct_metrics = dict(zip(df_metrics[df_metrics.columns[0]].values, df_metrics['train'].values))
        artifacts = [folder_out + f for f in tools_IO.get_filenames(folder_out,'*.*')]
        MLFlower.save_experiment(config.experiment_name,params=params,metrics=dct_metrics,artifacts=artifacts)

    return
# ----------------------------------------------------------------------------------------------------------------------
def save(df_metrics):

    os.makedirs(folder_out, exist_ok=True)
    with open(os.path.join(folder_out, 'metrics.md'), 'w') as f:
        f.write(tools_DF.prettify(df_metrics))

    return
# ----------------------------------------------------------------------------------------------------------------------
if __name__ == '__main__':

    filename_in = config.patch_path(filename_in)
    tools_IO.remove_files(folder_out, '*.*')
    df = tools_DF.hash_categoricals(pd.read_csv(filename_in,sep='\t').drop('alive', axis=1))
    df_metrics = ML.E2E_train_test_df(df,idx_target=0,do_charts=True,do_density=True,do_pca = True,description='')
    save(df_metrics)
    print(tools_DF.prettify(df_metrics))




