-- SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
--
-- SPDX-License-Identifier: MIT

-- Jupyter
local project = "project_462001452"
prepend_path("PATH", pathJoin("/projappl", project, "jupyter_env", "bin"))

-- lab / notebook / empty (defaults to jupyter)
setenv("_COURSE_NOTEBOOK_TYPE", "lab")
