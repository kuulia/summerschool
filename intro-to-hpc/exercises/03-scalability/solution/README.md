<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Discussion

The obtained data is below.

| Nodes | Runtime (s) | Resource cost (Node-s) | Speedup | Parallel efficiency | Electricity cost (Wh) | Monetary cost (€) |
| ----: | ----------: | ---------------------: | ------: | ------------------: | ---------------------: | ----------------: |
|     1 |     31.5086 |                31.5086 |  1.0000 |            100.0000 |                   5.25 |             0.035 |
|     2 |     17.0895 |                34.1789 |  1.8437 |             92.1871 |                   5.70 |             0.038 |
|     4 |      9.7534 |                39.0136 |  3.2305 |             80.7631 |                   6.50 |             0.043 |
|     8 |      5.5704 |                44.5632 |  5.6564 |             70.7054 |                   7.42 |             0.049 |
|    16 |      3.4241 |                54.7854 |  9.2020 |             57.5128 |                   9.13 |             0.061 |

The node count of 4 gives a higher parallel efficiency than 75%, but using more resources starts to get inefficient.

Even though supercomputer resources are typically free of charge for academic users, they still consume electricity, and in addition to direct electricity costs have lots of other costs (procuring the system, cooling, adminstration, ...). Even though in these simple tests both the electricity and monetary costs are tiny, in production simulations these increase quite a lot. 
For example, using 20 nodes for one month would mean 8640 kWh and 57 600 €. As a reference, the amount electricity is more than average Finnish household spends in a year. CSC (and national supercomputing centers in general) can typically provide resources 
more cost effectively than cloud providers, but the real monetary cost would is still tens of thousands of euros (CSC's price for commercial customers would be 31 010 €).
