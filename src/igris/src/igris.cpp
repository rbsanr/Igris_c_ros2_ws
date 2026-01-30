#include <atomic>
#include <chrono>
#include <csignal>
#include <deque>
#include <future>
#include <igris_sdk/channel_factory.hpp>
#include <igris_sdk/igris_c_client.hpp>
#include <igris_sdk/publisher.hpp>
#include <igris_sdk/subscriber.hpp>
#include <iostream>
#include <mutex>
#include <thread>
#include <iomanip>
#include <cmath>

// ROS2 includes
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>

#define NUM_MOTORS 31

using namespace igris_sdk;
using namespace igris_c::msg::dds;

// Motor and Joint names
static const std::array<const char *, 31> MOTOR_NAMES = {
    "Waist_Yaw",    "Waist_L",          "Waist_R",         "Hip_Pitch_L",    "Hip_Roll_L",    "Hip_Yaw_L",    "Knee_Pitch_L",
    "Ankle_Out_L",  "Ankle_In_L",       "Hip_Pitch_R",     "Hip_Roll_R",     "Hip_Yaw_R",     "Knee_Pitch_R", "Ankle_Out_R",
    "Ankle_In_R",   "Shoulder_Pitch_L", "Shoulder_Roll_L", "Shoulder_Yaw_L", "Elbow_Pitch_L", "Wrist_Yaw_L",  "Wrist_Front_L",
    "Wrist_Back_L", "Shoulder_Pitch_R", "Shoulder_Roll_R", "Shoulder_Yaw_R", "Elbow_Pitch_R", "Wrist_Yaw_R",  "Wrist_Front_R",
    "Wrist_Back_R", "Neck_Yaw",         "Neck_Pitch"};

static const std::array<const char *, 31> JOINT_NAMES = {
    "Waist_Yaw",     "Waist_Roll",       "Waist_Pitch",     "Hip_Pitch_L",    "Hip_Roll_L",    "Hip_Yaw_L",    "Knee_Pitch_L",
    "Ankle_Pitch_L", "Ankle_Roll_L",     "Hip_Pitch_R",     "Hip_Roll_R",     "Hip_Yaw_R",     "Knee_Pitch_R", "Ankle_Pitch_R",
    "Ankle_Roll_R",  "Shoulder_Pitch_L", "Shoulder_Roll_L", "Shoulder_Yaw_L", "Elbow_Pitch_L", "Wrist_Yaw_L",  "Wrist_Roll_L",
    "Wrist_Pitch_L", "Shoulder_Pitch_R", "Shoulder_Roll_R", "Shoulder_Yaw_R", "Elbow_Pitch_R", "Wrist_Yaw_R",  "Wrist_Roll_R",
    "Wrist_Pitch_R", "Neck_Yaw",         "Neck_Pitch"};

// Joint position limits
static const std::array<float, 31> JOINT_POS_MAX = {
    1.57f, 0.310f, 0.28f,
    0.480f, 2.300f, 1.570f, 2.280f, 0.698f, 0.349f,
    0.480f, 0.330f, 1.570f, 2.280f, 0.698f, 0.349f,
    1.047f, 3.140f, 1.570f, 0.0f, 1.570f, 0.870f, 0.650f,
    1.047f, 0.170f, 1.570f, 0.0f, 1.570f, 1.221f, 0.650f,
    1.221f, 0.520f
};

static const std::array<float, 31> JOINT_POS_MIN = {
    -1.57f, -0.310f, -0.87f,
    -2.000f, -0.330f, -1.570f, 0.0f, -0.70f, -0.350f,
    -2.000f, -2.300f, -1.570f, 0.0f, -0.70f, -0.350f,
    -3.141f, -0.170f, -1.570f, -2.3f, -1.570f, -1.221f, -0.650f,
    -3.141f, -3.140f, -1.570f, -2.3f, -1.570f, -0.870f, -0.650f,
    -1.221f, -0.520f
};

// Motor position limits
static const std::array<float, 31> MOTOR_POS_MAX = {
    1.57f, 0.87f, 0.87f,
    0.480f, 2.300f, 1.570f, 2.280f, 0.609f, 0.523f,
    0.480f, 0.330f, 1.570f, 2.280f, 0.609f, 0.523f,
    1.047f, 3.140f, 1.570f, 0.0f, 1.570f, 0.75f, 0.75f,
    1.047f, 0.170f, 1.570f, 0.0f, 1.570f, 0.98f, 0.98f,
    1.221f, 0.520f
};

