# Run go2 Example

### 1. Build workspaces
Compile unitree-ros2
```bash
cd ~/unitree_ros2
source ~/unitree_ros2/setup_default.sh # This sources ros2 with no network interface specified (needed for simulation)
colcon build
```

Compile cyclone-dds
```bash
cd ~/unitree_ros2/cyclonedds_ws
colcon build
```

### 2. Open go2 usd in isaac sim
```
unitree_ros2/example/src/src/go2/isaacsim_usd/go2.usd
```
Click play. The action graph in go2.usd should spin up the /joint_command and /joint_states topics.

### 3. Run Examples
In terminal 1:
```bash
cd ~/unitree_ros2/examples
source ~/unitree_ros2/setup_default.sh
./install/unitree_ros2_example/bridge_node
```
This launches the bridge node that maps the motor commands that the physical go2 would recieve to joint commands that the go2 in isaacsim expects.

In terminal 2:
```bash
cd ~/unitree_ros2/examples
./install/unitree_ros2_example/bin/go2_stand_example
```
This runs the go2 stand example. You should now see the go2 in isaacsim moving.
