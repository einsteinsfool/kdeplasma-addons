add_definitions(-DTRANSLATION_DOMAIN="plasma_runner_CharacterRunner")

set(krunner_charrunner_SRCS charrunner.cpp)
set(kcm_krunner_charrunner_SRCS charrunner_config.cpp)

ki18n_wrap_ui(kcm_krunner_charrunner_SRCS charrunner_config.ui)
add_library(kcm_krunner_charrunner MODULE ${kcm_krunner_charrunner_SRCS})
target_link_libraries(kcm_krunner_charrunner
    KF5::Runner
    KF5::KCMUtils
    KF5::I18n
)

# Now make sure all files get to the right place
add_library(krunner_charrunner MODULE ${krunner_charrunner_SRCS})
target_link_libraries(krunner_charrunner
    KF5::Runner
    KF5::I18n
)
add_dependencies(krunner_charrunner kcm_krunner_charrunner)

# Install the library and .desktop file
install(TARGETS krunner_charrunner kcm_krunner_charrunner DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES CharacterRunner.desktop CharRunner_config.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