static const std::array<float, 31> MOTOR_POS_MIN = {
    -1.57f, -0.34f, -0.34f,
    -2.000f, -0.330f, -1.570f, 0.0f, -0.630f, -0.617f,
    -2.000f, -2.300f, -1.570f, 0.0f, -0.630f, -0.617f,
    -3.141f, -0.170f, -1.570f, -2.3f, -1.570f, -0.98f, -0.98f,
    -3.141f, -3.140f, -1.570f, -2.3f, -1.570f, -0.75f, -0.75f,
    -1.221f, -0.520f
};

// PD gains
// static const std::array<float, NUM_MOTORS> kp = {
//     50.0, 25.0, 25.0,
//     500.0, 200.0, 50.0, 500.0, 300.0, 300.0,
//     500.0, 200.0, 50.0, 500.0, 300.0, 300.0,
//     50.0, 50.0, 30.0, 30.0, 5.0, 5.0, 5.0,
//     50.0, 50.0, 30.0, 30.0, 5.0, 5.0, 5.0,
//     2.0, 5.0
// };

// PD gains * 3
static const std::array<float, NUM_MOTORS> kp = {
    150.0, 75.0, 75.0,
    1500.0, 600.0, 150.0, 1500.0, 900.0, 900.0,
    1500.0, 600.0, 150.0, 1500.0, 900.0, 900.0,
    150.0, 150.0, 90.0, 90.0, 15.0, 15.0, 15.0,
    150.0, 150.0, 90.0, 90.0, 15.0, 15.0, 15.0,
    6.0, 15.0
};

static const std::array<float, NUM_MOTORS> kd = {
    0.8, 0.8, 0.8,
    3.0, 0.5, 0.5, 3.0, 1.5, 1.5,
    3.0, 0.5, 0.5, 3.0, 1.5, 1.5,
    0.5, 0.5, 0.15, 0.15, 0.1, 0.1, 0.1,
    0.5, 0.5, 0.15, 0.15, 0.1, 0.1, 0.1,
    0.05, 0.1
};

static std::atomic<bool> g_running(true);

void SignalHandler(int) {
    g_running = false;
    std::cout << "\n[Signal] Shutdown requested..." << std::endl;
    rclcpp::shutdown();
}

class IgrisController {
private:
    int domain_id;
    IgrisC_Client client;
    
    std::shared_ptr<Publisher<LowCmd>> cmd_pub;
    std::shared_ptr<Subscriber<LowState>> state_sub;
    std::shared_ptr<Subscriber<ControlModeState>> controlmode_sub;

    LowCmd cmd;
    LowState current_state;
    ControlModeState current_controlmode;

    std::atomic<bool> state_received{false};
    std::atomic<bool> first_state_received{false};
    std::atomic<uint32_t> state_count{0};
    std::atomic<uint32_t> cmd_publish_count{0};
    
    std::array<float, NUM_MOTORS> current_joint_pos{};
    std::array<float, NUM_MOTORS> current_motor_pos{};
    std::array<float, NUM_MOTORS> initial_joint_pos{};
    std::array<float, NUM_MOTORS> initial_motor_pos{};
    std::array<float, NUM_MOTORS> target_joint_pos{};
    std::array<float, NUM_MOTORS> target_motor_pos{};
    
    // ===== 추가: 이전 값 저장 및 필터링 =====
    std::array<float, NUM_MOTORS> previous_joint_pos{};
    std::array<float, NUM_MOTORS> previous_motor_pos{};
    const float MAX_POSITION_CHANGE = 0.5f;
    std::atomic<uint32_t> filter_count{0};
    
    std::atomic<bool> lowlevel_active{false};
    std::atomic<bool> use_joint_mode{true};
    
    std::mutex state_mutex;
    std::mutex target_mutex;
    std::mutex controlmode_mutex;
    
    std::thread control_thread;
    std::atomic<bool> control_thread_running{false};

    struct TrajectoryState {
        bool active = false;
        std::chrono::steady_clock::time_point start_time;
        double duration_sec = 0.0;
        std::array<float, NUM_MOTORS> start_positions{};
        std::array<float, NUM_MOTORS> end_positions{};
    };
    
