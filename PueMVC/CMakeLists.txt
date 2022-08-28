# Helper to create filters in VS
MACRO(SOURCE_GROUP_BY_DIR source_files)
    IF(MSVC)
        SET(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
        FOREACH(sgbd_file ${${source_files}})
            STRING(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
            STRING(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            STRING(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            STRING(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            IF(sgbd_nogroup)
                set(sgbd_group_name "\\")
            ENDIF(sgbd_nogroup)
            SOURCE_GROUP(${sgbd_group_name} FILES ${sgbd_file})
        ENDFOREACH(sgbd_file)
    ENDIF(MSVC)
ENDMACRO(SOURCE_GROUP_BY_DIR)
 
# PureMVC
SET(LIB_NAME PureMVC)
 
# The include/PureMVC directory
SET(PureMVC_INCLUDE_DIR
	"include/PureMVC"
)
 
# The src/PureMVC directory
SET(PureMVC_SRC_DIR
	"src/PureMVC"
)
 
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
 
LINK_DIRECTORIES(
	${CMAKE_LIBRARY_PATH}
	${PROJECT_BINARY_DIR}/lib
)
 
SET(PureMVC_LIBS
)
 
# Core
SET(PureMVC_Core_SRCS
	${PureMVC_INCLUDE_DIR}/Core/Controller.hpp
	${PureMVC_INCLUDE_DIR}/Core/Model.hpp
	${PureMVC_INCLUDE_DIR}/Core/View.hpp
	${PureMVC_SRC_DIR}/Core/Controller.cpp
	${PureMVC_SRC_DIR}/Core/Model.cpp
	${PureMVC_SRC_DIR}/Core/View.cpp		
)
 
# Interfaces
SET(PureMVC_Interfaces_SRCS
	${PureMVC_INCLUDE_DIR}/Interfaces/IAggregate.hpp
	${PureMVC_INCLUDE_DIR}/Interfaces/ICommand.hpp
	${PureMVC_INCLUDE_DIR}/Interfaces/IController.hpp
	${PureMVC_INCLUDE_DIR}/Interfaces/IFacade.hpp
	${PureMVC_INCLUDE_DIR}/Interfaces/IIterator.hpp
	${PureMVC_INCLUDE_DIR}/Interfaces/IMediator.hpp
	${PureMVC_INCLUDE_DIR}/Interfaces/IModel.hpp
	${PureMVC_INCLUDE_DIR}/Interfaces/INotification.hpp	
	${PureMVC_INCLUDE_DIR}/Interfaces/INotifier.hpp
	${PureMVC_INCLUDE_DIR}/Interfaces/IObserver.hpp
	${PureMVC_INCLUDE_DIR}/Interfaces/IProxy.hpp	
	${PureMVC_INCLUDE_DIR}/Interfaces/IView.hpp		
)
 
# Patterns
SET(PureMVC_Patterns_Command_SRCS
	${PureMVC_INCLUDE_DIR}/Patterns/Command/DelegateCommand.hpp
	${PureMVC_INCLUDE_DIR}/Patterns/Command/MacroCommand.hpp
	${PureMVC_INCLUDE_DIR}/Patterns/Command/SimpleCommand.hpp
	${PureMVC_SRC_DIR}/Patterns/Command/MacroCommand.cpp
	${PureMVC_SRC_DIR}/Patterns/Command/SimpleCommand.cpp	
)
 
SET(PureMVC_Patterns_Facade_SRCS
	${PureMVC_INCLUDE_DIR}/Patterns/Facade/Facade.hpp
	${PureMVC_SRC_DIR}/Patterns/Facade/Facade.cpp	
)
 
SET(PureMVC_Patterns_Iterator_SRCS
	${PureMVC_INCLUDE_DIR}/Patterns/Iterator/Iterator.hpp	
)
 
SET(PureMVC_Patterns_Mediator_SRCS	
	${PureMVC_INCLUDE_DIR}/Patterns/Mediator/Mediator.hpp		
	${PureMVC_SRC_DIR}/Patterns/Mediator/Mediator.cpp
)
 
SET(PureMVC_Patterns_Observer_SRCS
	${PureMVC_INCLUDE_DIR}/Patterns/Observer/Notification.hpp
	${PureMVC_INCLUDE_DIR}/Patterns/Observer/Notifier.hpp	
	${PureMVC_INCLUDE_DIR}/Patterns/Observer/Observer.hpp
	${PureMVC_SRC_DIR}/Patterns/Observer/Notification.cpp	
	${PureMVC_SRC_DIR}/Patterns/Observer/Notifier.cpp
)
 
SET(PureMVC_Patterns_Proxy_SRCS
	${PureMVC_INCLUDE_DIR}/Patterns/Proxy/Proxy.hpp
	${PureMVC_SRC_DIR}/Patterns/Proxy/Proxy.cpp
)
 
SET(PureMVC_Patterns_SRCS
	${PureMVC_Patterns_Command_SRCS}	
	${PureMVC_Patterns_Facade_SRCS}
	${PureMVC_Patterns_Iterator_SRCS}
	${PureMVC_Patterns_Mediator_SRCS}
	${PureMVC_Patterns_Observer_SRCS}	
	${PureMVC_Patterns_Proxy_SRCS}
)
 
# Resource Files
SET(PureMVC_RES_SRCS
	${PureMVC_SRC_DIR}/Resource/PureMVC.rc
)
 
SET(PureMVC_SRCS
	${PureMVC_Core_SRCS}
	${PureMVC_Interfaces_SRCS}
	${PureMVC_Patterns_SRCS}
	${PureMVC_RES_SRCS}
	${PureMVC_INCLUDE_DIR}/PureMVC.hpp
	${PureMVC_SRC_DIR}/PureMVC.cpp
	${PureMVC_SRC_DIR}/Common.hpp
)
 
source_group_by_dir(PureMVC_SRCS)
 
 
IF (MSVC)
    # Turn off deprecation warnings
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_WARNINGS)
	# Export PureMVC class symbols
	ADD_DEFINITIONS(-DPUREMVC_API_EXPORT)
ENDIF(MSVC)
 
ADD_LIBRARY(${LIB_NAME} STATIC ${PureMVC_SRCS})
TARGET_LINK_LIBRARIES(${LIB_NAME} ${PureMVC_LIBS})
 
IF (MSVC)
	# Runtime Library: -DMDd, -DMD
	ADD_LIBRARY(${LIB_NAME}dll SHARED ${PureMVC_SRCS})
	TARGET_LINK_LIBRARIES(${LIB_NAME}dll ${PureMVC_LIBS})
 
	SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_OUTPUT_NAME PureMVCd.dll  RELEASE_OUTPUT_NAME PureMVC.dll)
	SET_TARGET_PROPERTIES(${LIB_NAME}dll PROPERTIES DEBUG_OUTPUT_NAME PureMVCddll.dll  RELEASE_OUTPUT_NAME PureMVCdll.dll)
ENDIF(MSVC)
 
# Install
SET(INSTALL_INCDIR include)
SET(INSTALL_BINDIR bin)
IF(WIN32)
    SET(INSTALL_LIBDIR bin)
    SET(INSTALL_ARCHIVEDIR lib)
ELSE(WIN32)
    SET(INSTALL_LIBDIR lib${LIB_POSTFIX})
    SET(INSTALL_ARCHIVEDIR lib${LIB_POSTFIX})
ENDIF(WIN32)
 
INSTALL(
    TARGETS ${LIB_NAME}
    RUNTIME DESTINATION ${INSTALL_BINDIR}
    LIBRARY DESTINATION ${INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${INSTALL_ARCHIVEDIR}
)
 
INSTALL(
    FILES       ${HEADERS}
    DESTINATION ${INSTALL_INCDIR}/${LIB_NAME}
)