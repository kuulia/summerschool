# SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D


class PlotData:
    def __init__(self) -> None:
        self.labels = []
        self.markers = []
        self.measurements = []
        self.colors = []
        self.handles = []
        self.linestyle = "-"
        self.lw = 1.0
        self.ms = 1.0


def plot(pd: PlotData, name):
    f = plt.figure(figsize=(32, 18), dpi=120)
    ax = f.add_subplot(111)
    plt.subplots_adjust(
        left=0.105, right=0.9985, top=0.9725, bottom=0.108, wspace=0.2, hspace=0.2
    )

    for i, data in enumerate(pd.measurements):
        sizes = data[:, 0]
        with_overhead = data[:, 2]

        marker = pd.markers[i]
        color = pd.colors[i]
        lw = pd.lw
        ms = pd.ms
        linestyle = pd.linestyle

        ax.loglog(
            sizes,
            with_overhead,
            linestyle=linestyle,
            marker=marker,
            color=color,
            lw=lw,
            ms=ms,
        )

    ax.tick_params(
        axis="both",
        which="major",
        labelsize=64,
        length=20,
        width=5,
        grid_linewidth=2.0,
    )
    ax.tick_params(
        axis="both",
        which="minor",
        labelsize=64,
        length=15,
        width=2.5,
        labelleft=True,
    )

    ax.set_xlim(50, 2e9)
    ax.set_ylim(1e1, 1e11)

    ax.spines["left"].set_linewidth(3)
    ax.spines["right"].set_linewidth(3)
    ax.spines["bottom"].set_linewidth(3)
    ax.spines["top"].set_linewidth(3)

    leg = ax.legend(
        handles=pd.handles,
        labels=pd.labels,
        loc="upper left",
        handlelength=10,
    )

    ax.set_xlabel("vector size", fontsize=64)
    ax.set_ylabel("runtime [ns]", fontsize=64, rotation=90, labelpad=10)

    for txt in leg.get_texts():
        txt.set_fontsize(64)
    if leg.get_title():
        leg.get_title().set_fontsize(64)

    plt.grid(True)
    plt.savefig("runtimes_" + str(name) + ".png")


def main():
    plot_data = {}

    markers = {"hip": "o", "omp": "s", "serial": "D"}
    mpl_default_hex = [
        "#1f77b4",  # tab:blue
        "#ff7f0e",  # tab:orange
        "#2ca02c",  # tab:green
        "#d62728",  # tab:red
        "#9467bd",  # tab:purple
        "#8c564b",  # tab:brown
    ]
    linestyle = "-"
    lw = 5.0
    ms = 20.0

    plot_data["all"] = PlotData()
    plot_data["all"].linestyle = linestyle
    plot_data["all"].lw = lw
    plot_data["all"].ms = ms

    for name in ["hip", "omp", "serial"]:
        plot_data[name] = PlotData()
        plot_data[name].linestyle = linestyle
        plot_data[name].lw = lw
        plot_data[name].ms = ms

        for i, nterms in enumerate([0, 1, 2, 4, 8, 16]):
            if nterms not in plot_data:
                plot_data[nterms] = PlotData()
                plot_data[nterms].linestyle = linestyle
                plot_data[nterms].lw = lw
                plot_data[nterms].ms = ms

            fname = "data/" + name + "_" + str(nterms) + ".dat"

            data = np.genfromtxt(fname, delimiter=",")
            label = name + " N = " + str(nterms)
            marker = markers[name]
            color = mpl_default_hex[i]
            handle = Line2D(
                [0],
                [0],
                linestyle=linestyle,
                marker=marker,
                color=color,
                lw=lw,
                ms=ms,
            )

            plot_data[name].labels.append(label)
            plot_data[name].markers.append(marker)
            plot_data[name].measurements.append(data)
            plot_data[name].colors.append(color)
            plot_data[name].handles.append(handle)

            plot_data[nterms].labels.append(label)
            plot_data[nterms].markers.append(marker)
            plot_data[nterms].measurements.append(data)
            plot_data[nterms].colors.append(color)
            plot_data[nterms].handles.append(handle)

            plot_data["all"].markers.append(marker)
            plot_data["all"].measurements.append(data)
            plot_data["all"].colors.append(color)

    for name in ["hip", "omp", "serial"]:
        plot(plot_data[name], name)

    for nterms in [0, 1, 2, 4, 8, 16]:
        plot(plot_data[nterms], nterms)

    # Create custom legend labels for the "all" case
    plot_data["all"].labels = [
        "hip",
        "omp",
        "serial",
        "N = 0",
        "N = 1",
        "N = 2",
        "N = 4",
        "N = 8",
        "N = 16",
    ]

    # Create custom legend handles for the "all" case
    plot_data["all"].handles = [
        Line2D(
            [0],
            [0],
            linestyle=linestyle,
            marker=markers["hip"],
            color="#000000",
            lw=lw,
            ms=ms,
        ),
        Line2D(
            [0],
            [0],
            linestyle=linestyle,
            marker=markers["omp"],
            color="#000000",
            lw=lw,
            ms=ms,
        ),
        Line2D(
            [0],
            [0],
            linestyle=linestyle,
            marker=markers["serial"],
            color="#000000",
            lw=lw,
            ms=ms,
        ),
        Line2D([0], [0], linestyle=linestyle, color=mpl_default_hex[0], lw=lw),
        Line2D([0], [0], linestyle=linestyle, color=mpl_default_hex[1], lw=lw),
        Line2D([0], [0], linestyle=linestyle, color=mpl_default_hex[2], lw=lw),
        Line2D([0], [0], linestyle=linestyle, color=mpl_default_hex[3], lw=lw),
        Line2D([0], [0], linestyle=linestyle, color=mpl_default_hex[4], lw=lw),
        Line2D([0], [0], linestyle=linestyle, color=mpl_default_hex[5], lw=lw),
    ]

    plot(plot_data["all"], "all")


if __name__ == "__main__":
    main()