    TrajectoryState trajectory_state_;
    std::mutex trajectory_mutex_;

    float clamp(float value, float min_val, float max_val) {
        if (value < min_val) return min_val;
        if (value > max_val) return max_val;
        return value;
    }

    float CubicInterpolate(float start, float end, float t) {
        float delta = end - start;
        float t2 = t * t;
        float t3 = t2 * t;
        return start + delta * (3.0f * t2 - 2.0f * t3);
    }

    void UpdateTrajectory(std::array<float, NUM_MOTORS>& target_positions) {
        std::lock_guard<std::mutex> lock(trajectory_mutex_);
        
        if (!trajectory_state_.active) {
            return;
        }
        
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - trajectory_state_.start_time).count();
        
        if (elapsed >= trajectory_state_.duration_sec) {
            target_positions = trajectory_state_.end_positions;
            trajectory_state_.active = false;
            std::cout << "[Trajectory] Motion completed" << std::endl;
        } else {
            float t = static_cast<float>(elapsed / trajectory_state_.duration_sec);
            
            for (int i = 0; i < NUM_MOTORS; i++) {
                target_positions[i] = CubicInterpolate(
                    trajectory_state_.start_positions[i],
                    trajectory_state_.end_positions[i],
                    t
                );
            }
        }
    }

    void PrintResult(const std::string &action, const ServiceResponse &res) {
        std::cout << "\n[" << action << "] " << (res.success() ? "SUCCESS" : "FAILED") 
                  << " - " << res.message() << std::endl;
    }

    void StateCallback(const LowState &state) {
        std::lock_guard<std::mutex> lock(state_mutex);
        current_state = state;
        state_count++;
        
        bool filtered = false;
        
        for (size_t i = 0; i < NUM_MOTORS; i++) {
            // Motor position 처리
            if (i < state.motor_state().size()) {
                float new_pos = state.motor_state()[i].q();
                
                // 범위 체크 + NaN/Inf 체크
                if (std::isfinite(new_pos) && 
                    new_pos >= MOTOR_POS_MIN[i] - 0.5f && 
                    new_pos <= MOTOR_POS_MAX[i] + 0.5f) {
                    
                    // 급격한 변화 체크 (첫 수신 시 제외)
                    if (!first_state_received || 
                        std::abs(new_pos - previous_motor_pos[i]) <= MAX_POSITION_CHANGE) {
                        current_motor_pos[i] = new_pos;
                        previous_motor_pos[i] = new_pos;
                    } else {
                        current_motor_pos[i] = previous_motor_pos[i];
                        filtered = true;
                    }
                } else {
                    if (first_state_received) {
                        current_motor_pos[i] = previous_motor_pos[i];
                        filtered = true;
                    } else {
                        current_motor_pos[i] = 0.0f;
                        previous_motor_pos[i] = 0.0f;
                    }
                }
            }
            
            // Joint position 처리
            if (i < state.joint_state().size()) {
                float new_pos = state.joint_state()[i].q();
                
                // 범위 체크 + NaN/Inf 체크
                if (std::isfinite(new_pos) && 
                    new_pos >= JOINT_POS_MIN[i] - 0.5f && 
                    new_pos <= JOINT_POS_MAX[i] + 0.5f) {
                    
                    // 급격한 변화 체크 (첫 수신 시 제외)
                    if (!first_state_received || 
                        std::abs(new_pos - previous_joint_pos[i]) <= MAX_POSITION_CHANGE) {
                        current_joint_pos[i] = new_pos;
                        previous_joint_pos[i] = new_pos;
                    } else {
                        current_joint_pos[i] = previous_joint_pos[i];
                        filtered = true;
                    }
                } else {
                    if (first_state_received) {
                        current_joint_pos[i] = previous_joint_pos[i];
                        filtered = true;
                    } else {
                        current_joint_pos[i] = 0.0f;
                        previous_joint_pos[i] = 0.0f;
                    }
                }
            }
        }
        
        // 필터링 카운트
        if (filtered) {
            filter_count++;
            if (filter_count % 100 == 0) {
                std::cout << "[Warning] Filtered " << filter_count.load() 
                          << " abnormal position values" << std::endl;
            }
        }
        
        if (!first_state_received) {
            for (int i = 0; i < NUM_MOTORS; i++) {
                initial_motor_pos[i] = current_motor_pos[i];
                initial_joint_pos[i] = current_joint_pos[i];
                target_motor_pos[i] = current_motor_pos[i];
                target_joint_pos[i] = current_joint_pos[i];
            }
            first_state_received = true;
            std::cout << "[Info] Initial positions saved (Motor & Joint)" << std::endl;
        }
        
        if (!state_received) {
            state_received = true;
            std::cout << "[Info] First robot state received!" << std::endl;
        }
    }

    void ControlModeCallback(const ControlModeState &mode_state) {
        std::lock_guard<std::mutex> lock(controlmode_mutex);
        current_controlmode = mode_state;
    }

    void ControlThreadFunction() {
        const auto period = std::chrono::microseconds(3333);
        auto next_time = std::chrono::steady_clock::now();

        std::cout << "[Control Thread] Started at 300Hz" << std::endl;

        while (control_thread_running && g_running) {
            if (lowlevel_active && first_state_received) {
                LowCmd local_cmd;
                
                {
                    std::lock_guard<std::mutex> lock(target_mutex);
                    
                    if (use_joint_mode) {
                        UpdateTrajectory(target_joint_pos);
                    } else {
                        UpdateTrajectory(target_motor_pos);
                    }
                    
                    local_cmd.kinematic_mode(use_joint_mode ? KinematicMode::PJS : KinematicMode::MS);
                    
                    for (int i = 0; i < NUM_MOTORS; i++) {
                        auto &m = local_cmd.motors()[i];
                        m.id(i);
                        
                        float q_target;
                        if (use_joint_mode) {
                            q_target = clamp(target_joint_pos[i], JOINT_POS_MIN[i], JOINT_POS_MAX[i]);
                        } else {
                            q_target = clamp(target_motor_pos[i], MOTOR_POS_MIN[i], MOTOR_POS_MAX[i]);
                        }
                        
                        m.q(q_target);
                        m.dq(0.0f);
                        m.tau(0.0f);
                        m.kp(kp[i]);
                        m.kd(kd[i]);
                    }
                }
                
                cmd_pub->write(local_cmd);
                cmd_publish_count++;
            }
            
            next_time += period;
            std::this_thread::sleep_until(next_time);
        }
        
        std::cout << "[Control Thread] Stopped" << std::endl;
    }

