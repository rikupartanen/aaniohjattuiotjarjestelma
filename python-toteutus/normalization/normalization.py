import numpy as np

def normalization(data, mean, var):
  """
  Normalisointi-funktion argumentit:
  - data, 1-4 ulotteinen NumPy-taulukko. 3- ja 4-ulotteisilla taulukoilla viimeisen akselin pituuden tulee olla 1.
  - mean, keskiarvo, jonka mukaan normalisoidaan. Liukuluku.
  - var, varianssi, jonka mukaan normalisoidaan. Liukuluku.
  Palauttaa datan normalisoituna samanmuotoisessa NumPy-taulukossa kuin argumenttina annettu data.
  """
  # Varmistetaan ettei jaeta nollalla
  if(var == 0):
    var = 1e-7

  # Tyhjä taulukko normalisoiduille arvoille
  norm_data = np.ndarray(data.shape)
  dims = len(norm_data.shape)
  orig_dims = dims

  # (b, x, y, 1) muotoisella datalla ajetaan 3D-taulukko kerrallaan
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
    # Normalisoidaan jokainen alkio 2D-datasta
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

def testaus_random(count=1):
  """
  Testi satunnaisdatalle.
  """
  from tensorflow.keras import layers # type: ignore

  print("\nSatunnaistesti 1D-datalla")
  test_summary = []
  for i in range(count):
    nom, denom, bias, bias_mul = np.random.randint(5, 9), np.random.randint(2, 4), np.random.random(1), np.random.randint(2, 9)
    input1D = np.random.random((20,)) * 1.0 * nom / denom + bias * bias_mul
    layer = layers.Normalization(axis=None)
    layer.adapt(input1D)
    mean, var, _ = layer.get_weights()
    tulos_tf = layer(input1D)
    tulos_oma = normalization(input1D, mean, var)
    tf_sama_kuin_oma = np.allclose(tulos_tf, tulos_oma)
    print("1D-testi, ovatko TF ja oma tulos keskenään samanlaiset?", tf_sama_kuin_oma)
    test_summary.append(tf_sama_kuin_oma)
    if(not tf_sama_kuin_oma):
      print("TF mean:\t", np.mean(tulos_tf))
      print("Oma mean:\t", np.mean(tulos_oma))
      print("TF var:\t\t", np.var(tulos_tf))
      print("Oma var:\t", np.var(tulos_oma))
  return test_summary


if __name__ == "__main__":
  testaus1D()
  testaus4D()
  summary = testaus_random(30)
