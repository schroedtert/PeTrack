#########################################################
#                    INSTALL                            #
#########################################################

include(InstallRequiredSystemLibraries)
include(GNUInstallDirs)

install(TARGETS petrack)

# install Qwt and OpenCV
install(CODE "
include(BundleUtilities)
fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/petrack${CMAKE_EXECUTABLE_SUFFIX}\"  \"\" \"\")
")

# NOTE: For Future Mac installer, use macdeployqt utility (think about the licenses as well)
if (WIN32)
    # Install Qt dlls
    set (WINDEPLOYQT_APP \"${Qt5Core_DIR}/../../../bin/windeployqt\")
    install(CODE "
        message(\"\${CMAKE_INSTALL_PREFIX}/petrack.exe\")
        execute_process(COMMAND ${WINDEPLOYQT_APP} \"\${CMAKE_INSTALL_PREFIX}/bin/petrack.exe\")
    ")

    # NOTE: Paths might be platform specific
    install(DIRECTORY "${Qt5Core_DIR}/../../../../../Licenses" DESTINATION "Licenses/Qt_Licenses")
    install(DIRECTORY "${OpenCV_DIR}/etc/licenses" DESTINATION "Licenses/OpenCV_Licenses")
    install(FILES "${OpenCV_DIR}/LICENSE/" DESTINATION "Licenses/OpenCV_Licenses")
endif()

install(FILES "${CMAKE_SOURCE_DIR}/ReadMe.md" "${CMAKE_SOURCE_DIR}/LICENSE" DESTINATION ".")


##################################################################
#                         PACKAGE                                #
##################################################################

set(CPACK_PACKAGE_FILE_NAME "petrack-installer-${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "Forschungszentrum Juelich GmbH")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERISON_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERISON_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERISON_PATCH})
set(CPACK_PACKAGE_DESCRIPTION "PeTrack is a software for the automated extraction of pedestrian trajectories from videos.")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/ReadMe.md")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://www.fz-juelich.de/ias/ias-7/EN/Expertise/Software/PeTrack/petrack.html")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/ReadMe.md")
set(CPACK_PACKAGE_EXECUTABLES "petrack;PeTrack")
set(CPACK_MONOLITHIC_INSTALL TRUE)
set(CPACK_CREATE_DESKTOP_LINKS petrack)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")


if(WIN32)
    set(CPACK_GENERATOR "NSIS")

    set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}\\\\petrack.ico")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_DISPLAY_NAME "PeTrack")
    set(CPACK_NSIS_PACKAGE_NAME "PeTrack")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "${CMAKE_SOURCE_DIR}\\\\petrack.ico")
    set(CPACK_NSIS_HELP_LINK "https://www.fz-juelich.de/ias/ias-7/EN/Expertise/Software/PeTrack/petrack.html")
    set(CPACK_NSIS_URL_INFO_ABOUT ${CPACK_NSIS_HELP_LINK})
    set(CPACK_NSIS_CONTACT "petrack@fz-juelich.de")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
      WriteRegStr HKCU 'Software\\\\Forschungszentrum Juelich GmbH\\\\PeTrack' 'fileExtension' 'pet'
      WriteRegStr HKCU 'Software\\\\Forschungszentrum Juelich GmbH\\\\PeTrack' 'home' '$INSTDIR\\\\bin'
      WriteRegStr HKCR '.pet' '' 'petfile'
      WriteRegStr HKCR 'petfile' '' 'PeTrack Project File'
      WriteRegStr HKCR 'petfile\\\\DefaultIcon' '' '$INSTDIR\\\\bin\\\\petrack.exe,0'
      WriteRegStr HKCR 'petfile\\\\shell\\\\open\\\\command' '' '$INSTDIR\\\\bin\\\\petrack.exe %1'
      WriteRegStr HKLM 'Software\\\\RegisteredApplications' 'petrack' '$INSTDIR\\\\petrack.exe'
      WriteRegStr HKCU 'Software\\\\RegisteredApplications' 'petrack' '$INSTDIR\\\\petrack.exe'
      WriteRegStr HKCU 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Explorer\\\\FileExts\\\\.pet\\\\OpenWithList' 'a' '$INSTDIR\\\\bin\\\\petrack.exe'
      ")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
      DeleteRegKey HKCU 'Software\\\\Forschungszentrum Juelich GmbH\\\\PeTrack'
      DeleteRegKey HKCR '.pet'
      DeleteRegKey HKCR 'petfile'
      DeleteRegKey HKLM 'Software\\\\RegisteredApplications\\\\petrack'
      DeleteRegKey HKCU 'Software\\\\RegisteredApplications\\\\petrack'
      DeleteRegKey HKCU 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Explorer\\\\FileExts\\\\.pet'
    ")
endif()

include(CPack)






