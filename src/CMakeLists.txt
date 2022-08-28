file(GLOB_RECURSE LUA_BRIDGE_INCLUDE_DIR "../include/LuaBridge/*.h")
file(GLOB_RECURSE LUA_BRIDGE_SRC_DIR "LuaBridge/*.cpp")

file(GLOB_RECURSE LUA_RECAST_NAVIGATION_INCLUDE_DIR "../include/Library/RecastNavigation/*/*.h")
file(GLOB_RECURSE LUA_RECAST_NAVIGATION_SRC_DIR "Library/RecastNavigation/*/*.cpp")

file(GLOB_RECURSE NAVMESH_INCLUDE_DIR "../include/Navmesh/*.h")
file(GLOB_RECURSE NAVMESH_SRC_DIR "Navmesh/*.cpp")


file(GLOB_RECURSE MANAGER_INCLUDE_DIR "../include/Manager/*.h")
file(GLOB_RECURSE MANAGER_SRC_DIR "Manager/*.cpp")

#MVCÄ£¿é
set(MVC_INCLUDE_PATH "../include/Library/PureMVC")
set(MVC_SRC_PATH "Library/PureMVC")
file(GLOB_RECURSE PURE_MVC_INCLUDE_DIR ${MVC_INCLUDE_PATH}/*.hpp ${MVC_INCLUDE_PATH}/Core/*.hpp  ${MVC_INCLUDE_PATH}/Interfaces/*.hpp ${MVC_INCLUDE_PATH}/Patterns/*/*.hpp)
file(GLOB_RECURSE PURE_MVC_SRC_DIR ${MVC_SRC_PATH}/*.cpp ${MVC_SRC_PATH}/*.hpp  ${MVC_SRC_PATH}/Core/*.cpp ${MVC_SRC_PATH}/Patterns/*/*.cpp ) 


set(MVC_LOGIC_INCLUDE_PATH "../include/Logic")
set(MVC_LOGIC_SRC_PATH "Logic")
file(GLOB_RECURSE MVC_LOGIC_INCLUDE_DIR ${MVC_LOGIC_INCLUDE_PATH}/*.hpp ${MVC_LOGIC_INCLUDE_PATH}/*/*.hpp  ${MVC_LOGIC_INCLUDE_PATH}/*/*/*.hpp)
file(GLOB_RECURSE MVC_LOGIC_SRC_DIR ${MVC_LOGIC_SRC_PATH}/*.cpp ${MVC_LOGIC_SRC_PATH}/*/*.cpp  ${MVC_LOGIC_SRC_PATH}/*/*/*.cpp) 


set(BOX2D_INCLUDE_PATH "../include/Box2d")
set(BOX2D_SRC_PATH "Box2d")
file(GLOB_RECURSE BOX2D_INCLUDE_DIR ${BOX2D_INCLUDE_PATH}/*.hpp)
file(GLOB_RECURSE BOX2D_DIR ${BOX2D_SRC_PATH}/*/*.cpp ) 

