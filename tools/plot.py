import pandas as pd
import matplotlib.pyplot as plt
import sys

plt.imshow(pd.read_csv(sys.argv[1]))
plt.show()
