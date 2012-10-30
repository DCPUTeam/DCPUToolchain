cmake_minimum_required(VERSION 2.8.5)
project(BULLET_PHYSICS)

# Set Bullet variables.
set(BULLET_VERSION 2.81)

# Include source folders.
add_subdirectory(src)

# Place into "third-party" folder.
set_property(TARGET BulletCollision PROPERTY FOLDER "third-party")
set_property(TARGET BulletDynamics PROPERTY FOLDER "third-party")
set_property(TARGET BulletSoftBody PROPERTY FOLDER "third-party")
set_property(TARGET LinearMath PROPERTY FOLDER "third-party")