set(BOX2D_SOURCE_FILES 
	Library/xml/tinyxml2.cpp

	Library/lua/lapi.c
	Library/lua/lauxlib.c
	Library/lua/lbaselib.c
	Library/lua/lcode.c
	Library/lua/lcorolib.c
	Library/lua/lctype.c
	Library/lua/ldblib.c
	Library/lua/ldebug.c
	Library/lua/ldo.c
	Library/lua/ldump.c
	Library/lua/lfunc.c
	Library/lua/lgc.c
	Library/lua/linit.c
	Library/lua/liolib.c
	Library/lua/llex.c
	Library/lua/lmathlib.c
	Library/lua/lmem.c
	Library/lua/loadlib.c
	Library/lua/lobject.c
	Library/lua/lopcodes.c
	Library/lua/loslib.c
	Library/lua/lparser.c
	Library/lua/lstate.c
	Library/lua/lstring.c
	Library/lua/lstrlib.c
	Library/lua/ltable.c
	Library/lua/ltablib.c
	Library/lua/ltm.c
	Library/lua/lua.c
	Library/lua/luac.c
	Library/lua/lundump.c
	Library/lua/lutf8lib.c
	Library/lua/lvm.c
	Library/lua/lzio.c  
	 
	Library/Delabella/delabella.cpp

	Library/BehaviorTree/controls/fallback_node.cpp
	Library/BehaviorTree/controls/if_then_else_node.cpp
	Library/BehaviorTree/controls/parallel_node.cpp
	Library/BehaviorTree/controls/reactive_fallback.cpp
	Library/BehaviorTree/controls/reactive_sequence.cpp
	Library/BehaviorTree/controls/sequence_node.cpp
	Library/BehaviorTree/controls/sequence_star_node.cpp
	Library/BehaviorTree/controls/switch_node.cpp
	Library/BehaviorTree/controls/while_do_else_node.cpp 
	Library/BehaviorTree/decorators/delay_node.cpp 
	Library/BehaviorTree/decorators/inverter_node.cpp 
	Library/BehaviorTree/decorators/repeat_node.cpp
	Library/BehaviorTree/decorators/retry_node.cpp
	Library/BehaviorTree/decorators/subtree_node.cpp  
	Library/BehaviorTree/loggers/bt_cout_logger.cpp
	Library/BehaviorTree/loggers/bt_file_logger.cpp
	Library/BehaviorTree/loggers/bt_minitrace_logger.cpp
	Library/BehaviorTree/action_node.cpp
	Library/BehaviorTree/basic_types.cpp
	Library/BehaviorTree/behavior_tree.cpp
	Library/BehaviorTree/blackboard.cpp
	Library/BehaviorTree/bt_factory.cpp 
	Library/BehaviorTree/condition_node.cpp
	Library/BehaviorTree/control_node.cpp
	Library/BehaviorTree/decorator_node.cpp
	Library/BehaviorTree/shared_library.cpp
	Library/BehaviorTree/shared_library_WIN.cpp
	Library/BehaviorTree/tree_node.cpp
	Library/BehaviorTree/xml_parsing.cpp 
 	
	Manager/Base/BodyData.cpp   
	
	Library/fastlz/fastlz.c
	  
	${LUA_BRIDGE_SRC_DIR}
	${LUA_RECAST_NAVIGATION_SRC_DIR}
	${NAVMESH_SRC_DIR}
	${MANAGER_SRC_DIR}
	${PURE_MVC_SRC_DIR}
	${MVC_LOGIC_SRC_DIR}	  
	${BOX2D_DIR}	   
	
	PySourceParse/PhysicsParse.cpp
	)

