# SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

import numpy as np
import matplotlib.pyplot as plt

# Number of processors: 2^0 ... 2^16
k = np.arange(0, 17)
N = 2**k

x = 2**np.linspace(k[0], k[-1])

# Parallel fractions
parallel_portions = [0.5, 0.75, 0.9, 0.95]

def amdahl_speedup(p, N):
    return 1 / ((1 - p) + (p / N))


cmap = plt.get_cmap("tab10")
colors = [cmap(i) for i in range(len(parallel_portions))]

# Plot
plt.figure(figsize=(6, 4))

for p, color in zip(parallel_portions, colors):
    plt.plot(x, amdahl_speedup(p, x), lw=2,
             color=color, label=f"{int(p*100)}%")

plt.xscale('log', base=2)
plt.xlabel("Number of processors")
plt.ylabel("Speedup")
plt.title("Amdahl's law")

plt.xlim(N[0], N[-1])
plt.xticks(N, N, rotation=90)
plt.yticks(np.arange(0, 21, 2))
plt.grid(True, which='both', alpha=0.5)
plt.legend(title="Parallel portion", frameon=True)
plt.tight_layout()

plt.savefig("amdahl.svg", format="svg")
# plt.show()
