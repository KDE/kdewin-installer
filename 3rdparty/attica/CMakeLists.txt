
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
#set(LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Library directory name" FORCE)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH  TRUE)

find_package(Qt4 REQUIRED)
include_directories( ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} )

# mingw can't handle exported explicit template instantiations in a DLL

configure_file(version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

set(ocsclient_SRCS
   accountbalance.cpp
   accountbalanceparser.cpp
   achievement.cpp
   achievementparser.cpp
   buildservice.cpp
   buildserviceparser.cpp
   buildservicejob.cpp
   buildservicejobparser.cpp
   buildservicejoboutput.cpp
   buildservicejoboutputparser.cpp
   activity.cpp
   activityparser.cpp
   atticabasejob.cpp
   atticautils.cpp
   privatedata.cpp
   privatedataparser.cpp
   category.cpp
   categoryparser.cpp
   comment.cpp
   commentparser.cpp
   content.cpp
   contentparser.cpp
   deletejob.cpp
   distribution.cpp
   distributionparser.cpp
   downloaddescription.cpp
   downloaditem.cpp
   downloaditemparser.cpp
   event.cpp
   eventparser.cpp
   folder.cpp
   folderparser.cpp
   forum.cpp
   forumparser.cpp
   getjob.cpp
   homepageentry.cpp
   homepagetype.cpp
   homepagetypeparser.cpp
   icon.cpp
   itemjob.cpp
   knowledgebaseentry.cpp
   knowledgebaseentryparser.cpp
   license.cpp
   licenseparser.cpp
   listjob_inst.cpp
   message.cpp
   messageparser.cpp
   metadata.cpp
   parser.cpp
   person.cpp
   personparser.cpp
   platformdependent_v2.cpp
   postfiledata.cpp
   postjob.cpp
   project.cpp
   projectparser.cpp
   putjob.cpp
   remoteaccount.cpp
   remoteaccountparser.cpp
   provider.cpp
   providermanager.cpp
   publisher.cpp
   publisherparser.cpp
   publisherfield.cpp
   publisherfieldparser.cpp
   qtplatformdependent.cpp
   topic.cpp
   topicparser.cpp
)

qt4_automoc(${ocsclient_SRCS})

add_library(attica STATIC ${ocsclient_SRCS})

#set_target_properties(attica PROPERTIES
#    VERSION ${CMAKE_LIBATTICA_VERSION_STRING}
#    SOVERSION ${CMAKE_LIBATTICA_VERSION_MAJOR}
#    LINK_INTERFACE_LIBRARIES ""
#    DEFINE_SYMBOL ATTICA_LIB_MAKEDLL
#)

target_link_libraries(attica ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY} ws2_32)
set(ATTICA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/ CACHE PATH "include directories" FORCE)