public:
    IgrisController(int id = 0) : domain_id(id) {
        signal(SIGINT, SignalHandler);
        signal(SIGTERM, SignalHandler);
        
        std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
        std::cout << "║   IGRIS Controller Initialization     ║" << std::endl;
        std::cout << "╚════════════════════════════════════════╝\n" << std::endl;

        std::cout << "[Init] Initializing ChannelFactory..." << std::endl;
        ChannelFactory::Instance()->Init(domain_id);
        if (!ChannelFactory::Instance()->IsInitialized()) {
            throw std::runtime_error("Failed to initialize ChannelFactory");
        }

        std::cout << "[Init] Initializing Service Client..." << std::endl;
        client.Init();
        client.SetTimeout(10.0f);

        std::cout << "[Init] Initializing LowCmd Publisher..." << std::endl;
        cmd_pub = std::make_shared<Publisher<LowCmd>>("rt/lowcmd");
        if (!cmd_pub->init()) {
            throw std::runtime_error("Failed to initialize LowCmd publisher");
        }

        std::cout << "[Init] Initializing LowState Subscriber..." << std::endl;
        state_sub = std::make_shared<Subscriber<LowState>>("rt/lowstate");
        if (!state_sub->init([this](const LowState& s){ this->StateCallback(s); })) {
            throw std::runtime_error("Failed to initialize LowState subscriber");
        }

        std::cout << "[Init] Initializing ControlModeState Subscriber..." << std::endl;
        controlmode_sub = std::make_shared<Subscriber<ControlModeState>>("rt/controlmodestate");
        if (!controlmode_sub->init([this](const ControlModeState& m){ this->ControlModeCallback(m); })) {
            std::cout << "[Warning] ControlModeState subscriber failed (optional)" << std::endl;
        }

        std::cout << "[Init] Waiting for first robot state..." << std::endl;
        int wait_count = 0;
        while (!state_received && wait_count < 100 && g_running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            wait_count++;
        }
        
        if (state_received) {
            std::cout << "[Init] ✓ Robot state synchronized" << std::endl;
        } else {
            std::cout << "[Warning] No robot state received within timeout" << std::endl;
        }
        
        std::cout << "\n[Init] Controller ready!\n" << std::endl;
    }

    ~IgrisController() {
        std::cout << "\n[Shutdown] Stopping controller..." << std::endl;
        StopControl();
        
        if (state_received) {
            std::cout << "[Shutdown] Turning off torque..." << std::endl;
            TORQUE_OFF();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        std::cout << "[Shutdown] Controller destroyed" << std::endl;
    }

    void BMS_AND_MOTOR_INIT() {
        PrintResult("Init BMS_AND_MOTOR", client.InitBms(BmsInitType::BMS_AND_MOTOR_INIT, 30000));
    }
    
    void TORQUE_ON() {
        PrintResult("Torque ON", client.SetTorque(TorqueType::TORQUE_ON, 30000));
    }
    
    void TORQUE_OFF() {
        PrintResult("Torque OFF", client.SetTorque(TorqueType::TORQUE_OFF, 30000));
    }
    
    void BMS_OFF() {
        PrintResult("BMS OFF", client.InitBms(BmsInitType::BMS_INIT_NONE, 30000));
    }
    
    void LOW_LEVEL_MODE() {
        auto res = client.SetControlMode(ControlMode::CONTROL_MODE_LOW_LEVEL, 30000);
        PrintResult("Mode: LOW_LEVEL", res);
        
        if (res.success()) {
            std::lock_guard<std::mutex> lock(target_mutex);
            for (int i = 0; i < NUM_MOTORS; i++) {
                target_joint_pos[i] = current_joint_pos[i];
                target_motor_pos[i] = current_motor_pos[i];
            }
            std::cout << "[Info] Target positions initialized to current state" << std::endl;
        }
    }

    void StartControl(bool use_joints = true) {
        if (control_thread_running) {
            std::cout << "[Warning] Control thread already running" << std::endl;
            return;
        }
        
        if (!first_state_received) {
            std::cout << "[Error] Cannot start control - no robot state received" << std::endl;
            return;
        }
        
        use_joint_mode = use_joints;
        lowlevel_active = true;
        control_thread_running = true;
        
        control_thread = std::thread(&IgrisController::ControlThreadFunction, this);
        
        std::cout << "[Control] Started in " << (use_joints ? "JOINT (PJS)" : "MOTOR (MS)") 
                  << " mode at 300Hz" << std::endl;
    }

    void StopControl() {
        if (!control_thread_running) return;
        
        std::cout << "[Control] Stopping control thread..." << std::endl;
        lowlevel_active = false;
        control_thread_running = false;
        
        if (control_thread.joinable()) {
            control_thread.join();
        }
        
        std::cout << "[Control] Control thread stopped. Published " 
                  << cmd_publish_count.load() << " commands" << std::endl;
    }

    void SetTargetJointPositions(const std::array<float, NUM_MOTORS>& positions) {
        std::lock_guard<std::mutex> lock(target_mutex);
        for (int i = 0; i < NUM_MOTORS; i++) {
            target_joint_pos[i] = clamp(positions[i], JOINT_POS_MIN[i], JOINT_POS_MAX[i]);
        }
    }

    void MoveJointsSmoothly(const std::array<float, NUM_MOTORS>& target_positions, 
                            double duration_sec = 3.0,
                            bool wait_for_completion = false) {
        if (!first_state_received) {
            std::cout << "[Error] No initial state received" << std::endl;
            return;
        }
        
        if (!lowlevel_active) {
            std::cout << "[Error] Control not active" << std::endl;
            return;
        }
        
        {
            std::lock_guard<std::mutex> traj_lock(trajectory_mutex_);
            std::lock_guard<std::mutex> target_lock(target_mutex);
            
            trajectory_state_.start_positions = (use_joint_mode ? target_joint_pos : target_motor_pos);
            
            for (int i = 0; i < NUM_MOTORS; i++) {
                if(i != 20 && i != 21){  // wrist_roll joints are excluded) if fix robot delete this line
                    if (use_joint_mode) {
                        trajectory_state_.end_positions[i] = clamp(target_positions[i], 
                            JOINT_POS_MIN[i], JOINT_POS_MAX[i]);
                    } else {
                        trajectory_state_.end_positions[i] = clamp(target_positions[i], 
                            MOTOR_POS_MIN[i], MOTOR_POS_MAX[i]);
                    }
                }
            }
            
            trajectory_state_.duration_sec = duration_sec;
            trajectory_state_.start_time = std::chrono::steady_clock::now();
            trajectory_state_.active = true;
            
            std::cout << "[Trajectory] Started smooth motion (" << duration_sec << "s)" << std::endl;
        }
        
        if (wait_for_completion) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(static_cast<int>(duration_sec * 1000 + 100)));
        }
    }

    void MoveSingleJointSmoothly(int joint_id, float target_position, 
                                 double duration_sec = 2.0,
                                 bool wait_for_completion = false) {
        if (joint_id < 0 || joint_id >= NUM_MOTORS) {
            std::cout << "[Error] Invalid joint ID: " << joint_id << std::endl;
            return;
        }
        
        std::array<float, NUM_MOTORS> target_pos;
        {
            std::lock_guard<std::mutex> lock(target_mutex);
            target_pos = (use_joint_mode ? target_joint_pos : target_motor_pos);
        }
        
        target_pos[joint_id] = target_position;
        
        std::cout << "[Trajectory] Moving " << JOINT_NAMES[joint_id] 
                  << " to " << target_position << " rad" << std::endl;
        
        MoveJointsSmoothly(target_pos, duration_sec, wait_for_completion);
    }

    void MoveToPositions(const std::array<float, NUM_MOTORS>& target_positions, 
                         double duration_sec = 3.0,
                         bool wait_for_completion = false) {
        MoveJointsSmoothly(target_positions, duration_sec, wait_for_completion);
    }
    
    void MoveToPositionsVector(const std::vector<double>& target_positions, 
                               double duration_sec = 3.0,
                               bool wait_for_completion = false) {
        if (target_positions.empty()) {
            std::cout << "[Error] Empty target positions vector" << std::endl;
            return;
        }
        
        std::array<float, NUM_MOTORS> target_pos;
        
        {
            std::lock_guard<std::mutex> lock(target_mutex);
            target_pos = (use_joint_mode ? target_joint_pos : target_motor_pos);
        }
        
        size_t count = std::min(target_positions.size(), static_cast<size_t>(NUM_MOTORS));
        for (size_t i = 0; i < count; i++) {
            target_pos[i] = static_cast<float>(target_positions[i]);
        }
        
        std::cout << "[Trajectory] Moving " << count << " joints to target positions" << std::endl;
        
        MoveJointsSmoothly(target_pos, duration_sec, wait_for_completion);
    }
    
    void MoveNamedJoints(const std::vector<std::string>& joint_names,
                         const std::vector<double>& target_positions,
                         double duration_sec = 3.0,
                         bool wait_for_completion = false) {
        if (joint_names.size() != target_positions.size()) {
            std::cout << "[Error] Joint names and positions size mismatch" << std::endl;
            return;
        }
        
        std::array<float, NUM_MOTORS> target_pos;
        
        {
            std::lock_guard<std::mutex> lock(target_mutex);
            target_pos = (use_joint_mode ? target_joint_pos : target_motor_pos);
        }
        
        int updated_count = 0;
        for (size_t i = 0; i < joint_names.size(); i++) {
            for (int j = 0; j < NUM_MOTORS; j++) {
                if (joint_names[i] == JOINT_NAMES[j]) {
                    target_pos[j] = static_cast<float>(target_positions[i]);
                    updated_count++;
                    break;
                }
            }
        }
        
        std::cout << "[Trajectory] Moving " << updated_count << " named joints" << std::endl;
        
        MoveJointsSmoothly(target_pos, duration_sec, wait_for_completion);
    }
    
    void MoveRelative(const std::array<float, NUM_MOTORS>& position_offsets,
                      double duration_sec = 3.0,
                      bool wait_for_completion = false) {
        std::array<float, NUM_MOTORS> target_pos;
        
        {
            std::lock_guard<std::mutex> lock(target_mutex);
            target_pos = (use_joint_mode ? target_joint_pos : target_motor_pos);
        }
        
        for (int i = 0; i < NUM_MOTORS; i++) {
            target_pos[i] += position_offsets[i];
        }
        
        std::cout << "[Trajectory] Moving with relative offsets" << std::endl;
        
        MoveJointsSmoothly(target_pos, duration_sec, wait_for_completion);
    }

    bool IsTrajectoryActive() {
        std::lock_guard<std::mutex> lock(trajectory_mutex_);
        return trajectory_state_.active;
    }

    float GetTrajectoryProgress() {
        std::lock_guard<std::mutex> lock(trajectory_mutex_);
        
        if (!trajectory_state_.active) {
            return 1.0f;
        }
        
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - trajectory_state_.start_time).count();
        
        return std::min(1.0f, static_cast<float>(elapsed / trajectory_state_.duration_sec));
    }

    bool GetCurrentJointPositions(std::array<float, NUM_MOTORS>& out_pos) {
        if (!state_received) return false;
        std::lock_guard<std::mutex> lock(state_mutex);
        out_pos = current_joint_pos;
        return true;
    }

    // ===== 추가: 현재 목표 위치 가져오기 =====
    bool GetTargetJointPositions(std::array<float, NUM_MOTORS>& out_pos) {
        std::lock_guard<std::mutex> lock(target_mutex);
        out_pos = target_joint_pos;
        return true;
    }

    void PrintStatus() {
        std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
        std::cout << "║         Controller Status             ║" << std::endl;
        std::cout << "╚════════════════════════════════════════╝" << std::endl;
        std::cout << "State received:    " << (state_received ? "YES" : "NO") << std::endl;
        std::cout << "State count:       " << state_count.load() << std::endl;
        std::cout << "Control active:    " << (lowlevel_active ? "YES" : "NO") << std::endl;
        std::cout << "Control mode:      " << (use_joint_mode ? "JOINT (PJS)" : "MOTOR (MS)") << std::endl;
        std::cout << "Commands sent:     " << cmd_publish_count.load() << std::endl;
        std::cout << "Filtered values:   " << filter_count.load() << std::endl;
        std::cout << std::endl;
    }
};

