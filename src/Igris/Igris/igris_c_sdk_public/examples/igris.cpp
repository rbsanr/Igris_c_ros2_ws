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

#define NUM_MOTORS 31

using namespace igris_sdk;
using namespace igris_c::msg::dds;

// Motor and Joint names (1-indexed in data, 0-indexed in array)
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

// Joint position limits (rad) from params.yaml
static const std::array<float, 31> JOINT_POS_MAX = {
    1.57f,                                                   // Waist_Yaw
    0.310f, 0.28f,                                           // Waist_Roll, Waist_Pitch
    0.480f, 2.300f, 1.570f, 2.280f, 0.698f, 0.349f,          // Left leg
    0.480f, 0.330f, 1.570f, 2.280f, 0.698f, 0.349f,          // Right leg
    1.047f, 3.140f, 1.570f, 0.0f,   1.570f, 0.870f, 0.650f,  // Left arm
    1.047f, 0.170f, 1.570f, 0.0f,   1.570f, 1.221f, 0.650f,  // Right arm
    1.221f, 0.520f                                           // Neck
};

static const std::array<float, 31> JOINT_POS_MIN = {
    -1.57f,                                                       // Waist_Yaw
    -0.310f, -0.87f,                                              // Waist_Roll, Waist_Pitch
    -2.000f, -0.330f, -1.570f, 0.0f,  -0.70f,  -0.350f,           // Left leg
    -2.000f, -2.300f, -1.570f, 0.0f,  -0.70f,  -0.350f,           // Right leg
    -3.141f, -0.170f, -1.570f, -2.0f, -1.570f, -1.221f, -0.650f,  // Left arm
    -3.141f, -3.140f, -1.570f, -2.0f, -1.570f, -0.870f, -0.650f,  // Right arm
    -1.221f, -0.520f                                              // Neck
};

// Motor position limits (rad) from params.yaml
static const std::array<float, 31> MOTOR_POS_MAX = {
    1.57f,                                                  // Waist_Yaw
    0.87f,  0.87f,                                          // Waist_L, Waist_R
    0.480f, 2.300f, 1.570f, 2.280f, 0.609f, 0.523f,         // Left leg
    0.480f, 0.330f, 1.570f, 2.280f, 0.609f, 0.523f,         // Right leg
    1.047f, 3.140f, 1.570f, 0.0f,   1.570f, 0.75f,  0.75f,  // Left arm
    1.047f, 0.170f, 1.570f, 0.0f,   1.570f, 0.98f,  0.98f,  // Right arm
    1.221f, 0.520f                                          // Neck
};

static const std::array<float, 31> MOTOR_POS_MIN = {
    -1.57f,                                                      // Waist_Yaw
    -0.34f,  -0.34f,                                             // Waist_L, Waist_R
    -2.000f, -0.330f, -1.570f, 0.0f,  -0.630f, -0.617f,          // Left leg
    -2.000f, -2.300f, -1.570f, 0.0f,  -0.630f, -0.617f,          // Right leg
    -3.141f, -0.170f, -1.570f, -2.0f, -1.570f, -0.98f,  -0.98f,  // Left arm
    -3.141f, -3.140f, -1.570f, -2.0f, -1.570f, -0.75f,  -0.75f,  // Right arm
    -1.221f, -0.520f                                             // Neck
};

// Example PD gains (adjust for your robot)
static const std::array<float, NUM_MOTORS> kp = {
    50.0,  25.0,  25.0,                            // Waist
    500.0, 200.0, 50.0, 500.0, 300.0, 300.0,       // Left leg
    500.0, 200.0, 50.0, 500.0, 300.0, 300.0,       // Right leg
    50.0,  50.0,  30.0, 30.0,  5.0,   5.0,   5.0,  // Left arm
    50.0,  50.0,  30.0, 30.0,  5.0,   5.0,   5.0,  // Right arm
    2.0,   5.0                                     // Neck
};
static const std::array<float, NUM_MOTORS> kd = {
    0.8,  0.8, 0.8,                        // Waist
    3.0,  0.5, 0.5,  3.0,  1.5, 1.5,       // Left leg
    3.0,  0.5, 0.5,  3.0,  1.5, 1.5,       // Right leg
    0.5,  0.5, 0.15, 0.15, 0.1, 0.1, 0.1,  // Left arm
    0.5,  0.5, 0.15, 0.15, 0.1, 0.1, 0.1,  // Right arm
    0.05, 0.1                              // Neck
};

