# Define linking information for each platform.
set(${PROJECT_NAME}_Windows readline PARENT_SCOPE)
set(${PROJECT_NAME}_Unix readline PARENT_SCOPE)

dt_project(library)
dt_folder("third-party")