class IgrisROS2Bridge : public rclcpp::Node {
private:
    IgrisController* controller_;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr current_joint_pub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr target_joint_sub_;
    rclcpp::TimerBase::SharedPtr state_publish_timer_;
    std::chrono::milliseconds publish_period_{20};

    void PublishCurrentJointState() {
        std::array<float, NUM_MOTORS> current_pos;
        
        if (!controller_->GetCurrentJointPositions(current_pos)) {
            return;
        }
        
        auto msg = sensor_msgs::msg::JointState();
        msg.header.stamp = this->now();
        msg.header.frame_id = "base_link";
        
        msg.name.resize(NUM_MOTORS);
        msg.position.resize(NUM_MOTORS);
        msg.velocity.resize(NUM_MOTORS);
        msg.effort.resize(NUM_MOTORS);
        
        for (int i = 0; i < NUM_MOTORS; i++) {
            msg.name[i] = JOINT_NAMES[i];
            msg.position[i] = current_pos[i];
            msg.velocity[i] = 0.0;
            msg.effort[i] = 0.0;
        }
        
        current_joint_pub_->publish(msg);
    }

    void TargetJointCallback(const sensor_msgs::msg::JointState::SharedPtr msg) {
        if (msg->position.empty()) {
            return;
        }
        
        std::array<float, NUM_MOTORS> target_pos;
        
        // 현재 "목표" 위치를 기본값으로
        if (!controller_->GetTargetJointPositions(target_pos)) {
            return;
        }
        
        // 토픽에 있는 조인트만 업데이트
        if (!msg->name.empty()) {
            // exept Wrist_Roll_L and Wrist_Pitch_L
            for (size_t i = 0; i < msg->name.size() && i < msg->position.size(); i++) {
                for (int j = 0; j < NUM_MOTORS; j++) {
                    if (msg->name[i] == JOINT_NAMES[j]) {
                        if(j != 20 && j != 21) {  // wrist_roll joints are excluded
                            target_pos[j] = static_cast<float>(msg->position[i]);
                        }
                        break;
                    }
                }
            }
        } else {
            size_t count = std::min(msg->position.size(), static_cast<size_t>(NUM_MOTORS));
            for (size_t i = 0; i < count; i++) {
                target_pos[i] = static_cast<float>(msg->position[i]);
            }
        }
        
        controller_->SetTargetJointPositions(target_pos);
    }

