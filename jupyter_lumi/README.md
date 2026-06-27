<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Setting up Jupyter for courses on LUMI

Create the environment where notebooks are run (see [LUMI documentation](https://docs.lumi-supercomputer.eu/software/installing/container-wrapper/) for more information):

```bash
module load LUMI/25.03 lumi-container-wrapper
conda-containerize new SummerSchool-env.yml --prefix /projappl/project_462001452/jupyter_env
```

Create the course module (see [LUMI documentation](https://docs.lumi-supercomputer.eu/runjobs/webui/jupyter-for-courses/) for more information):

```bash
mkdir -p /projappl/project_462001452/www_lumi_modules
cp SummerSchool.lua SummerSchool-resources.yml /projappl/project_462001452/www_lumi_modules/
```
