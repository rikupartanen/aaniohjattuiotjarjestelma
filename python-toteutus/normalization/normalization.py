import numpy as np

def normalization(data, mean, var):
  """
  Normalisointi-funktion argumentit:
  - data, 1-4 ulotteinen Numpy-lista. 3- ja 4-ulotteisilla listoilla viimeisen akselin pituuden tulee olla 1.
  - mean, keskiarvo, jonka mukaan normalisoidaan. Liukuluku.
  - var, varianssi, jonka mukaan normalisoidaan. Liukuluku.
  Palauttaa datan normalisoituna samanmuotoisessa Numpy-listassa kuin argumenttina annettu data.
  """
  # Varmistetaan ettei jaeta nollalla
  if(var == 0):
    var = 1e-7

  # Tyhjä taulukko normalisoiduille arvoille
  norm_data = np.ndarray(data.shape)
  dims = len(norm_data.shape)
  orig_dims = dims

  # (b, x, y, 1) muotoisella datalla ajetaan batch kerrallaan
  # (x, y, 1) datamuodossa tätä funktiota rekursiivisesti
  if((dims == 4) and (len(data[0, 0, 0, :]) == 1)):
    batch_size = len(data[:, 0, 0, 0])
    for b in range(batch_size):
      norm_data[b] = normalization(data[b], mean, var)

  # (x, y, 1) muotoisella datalla poistetaan viimeinen akseli
  # ja ajetaan normalsointi (x, y) muotoiselle datalle
  if((dims == 3) and (len(data[0, 0, :]) == 1)):
    data = data[:, :, 0]
    norm_data = np.ndarray(data.shape)
    dims = len(norm_data.shape)

  if(dims == 1):
    # Normalisoidaan jokainen alkio 1D-datasta
    for i in range(len(norm_data)):
      norm_data[i] = (data[i] - mean) / np.sqrt(var)
  elif(dims == 2):
    for i, row in enumerate(norm_data):
      for j in range(len(row)):
        norm_data[i][j] = (data[i][j] - mean) / np.sqrt(var)
    if(orig_dims == 3): # Palautetaan poistettu akseli, jotta outputin muoto on sama kuin inputin
      norm_data = norm_data[..., np.newaxis] # sama kuin norm_data[:, :, np.newaxis]
  elif(dims == 3 or dims == 4):
    pass  # Nämä on käsitelty aiemmin
  else:
    print("Unexpected shape. Expected shapes (x), (x, y), (x, y, 1) and (b, x, y, 1). Got ", data.shape)
    return "Unexpected shape."

  return norm_data

def testaus1D():
  """
  Tensorflow:n / Keraksen esimerkki.
  Vain axis=None toteutus on tässä tuettu.
  """

  from tensorflow.keras import layers # type: ignore

  print("\nTesti 1D datalle")
  input_data = np.array([1., 2., 3.], dtype='float32')
  layer = layers.Normalization(axis=None, mean=3, variance=2)
  print("Tensorflow palautti:")
  print(layer(input_data))
  print("Oma toteutus palautti:")
  print(normalization(input_data, 3, 2))

  return 0

def testaus4D():
  """
  Testi 4D-datalle. Numpy-datatallenteet Simple Audio esimerkistä.
  """
  from tensorflow.keras import layers # type: ignore

  print("\nTesti 4D-datalle. (Batchatulle 3D-datalle)")
  example_ds_input = np.load('ex_ds_after_resize.npy')
  expected_ds_output = np.load('ex_ds_after_norm.npy')
  # Output normalisoitu arvoilla:
  #   Mean  0.12540941
  #   Var   0.5840317

  norm_layer = layers.Normalization(axis=None, mean=0.12540941, variance=0.5840317)
  #norm_layer.adapt(example_ds_input)  # Tämä antaisi: Mean 0.097654596, Var 0.28684083
  tulos_tf = norm_layer(example_ds_input)
  tulos_oma = normalization(example_ds_input, 0.12540941, 0.5840317)
  
  tf_tulos_oikein = np.allclose(tulos_tf, expected_ds_output)
  oma_tulos_oikein = np.allclose(tulos_oma, expected_ds_output)
  tf_sama_kuin_oma = np.allclose(tulos_tf, tulos_oma)
  print("Antaako Tensorflow odotetun tuloksen?\t", tf_tulos_oikein)
  print("Antaako oma toteutus oikean tuloksen?\t", oma_tulos_oikein)
  print("Onko tulokset keskenään samanlaiset?\t", tf_sama_kuin_oma)

  return 0

def devaus():
  """
  Funktion kehityksessä käytettyjä testaus ja debug juttuja.
  """
  # Ota yksittäinen resizattu spektri
  # spectrum_single = np.load('resized_single_spectrogram.npy')
  example_spec_input = np.load('ex_ds_after_resize.npy')[0]
  print(example_spec_input.shape)

  # print(spectrum_single[:, :, 0].shape)
  # print(spectrum_single[0, 0, :].shape)

  #ret = normalization(spectrum_single, 0.125, 0.584)
  #ret = normalization(spectrum_single, 0.04044, 0.02623)
  ret = normalization(example_spec_input, 0.07396522, 0.17421223)
  # Normalisoinnissa:
  #   Keskiarvon tulisi hakeutua kohti nollaa
  #   Varianssin tulisi hakeutua kohti ykköstä
  print(np.mean(example_spec_input))
  print(np.var(example_spec_input))
  print(np.mean(ret))
  print(np.var(ret))

  # Ota datasetti spektrejä
  #spectrum_ds = np.load('resized_example_spectrograms.npy')
  example_ds_input = np.load('ex_ds_after_resize.npy')
  print(example_ds_input.shape)

  # print(spectrum_ds[:, :, :, 0].shape)
  # print(spectrum_ds[0, 0, 0, :].shape)

  #ret = normalization(spectrum_ds, 0.125, 0.584)
  #ret = normalization(spectrum_ds, 0.10691, 0.32921)
  ret = normalization(example_ds_input, 0.097654596, 0.28684083)

  # for b in range(len(spectrum_ds[:, 0, 0, 0])):
  #   #print(np.array([np.mean(ret[b]), np.var(ret[b])]))
  #   new_mean_is_closer_to_zero = np.abs(np.mean(ret[b])) < np.abs(np.mean(spectrum_ds[b]))
  #   new_var_is_closer_to_one = np.abs(1 - np.var(ret[b])) < np.abs(1 - np.var(spectrum_ds[b]))
  #   print(np.array([new_mean_is_closer_to_zero, new_var_is_closer_to_one]))
  
  print(np.mean(example_ds_input))
  print(np.var(example_ds_input))
  print(np.mean(ret))
  print(np.var(ret))
    
  return 0


if __name__ == "__main__":
  testaus1D()
  testaus4D()
  devaus()