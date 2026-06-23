# SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

import numpy as np
import matplotlib.pyplot as plt

def analyze(name):
    data = np.loadtxt(f"{name}.txt")
    n = data[:,0]
    time = data[:,1]

    cost = n * time
    speedup = time[0] / time
    eff = speedup / n

    if name == "full-nodes":
        label = "Number of nodes"
        print("| Nodes | Runtime (s) | Resource cost (Node-s) | Speedup | Parallel efficiency |")
    else:
        label = "Number of cores within a single node"
        print("| Cores | Runtime (s) | Resource cost (Core-s) | Speedup | Parallel efficiency |")
    print("| ----: | ----------: | ---------------------: | ------: | ------------------: |")
    for x, t, c, s, e in zip(n, time, cost, speedup, eff):
        print(f"| {int(x):5d} | {t:11.4f} | {c:22.4f} |  {s:6.4f} | {e*100:19.4f} |")

    cmap = plt.get_cmap("tab10")
    fig, ax1 = plt.subplots()

    handles = []

    if name == "full-nodes":
        h, = ax1.plot(n, n, color='k', label='Ideal speedup')
        handles.append(h)
    h, = ax1.plot(n, speedup, 'o-', color=cmap(0), label='Speedup')
    handles.append(h)
    ax1.set_xlabel(label)
    ax1.set_ylabel("Speedup")
    ax2 = ax1.twinx()
    h, = ax2.plot(n, eff * 100, 's--', color=cmap(1), label='Efficiency')
    handles.append(h)
    ax2.set_ylabel("Parallel efficiency (%)")

    ax1.legend(handles=handles, loc='upper center')
    plt.tight_layout()
    plt.savefig(f"{name}.svg", format="svg")
    #plt.show()

analyze("full-nodes")
# analyze("single-node")

