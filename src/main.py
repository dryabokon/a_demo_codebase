#---CCC---
import pandas as pd
filename_in = "./input/dataset_titanic.csv"
folder_out = "./output/"
# ----------------------------------------------------------------------------------------------------------------------
import config
import tools_ML_v2
import tools_DF
import tools_MLflower
import classifier_Gauss
# ----------------------------------------------------------------------------------------------------------------------
config = config.cnfg_experiment()
params = {k: getattr(config, k) for k in dir(config) if not k.startswith('__') and not callable(getattr(config, k)) and k != 'parser'}
# ----------------------------------------------------------------------------------------------------------------------
Classifier = classifier_Gauss.classifier_Gauss()
ML = tools_ML_v2.ML(Classifier,folder_out=folder_out)
MLFlower = tools_MLflower.MLFlower(config.host_mlflow, config.port_mlflow)
# ----------------------------------------------------------------------------------------------------------------------
if __name__ == '__main__':

    filename_in = config.patch_path(filename_in)

    df = pd.read_csv(filename_in,sep='\t')
    df = df.drop('alive', axis=1)
    df = tools_DF.hash_categoricals(df)
    df_metrics = ML.E2E_train_test_df(df,idx_target=0,do_charts=True,do_density=True,do_pca = True,description='')
    print(tools_DF.prettify(df_metrics))
    if MLFlower.is_available:
        dct_metrics = dict(zip(df_metrics[df_metrics.columns[0]].values, df_metrics['train'].values))
        MLFlower.save_experiment(config.experiment_name,params=params,metrics=dct_metrics,artifacts=[])




