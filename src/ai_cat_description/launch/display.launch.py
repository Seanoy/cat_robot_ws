import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import Command, LaunchConfiguration, PathJoinSubstitution

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    # ------------------------------
    # Launch arguments
    # ------------------------------
    use_gui = LaunchConfiguration('use_gui', default='true')

    # ------------------------------
    # Paths
    # ------------------------------
    pkg_share = FindPackageShare('ai_cat_description').find('ai_cat_description')
    xacro_path = PathJoinSubstitution([pkg_share, 'urdf', 'ai_cat.xacro'])

    # Generate robot_description by processing xacro
    robot_description_content = Command([
        'xacro ',
        xacro_path
    ])

    robot_description = {'robot_description': robot_description_content}

    # ------------------------------
    # Nodes
    # ------------------------------
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[robot_description]
    )

    joint_state_publisher_gui_node = Node(
        package='joint_state_publisher_gui',
        executable='joint_state_publisher_gui',
        name='joint_state_publisher_gui',
        condition=IfCondition(use_gui)
    )

    rviz_config = PathJoinSubstitution([
        pkg_share,
        'rviz',           # assuming you have a rviz config folder/file
        'cat_view.rviz'
    ])

    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        arguments=['-d', rviz_config]
    )

    return LaunchDescription([
        DeclareLaunchArgument(
            'use_gui',
            default_value='true',
            description='Use joint state publisher GUI'
        ),

        robot_state_publisher_node,
        joint_state_publisher_gui_node,
        rviz_node,
    ])