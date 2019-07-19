CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

INCLUDE(ExternalProject)


FUNCTION(BUILD_ODEINT BUILD_DIR)

  SET(ODEINT_SRC ${BUILD_DIR}/src)

  EXTERNALPROJECT_ADD(
    odeint-v2
    GIT_REPOSITORY https://github.com/headmyshoulder/odeint-v2.git
    GIT_TAG db8b39ae27346e501941bf705362a1f4879b653c
    SOURCE_DIR ${ODEINT_SRC}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )

  SET(ODEINT_INCLUDE_DIRS ${BUILD_DIR}/src/include PARENT_SCOPE)

  ADD_DEPENDENCIES(DEPS odeint-v2)

ENDFUNCTION(BUILD_ODEINT)