    // void TargetJointCallback(const sensor_msgs::msg::JointState::SharedPtr msg) {
    //     if (msg->position.empty()) {
    //         return;
    //     }
        
    //     std::array<float, NUM_MOTORS> target_pos;
        
    //     if (!controller_->GetCurrentJointPositions(target_pos)) {
    //         return;
    //     }
        
    //     if (!msg->name.empty()) {
    //         for (size_t i = 0; i < msg->name.size() && i < msg->position.size(); i++) {
    //             for (int j = 0; j < NUM_MOTORS; j++) {
    //                 if (msg->name[i] == JOINT_NAMES[j]) {
    //                     target_pos[j] = static_cast<float>(msg->position[i]);
    //                     break;
    //                 }
    //             }
    //         }
    //     } else {
    //         size_t count = std::min(msg->position.size(), static_cast<size_t>(NUM_MOTORS));
    //         for (size_t i = 0; i < count; i++) {
    //             target_pos[i] = static_cast<float>(msg->position[i]);
    //         }
    //     }
        
    //     controller_->SetTargetJointPositions(target_pos);
    // }

public:
    IgrisROS2Bridge(IgrisController* ctrl) 
        : Node("igris_ros2_bridge"), controller_(ctrl) {
        
        RCLCPP_INFO(this->get_logger(), "Initializing ROS2 Bridge...");
        
        current_joint_pub_ = this->create_publisher<sensor_msgs::msg::JointState>(
            "/igris/joint_states", 10);
        
        target_joint_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
            "/igris/target_joint_states", 10,
            std::bind(&IgrisROS2Bridge::TargetJointCallback, this, std::placeholders::_1));
        
