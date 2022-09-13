file(GLOB_RECURSE LUA_BRIDGE_INCLUDE_DIR "../include/LuaBridge/*.h")
file(GLOB_RECURSE LUA_BRIDGE_SRC_DIR "LuaBridge/*.cpp")

file(GLOB_RECURSE LUA_RECAST_NAVIGATION_INCLUDE_DIR "../include/Library/RecastNavigation/*/*.h")
file(GLOB_RECURSE LUA_RECAST_NAVIGATION_SRC_DIR "Library/RecastNavigation/*/*.cpp")

file(GLOB_RECURSE NAVMESH_INCLUDE_DIR "../include/Navmesh/*.h")
file(GLOB_RECURSE NAVMESH_SRC_DIR "NAVMESH/*.cpp")


file(GLOB_RECURSE MANAGER_INCLUDE_DIR "../include/Manager/*.h")
file(GLOB_RECURSE MANAGER_SRC_DIR "Manager/*.cpp")

set(BOX2D_SOURCE_FILES
	Box2d/collision/b2_broad_phase.cpp
	Box2d/collision/b2_chain_shape.cpp
	Box2d/collision/b2_circle_shape.cpp
	Box2d/collision/b2_collide_circle.cpp
	Box2d/collision/b2_collide_edge.cpp
	Box2d/collision/b2_collide_polygon.cpp
	Box2d/collision/b2_collision.cpp
	Box2d/collision/b2_distance.cpp
	Box2d/collision/b2_dynamic_tree.cpp
	Box2d/collision/b2_edge_shape.cpp
	Box2d/collision/b2_polygon_shape.cpp
	Box2d/collision/b2_time_of_impact.cpp
	Box2d/common/b2_block_allocator.cpp
	Box2d/common/b2_draw.cpp
	Box2d/common/b2_math.cpp
	Box2d/common/b2_settings.cpp
	Box2d/common/b2_stack_allocator.cpp
	Box2d/common/b2_timer.cpp
	Box2d/dynamics/b2_body.cpp
	Box2d/dynamics/b2_chain_circle_contact.cpp
	Box2d/dynamics/b2_chain_circle_contact.h
	Box2d/dynamics/b2_chain_polygon_contact.cpp
	Box2d/dynamics/b2_chain_polygon_contact.h
	Box2d/dynamics/b2_circle_contact.cpp
	Box2d/dynamics/b2_circle_contact.h
	Box2d/dynamics/b2_contact.cpp
	Box2d/dynamics/b2_contact_manager.cpp
	Box2d/dynamics/b2_contact_solver.cpp
	Box2d/dynamics/b2_contact_solver.h
	Box2d/dynamics/b2_distance_joint.cpp
	Box2d/dynamics/b2_edge_circle_contact.cpp
	Box2d/dynamics/b2_edge_circle_contact.h
	Box2d/dynamics/b2_edge_polygon_contact.cpp
	Box2d/dynamics/b2_edge_polygon_contact.h
	Box2d/dynamics/b2_fixture.cpp
	Box2d/dynamics/b2_friction_joint.cpp
	Box2d/dynamics/b2_gear_joint.cpp
	Box2d/dynamics/b2_island.cpp
	Box2d/dynamics/b2_island.h
	Box2d/dynamics/b2_joint.cpp
	Box2d/dynamics/b2_motor_joint.cpp
	Box2d/dynamics/b2_mouse_joint.cpp
	Box2d/dynamics/b2_polygon_circle_contact.cpp
	Box2d/dynamics/b2_polygon_circle_contact.h
	Box2d/dynamics/b2_polygon_contact.cpp
	Box2d/dynamics/b2_polygon_contact.h
	Box2d/dynamics/b2_prismatic_joint.cpp
	Box2d/dynamics/b2_pulley_joint.cpp
	Box2d/dynamics/b2_revolute_joint.cpp
	Box2d/dynamics/b2_weld_joint.cpp
	Box2d/dynamics/b2_wheel_joint.cpp
	Box2d/dynamics/b2_world.cpp
	Box2d/dynamics/b2_world_callbacks.cpp  
	
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
	  
	PySourceParse/PhysicsParse.cpp
	)

set(BOX2D_HEADER_FILES
	../include/Box2d/b2_api.h
	../include/Box2d/b2_block_allocator.h
	../include/Box2d/b2_body.h
	../include/Box2d/b2_broad_phase.h
	../include/Box2d/b2_chain_shape.h
	../include/Box2d/b2_circle_shape.h
	../include/Box2d/b2_collision.h
	../include/Box2d/b2_common.h
	../include/Box2d/b2_contact.h
	../include/Box2d/b2_contact_manager.h
	../include/Box2d/b2_distance.h
	../include/Box2d/b2_distance_joint.h
	../include/Box2d/b2_draw.h
	../include/Box2d/b2_dynamic_tree.h
	../include/Box2d/b2_edge_shape.h
	../include/Box2d/b2_fixture.h
	../include/Box2d/b2_friction_joint.h
	../include/Box2d/b2_gear_joint.h
	../include/Box2d/b2_growable_stack.h
	../include/Box2d/b2_joint.h
	../include/Box2d/b2_math.h
	../include/Box2d/b2_motor_joint.h
	../include/Box2d/b2_mouse_joint.h
	../include/Box2d/b2_polygon_shape.h
	../include/Box2d/b2_prismatic_joint.h
	../include/Box2d/b2_pulley_joint.h
	../include/Box2d/b2_revolute_joint.h
	../include/Box2d/b2_rope.h
	../include/Box2d/b2_settings.h
	../include/Box2d/b2_shape.h
	../include/Box2d/b2_stack_allocator.h
	../include/Box2d/b2_time_of_impact.h
	../include/Box2d/b2_timer.h
	../include/Box2d/b2_time_step.h
	../include/Box2d/b2_types.h
	../include/Box2d/b2_weld_joint.h
	../include/Box2d/b2_wheel_joint.h
	../include/Box2d/b2_world.h
	../include/Box2d/b2_world_callbacks.h 
	 
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

	../include/Library/JumpList/JumpList.h 

	
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
	) 
add_library(box2d ${BOX2D_SOURCE_FILES} ${BOX2D_HEADER_FILES})
target_include_directories(box2d
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
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
