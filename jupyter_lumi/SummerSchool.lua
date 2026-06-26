-- Jupyter
local project = "project_462001452"
prepend_path("PATH", pathJoin("/projappl", project, "jupyter_env", "bin"))

-- lab / notebook / empty (defaults to jupyter)
setenv("_COURSE_NOTEBOOK_TYPE", "lab")