        state_publish_timer_ = this->create_wall_timer(
            publish_period_,
            std::bind(&IgrisROS2Bridge::PublishCurrentJointState, this));
        
        RCLCPP_INFO(this->get_logger(), "ROS2 Bridge initialized");
        RCLCPP_INFO(this->get_logger(), "Publishing: /igris/joint_states (50Hz)");
        RCLCPP_INFO(this->get_logger(), "Subscribing: /igris/target_joint_states");
        RCLCPP_INFO(this->get_logger(), "Mode: Direct position control with filtering");
    }
};

int main(int argc, char** argv) {
    try {
        rclcpp::init(argc, argv);
        
        std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
        std::cout << "║   IGRIS ROS2 Control Node             ║" << std::endl;
        std::cout << "╚════════════════════════════════════════╝\n" << std::endl;
        
        IgrisController robot(0);
        
        std::cout << "\n=== Initialization Sequence ===" << std::endl;
        robot.BMS_AND_MOTOR_INIT();
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        robot.TORQUE_ON();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        robot.LOW_LEVEL_MODE();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        robot.StartControl(true);
        
        std::array<float, NUM_MOTORS> current_pos;
        if (robot.GetCurrentJointPositions(current_pos)) {
            std::cout << "\nInitial Joint Positions:" << std::endl;
            for (int i = 0; i < NUM_MOTORS; i++) {
                printf("  J%02d (%20s): %7.4f rad\n", i, JOINT_NAMES[i], current_pos[i]);
            }
        }
        
        std::cout << "\n=== Testing Smooth Motion ===" << std::endl;
        std::array<float, NUM_MOTORS> zero_pose = {0.0f};
        // zero_pose[18] = -1.5f;
        // zero_pose[25] = -1.5f;
        zero_pose[30] = -0.2f;
        robot.MoveToPositions(zero_pose, 3.0, false);
        
        while (robot.IsTrajectoryActive()) {
            float progress = robot.GetTrajectoryProgress();
            std::cout << "\rProgress: " << std::fixed << std::setprecision(1) 
                      << (progress * 100) << "%" << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << std::endl;
        
        std::cout << "\n=== Starting ROS2 Bridge ===" << std::endl;
        auto ros2_node = std::make_shared<IgrisROS2Bridge>(&robot);
        
        std::cout << "\n=== ROS2 Node Running ===" << std::endl;
        std::cout << "Press Ctrl+C to stop\n" << std::endl;
        
        while (rclcpp::ok() && g_running) {
            rclcpp::spin_some(ros2_node);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        std::cout << "\n=== Cleanup ===" << std::endl;
        robot.PrintStatus();
        robot.StopControl();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        robot.TORQUE_OFF();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        robot.BMS_OFF();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        rclcpp::shutdown();
        
        std::cout << "\n[Main] Program completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n[Error] Exception: " << e.what() << std::endl;
        rclcpp::shutdown();
        return 1;
    }
    
    return 0;
}