set(BOX2D_HEADER_FILES 
	../include/Library/BehaviorTree/actions/always_failure_node.h
	../include/Library/BehaviorTree/actions/always_success_node.h
	../include/Library/BehaviorTree/actions/pop_from_queue.hpp
	../include/Library/BehaviorTree/actions/set_blackboard_node.h
	../include/Library/BehaviorTree/controls/fallback_node.h
	../include/Library/BehaviorTree/controls/if_then_else_node.h
	../include/Library/BehaviorTree/controls/parallel_node.h
	../include/Library/BehaviorTree/controls/reactive_fallback.h
	../include/Library/BehaviorTree/controls/reactive_sequence.h
	../include/Library/BehaviorTree/controls/sequence_node.h
	../include/Library/BehaviorTree/controls/sequence_star_node.h
	../include/Library/BehaviorTree/controls/switch_node.h
	../include/Library/BehaviorTree/controls/while_do_else_node.h
	../include/Library/BehaviorTree/decorators/blackboard_precondition.h
	../include/Library/BehaviorTree/decorators/consume_queue.h
	../include/Library/BehaviorTree/decorators/delay_node.h
	../include/Library/BehaviorTree/decorators/force_failure_node.h
	../include/Library/BehaviorTree/decorators/force_success_node.h
	../include/Library/BehaviorTree/decorators/inverter_node.h
	../include/Library/BehaviorTree/decorators/keep_running_until_failure_node.h
	../include/Library/BehaviorTree/decorators/repeat_node.h
	../include/Library/BehaviorTree/decorators/retry_node.h
	../include/Library/BehaviorTree/decorators/subtree_node.h
	../include/Library/BehaviorTree/decorators/timeout_node.h
	../include/Library/BehaviorTree/decorators/timer_queue.h
	../include/Library/BehaviorTree/flatbuffers/base.h
	../include/Library/BehaviorTree/flatbuffers/bt_flatbuffer_helper.h
	../include/Library/BehaviorTree/flatbuffers/BT_logger_generated.h
	../include/Library/BehaviorTree/flatbuffers/flatbuffers.h
	../include/Library/BehaviorTree/flatbuffers/stl_emulation.h
	../include/Library/BehaviorTree/loggers/abstract_logger.h
	../include/Library/BehaviorTree/loggers/bt_cout_logger.h
	../include/Library/BehaviorTree/loggers/bt_file_logger.h
	../include/Library/BehaviorTree/loggers/bt_minitrace_logger.h
	../include/Library/BehaviorTree/loggers/zmq.hpp
	../include/Library/BehaviorTree/utils/any.hpp
	../include/Library/BehaviorTree/utils/convert_impl.hpp
	../include/Library/BehaviorTree/utils/demangle_util.h
	../include/Library/BehaviorTree/utils/expected.hpp
	../include/Library/BehaviorTree/utils/make_unique.hpp
	../include/Library/BehaviorTree/utils/platform.hpp
	../include/Library/BehaviorTree/utils/safe_any.hpp
	../include/Library/BehaviorTree/utils/shared_library.h
	../include/Library/BehaviorTree/utils/signal.h
	../include/Library/BehaviorTree/utils/simple_string.hpp
	../include/Library/BehaviorTree/utils/strcat.hpp
	../include/Library/BehaviorTree/utils/string_view.hpp
	../include/Library/BehaviorTree/utils/wakeup_signal.hpp
	../include/Library/BehaviorTree/action_node.h
	../include/Library/BehaviorTree/basic_types.h
	../include/Library/BehaviorTree/behavior_tree.h
	../include/Library/BehaviorTree/blackboard.h
	../include/Library/BehaviorTree/bt_factory.h
	../include/Library/BehaviorTree/bt_parser.h
	../include/Library/BehaviorTree/condition_node.h
	../include/Library/BehaviorTree/control_node.h
	../include/Library/BehaviorTree/decorator_node.h
	../include/Library/BehaviorTree/exceptions.h
	../include/Library/BehaviorTree/leaf_node.h
	../include/Library/BehaviorTree/tree_node.h
	../include/Library/BehaviorTree/xml_parsing.h
	
	../include/Library/Delabella/delabella.h
	../include/Library/Delabella/predicates.h
	
	 
	../include/Library/xml/tinyxml2.h

	../include/Library/lua/lapi.h
	../include/Library/lua/lauxlib.h
	../include/Library/lua/lcode.h
	../include/Library/lua/lctype.h
	../include/Library/lua/ldebug.h
	../include/Library/lua/ldo.h
	../include/Library/lua/lfunc.h
	../include/Library/lua/lgc.h
	../include/Library/lua/ljumptab.h
	../include/Library/lua/llex.h
	../include/Library/lua/llimits.h
	../include/Library/lua/lmem.h
	../include/Library/lua/lobject.h
	../include/Library/lua/lopcodes.h
	../include/Library/lua/lopnames.h
	../include/Library/lua/lparser.h
	../include/Library/lua/lprefix.h
	../include/Library/lua/lstate.h
	../include/Library/lua/lstring.h
	../include/Library/lua/ltable.h
	../include/Library/lua/ltm.h
	../include/Library/lua/lua.h
	../include/Library/lua/lua.hpp
	../include/Library/lua/luaconf.h
	../include/Library/lua/lualib.h
	../include/Library/lua/lundump.h
	../include/Library/lua/lvm.h
	../include/Library/lua/lzio.h 

	
	../include/PySourceParse/ShapeBase.h
	../include/PySourceParse/PolygonShapeBase.h
	../include/PySourceParse/CircleShapeBase.h
	../include/PySourceParse/FixtureBase.h
	../include/PySourceParse/PhysicsParse.h

	../include/Manager/Base/BodyData.h  

	../include/Library/fastlz/fastlz.h 
	
	../include/Library/TimeWheel/timer-wheel.h
	
	${LUA_BRIDGE_INCLUDE_DIR}	
	${LUA_RECAST_NAVIGATION_INCLUDE_DIR} 
	${NAVMESH_INCLUDE_DIR}  
	${MANAGER_INCLUDE_DIR}     
	${PURE_MVC_INCLUDE_DIR}   
	${MVC_LOGIC_INCLUDE_DIR}   
	${BOX2D_INCLUDE_DIR}   
	  
	) 
add_library(box2d ${BOX2D_SOURCE_FILES} ${BOX2D_HEADER_FILES})
target_include_directories(box2d
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../PueMVC/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/Library/RecastNavigation>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(box2d PROPERTIES
	CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

if(MSVC)
  set_target_properties(box2d PROPERTIES
    COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMPILE_PDB_NAME box2d
  )

  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/box2d.pdb"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    OPTIONAL
  )
endif()

if (BUILD_SHARED_LIBS)
  target_compile_definitions(box2d
    PUBLIC
      B2_SHARED
  )
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "src" FILES ${BOX2D_SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../include" PREFIX "include" FILES ${BOX2D_HEADER_FILES})

install(
  TARGETS box2d
  EXPORT box2dConfig
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
  EXPORT box2dConfig
  NAMESPACE box2d::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/box2d"
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/box2dConfigVersion.cmake"
  COMPATIBILITY SameMajorVersion
)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/box2dConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/box2d"
)