// Global running flag for signal handling
static std::atomic<bool> g_running(true);

void SignalHandler(int) { 
    g_running = false; 
    std::cout << "\n[Signal] Shutdown requested..." << std::endl;
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

    // ===== 추가: 상태 추적 변수들 =====
    std::atomic<bool> state_received{false};
    std::atomic<bool> first_state_received{false};
    std::atomic<uint32_t> state_count{0};
    std::atomic<uint32_t> cmd_publish_count{0};
    
    // ===== 추가: Joint/Motor 현재 위치 추적 =====
    std::array<float, NUM_MOTORS> current_joint_pos{};
    std::array<float, NUM_MOTORS> current_motor_pos{};
    
    // ===== 추가: 초기 위치 저장 (안전한 복귀 지점) =====
    std::array<float, NUM_MOTORS> initial_joint_pos{};
    std::array<float, NUM_MOTORS> initial_motor_pos{};
    
    // ===== 추가: 목표 위치 (제어 스레드와 공유) =====
    std::array<float, NUM_MOTORS> target_joint_pos{};
    std::array<float, NUM_MOTORS> target_motor_pos{};
    
    // ===== 추가: 제어 모드 추적 =====
    std::atomic<bool> lowlevel_active{false};
    std::atomic<bool> use_joint_mode{true}; // true: PJS, false: MS
    
    std::mutex state_mutex;
    std::mutex target_mutex;
    std::mutex controlmode_mutex;
    
    // ===== 추가: 전용 제어 스레드 =====
    std::thread control_thread;
    std::atomic<bool> control_thread_running{false};

    // ===== 추가: 위치 클램핑 함수 =====
    float clamp(float value, float min_val, float max_val) {
        if (value < min_val) return min_val;
        if (value > max_val) return max_val;
        return value;
    }

    void PrintResult(const std::string &action, const ServiceResponse &res) {
        std::cout << "\n[" << action << "] " << (res.success() ? "SUCCESS" : "FAILED") 
                  << " - " << res.message() << std::endl;
    }

    // ===== 개선: LowState 콜백 (Joint + Motor 모두 추적) =====
    void StateCallback(const LowState &state) {
        std::lock_guard<std::mutex> lock(state_mutex);
        current_state = state;
        state_count++;
        
        // Motor와 Joint 위치 모두 업데이트
        for (int i = 0; i < NUM_MOTORS; i++) {
            if (i < state.motor_state().size()) {
                current_motor_pos[i] = state.motor_state()[i].q();
            }
            if (i < state.joint_state().size()) {
                current_joint_pos[i] = state.joint_state()[i].q();
            }
        }
        
        // ===== 추가: 첫 수신 시 초기 위치 저장 =====
        if (!first_state_received) {
            for (int i = 0; i < NUM_MOTORS; i++) {
                initial_motor_pos[i] = current_motor_pos[i];
                initial_joint_pos[i] = current_joint_pos[i];
                
                // 목표 위치도 현재 위치로 초기화
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

    // ===== 추가: ControlModeState 콜백 =====
    void ControlModeCallback(const ControlModeState &mode_state) {
        std::lock_guard<std::mutex> lock(controlmode_mutex);
        current_controlmode = mode_state;
    }

    // ===== 추가: 300Hz 제어 스레드 =====
    void ControlThreadFunction() {
        const auto period = std::chrono::microseconds(3333);  // ~300Hz
        auto next_time = std::chrono::steady_clock::now();

        std::cout << "[Control Thread] Started at 300Hz" << std::endl;

        while (control_thread_running && g_running) {
            if (lowlevel_active && first_state_received) {
                LowCmd local_cmd;
                
                {
                    std::lock_guard<std::mutex> lock(target_mutex);
                    
                    // Kinematic mode 설정
                    local_cmd.kinematic_mode(use_joint_mode ? KinematicMode::PJS : KinematicMode::MS);
                    
                    for (int i = 0; i < NUM_MOTORS; i++) {
                        auto &m = local_cmd.motors()[i];
                        m.id(i);
                        
                        // 모드에 따라 목표 위치 선택 및 클램핑
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
                
                // Publish
                cmd_pub->write(local_cmd);
                cmd_publish_count++;
            }
            
            // 정확한 주기 유지
            next_time += period;
            std::this_thread::sleep_until(next_time);
        }
        
        std::cout << "[Control Thread] Stopped" << std::endl;
    }

public:
    IgrisController(int id = 0) : domain_id(id) {
        // 신호 핸들러 등록
        signal(SIGINT, SignalHandler);
        signal(SIGTERM, SignalHandler);
        
        std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
        std::cout << "║   IGRIS Controller Initialization     ║" << std::endl;
        std::cout << "╚════════════════════════════════════════╝\n" << std::endl;

        // 1. SDK 채널 초기화
        std::cout << "[Init] Initializing ChannelFactory..." << std::endl;
        ChannelFactory::Instance()->Init(domain_id);
        if (!ChannelFactory::Instance()->IsInitialized()) {
            throw std::runtime_error("Failed to initialize ChannelFactory");
        }

        // 2. 서비스 클라이언트 초기화
        std::cout << "[Init] Initializing Service Client..." << std::endl;
        client.Init();
        client.SetTimeout(10.0f);

        // 3. Publisher 초기화
        std::cout << "[Init] Initializing LowCmd Publisher..." << std::endl;
        cmd_pub = std::make_shared<Publisher<LowCmd>>("rt/lowcmd");
        if (!cmd_pub->init()) {
            throw std::runtime_error("Failed to initialize LowCmd publisher");
        }

        // 4. LowState Subscriber 초기화
        std::cout << "[Init] Initializing LowState Subscriber..." << std::endl;
        state_sub = std::make_shared<Subscriber<LowState>>("rt/lowstate");
        if (!state_sub->init([this](const LowState& s){ this->StateCallback(s); })) {
            throw std::runtime_error("Failed to initialize LowState subscriber");
        }

        // 5. ControlModeState Subscriber 초기화 (선택적)
        std::cout << "[Init] Initializing ControlModeState Subscriber..." << std::endl;
        controlmode_sub = std::make_shared<Subscriber<ControlModeState>>("rt/controlmodestate");
        if (!controlmode_sub->init([this](const ControlModeState& m){ this->ControlModeCallback(m); })) {
            std::cout << "[Warning] ControlModeState subscriber failed (optional)" << std::endl;
        }

        // 6. 첫 상태 수신 대기
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
        
        // 안전하게 토크 끄기
        if (state_received) {
            std::cout << "[Shutdown] Turning off torque..." << std::endl;
            TORQUE_OFF();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        std::cout << "[Shutdown] Controller destroyed" << std::endl;
    }

    // --- Service API Wrappers ---
    void BMS_INIT() {
        PrintResult("Init BMS", client.InitBms(BmsInitType::BMS_INIT, 30000));
    }
    
    void MOTOR_INIT() {
        PrintResult("Init Motor", client.InitBms(BmsInitType::MOTOR_INIT, 30000));
    }
    
    void BMS_AND_MOTOR_INIT() {
        PrintResult("Init BMS_AND_MOTOR", client.InitBms(BmsInitType::BMS_AND_MOTOR_INIT, 30000));
    }
    
    void BMS_OFF() {
        PrintResult("BMS OFF", client.InitBms(BmsInitType::BMS_INIT_NONE, 30000));
    }
    
    void TORQUE_ON() {
        PrintResult("Torque ON", client.SetTorque(TorqueType::TORQUE_ON, 30000));
    }
    
    void TORQUE_OFF() {
        PrintResult("Torque OFF", client.SetTorque(TorqueType::TORQUE_OFF, 30000));
    }
    
    void LOW_LEVEL_MODE() {
        auto res = client.SetControlMode(ControlMode::CONTROL_MODE_LOW_LEVEL, 30000);
        PrintResult("Mode: LOW_LEVEL", res);
        
        if (res.success()) {
            // 목표 위치를 현재 위치로 초기화
            std::lock_guard<std::mutex> lock(target_mutex);
            for (int i = 0; i < NUM_MOTORS; i++) {
                target_joint_pos[i] = current_joint_pos[i];
                target_motor_pos[i] = current_motor_pos[i];
            }
            std::cout << "[Info] Target positions initialized to current state" << std::endl;
        }
    }
    
    void HIGH_LEVEL_MODE() {
        PrintResult("Mode: HIGH_LEVEL", client.SetControlMode(ControlMode::CONTROL_MODE_HIGH_LEVEL, 30000));
    }

    // ===== 추가: 제어 시작/중지 함수 =====
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

    // ===== 추가: 안전한 위치 설정 함수 (클램핑 포함) =====
    void SetTargetJointPositions(const std::array<float, NUM_MOTORS>& positions) {
        std::lock_guard<std::mutex> lock(target_mutex);
        for (int i = 0; i < NUM_MOTORS; i++) {
            target_joint_pos[i] = clamp(positions[i], JOINT_POS_MIN[i], JOINT_POS_MAX[i]);
        }
    }

    void SetTargetMotorPositions(const std::array<float, NUM_MOTORS>& positions) {
        std::lock_guard<std::mutex> lock(target_mutex);
        for (int i = 0; i < NUM_MOTORS; i++) {
            target_motor_pos[i] = clamp(positions[i], MOTOR_POS_MIN[i], MOTOR_POS_MAX[i]);
        }
    }

    // ===== 추가: 단일 조인트 제어 함수 =====
    void SetSingleJoint(int joint_id, float position) {
        if (joint_id < 0 || joint_id >= NUM_MOTORS) {
            std::cout << "[Error] Invalid joint ID: " << joint_id << std::endl;
            return;
        }
        
        std::lock_guard<std::mutex> lock(target_mutex);
        target_joint_pos[joint_id] = clamp(position, JOINT_POS_MIN[joint_id], JOINT_POS_MAX[joint_id]);
    }

    void SetSingleMotor(int motor_id, float position) {
        if (motor_id < 0 || motor_id >= NUM_MOTORS) {
            std::cout << "[Error] Invalid motor ID: " << motor_id << std::endl;
            return;
        }
        
        std::lock_guard<std::mutex> lock(target_mutex);
        target_motor_pos[motor_id] = clamp(position, MOTOR_POS_MIN[motor_id], MOTOR_POS_MAX[motor_id]);
    }

    // ===== 추가: 초기 위치로 복귀 =====
    void ResetToInitial() {
        if (!first_state_received) {
            std::cout << "[Error] No initial position saved" << std::endl;
            return;
        }
        
        std::lock_guard<std::mutex> lock(target_mutex);
        target_joint_pos = initial_joint_pos;
        target_motor_pos = initial_motor_pos;
        
        std::cout << "[Control] Reset to initial positions" << std::endl;
    }

    // --- Getter 함수들 ---
    bool GetCurrentJointPositions(std::array<float, NUM_MOTORS>& out_pos) {
        if (!state_received) return false;
        std::lock_guard<std::mutex> lock(state_mutex);
        out_pos = current_joint_pos;
        return true;
    }

    bool GetCurrentMotorPositions(std::array<float, NUM_MOTORS>& out_pos) {
        if (!state_received) return false;
        std::lock_guard<std::mutex> lock(state_mutex);
        out_pos = current_motor_pos;
        return true;
    }

    void GetCurrentIMU(std::array<float, 4>& quat, std::array<float, 3>& gyro, 
                       std::array<float, 3>& accel, std::array<float, 3>& rpy) {
        std::lock_guard<std::mutex> lock(state_mutex);
        const auto& imu = current_state.imu_state();
        
        for (int i = 0; i < 4; i++) quat[i] = imu.quaternion()[i];
        for (int i = 0; i < 3; i++) {
            gyro[i] = imu.gyroscope()[i];
            accel[i] = imu.accelerometer()[i];
            rpy[i] = imu.rpy()[i];
        }
    }

    // ===== 추가: 상태 출력 함수 =====
    void PrintStatus() {
        std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
        std::cout << "║         Controller Status             ║" << std::endl;
        std::cout << "╚════════════════════════════════════════╝" << std::endl;
        std::cout << "State received:    " << (state_received ? "YES" : "NO") << std::endl;
        std::cout << "State count:       " << state_count.load() << std::endl;
        std::cout << "Control active:    " << (lowlevel_active ? "YES" : "NO") << std::endl;
        std::cout << "Control mode:      " << (use_joint_mode ? "JOINT (PJS)" : "MOTOR (MS)") << std::endl;
        std::cout << "Commands sent:     " << cmd_publish_count.load() << std::endl;
        std::cout << std::endl;
    }
};

int main() {
    try {
        // 1. 컨트롤러 생성
        IgrisController robot(0);
        
        // 2. 초기화 시퀀스
        std::cout << "\n=== Initialization Sequence ===" << std::endl;
        robot.BMS_AND_MOTOR_INIT();
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        robot.TORQUE_ON();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        robot.LOW_LEVEL_MODE();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // 3. 제어 시작 (Joint 모드)
        std::cout << "\n=== Starting Control ===" << std::endl;
        robot.StartControl(true);  // true = Joint mode (PJS)
        
        // 4. 현재 위치 확인
        std::array<float, NUM_MOTORS> current_pos;
        if (robot.GetCurrentJointPositions(current_pos)) {
            std::cout << "\nInitial Joint Positions:" << std::endl;
            for (int i = 0; i < NUM_MOTORS; i++) {
                printf("  J%02d (%20s): %7.4f rad\n", i, JOINT_NAMES[i], current_pos[i]);
            }
        }
        
        // 5. 예제: Neck_Pitch (30번) 조인트를 천천히 움직이기
        std::cout << "\n=== Example: Moving Neck_Pitch ===" << std::endl;
        
        const int test_joint = 30;  // Neck_Pitch
        const float target_angle = 0.3f;  // 목표 각도
        const int duration_sec = 5;
        
        std::cout << "Moving joint " << test_joint << " (" << JOINT_NAMES[test_joint] 
                  << ") to " << target_angle << " rad over " << duration_sec << " seconds" << std::endl;
        
        std::array<float, NUM_MOTORS> target_pos = current_pos;
        target_pos[test_joint] = target_angle;
        robot.SetTargetJointPositions(target_pos);
        
        // 진행 상황 모니터링
        for (int i = 0; i < duration_sec && g_running; i++) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            robot.GetCurrentJointPositions(current_pos);
            printf("[%d/%d] Current: %.4f rad, Target: %.4f rad\n", 
                   i+1, duration_sec, current_pos[test_joint], target_angle);
        }
        
        // 6. 초기 위치로 복귀
        if (g_running) {
            std::cout << "\n=== Returning to Initial Position ===" << std::endl;
            robot.ResetToInitial();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        
        // 7. 상태 출력
        robot.PrintStatus();
        
        // 8. 정리
        std::cout << "\n=== Cleanup ===" << std::endl;
        robot.StopControl();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        robot.TORQUE_OFF();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        std::cout << "\n[Main] Program completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n[Error] Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}