// // // #include <Arduino.h>
// // // #include <BLEDevice.h>
// // // #include <BLEServer.h>
// // // #include <BLEUtils.h>
// // // #include <BLE2902.h>

// // // #define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
// // // #define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
// // // #define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// // // // ==================== BUTTON & BUZZER ====================
// // // const int BTN_PIN    = 35;     // 3.3V -> Button -> IO35 + 1k pull-down to GND
// // // const int BUZZER_PIN = 13;     // IO13 -> Buzzer (+) -> GND
// // // #define BTN_PRESSED_LEVEL  HIGH
// // // #define BUZZER_ACTIVE_HIGH true
// // // #define BTN_DEBOUNCE_MS    40
// // // #define BTN_RECAL_HOLD_MS  1200

// // // // ==================== MOTOR ====================
// // // const int R_PWM = 23, R_IN1 = 21, R_IN2 = 22;   // U6 - right
// // // const int L_PWM = 25, L_IN1 = 27, L_IN2 = 26;   // U7 - left
// // // const int STBY  = 33;
// // // #define R_INVERT false
// // // #define L_INVERT false
// // // #define PWM_FREQ 20000
// // // #define PWM_RES  8

// // // // ==================== ENCODER ====================
// // // const int ENC_L_A = 4, ENC_R_A = 14;
// // // volatile long pulseLeft = 0, pulseRight = 0;
// // // void IRAM_ATTR isrL() { pulseLeft++; }
// // // void IRAM_ATTR isrR() { pulseRight++; }

// // // // ==================== 12 SENSORS (CD74HC4067) ====================
// // // #define NS 12
// // // const uint8_t MUX_S[4] = {19, 18, 17, 16};
// // // const uint8_t SIG_PIN  = 34;

// // // const uint8_t MUX_CH[NS] = {11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
// // // const uint16_t POS[NS] = {0, 1000, 2000, 3000, 4000, 5000,
// // //                           6000, 7000, 8000, 9000, 10000, 11000};
// // // #define SETPOINT 5500
// // // #define INVERT_COLOR false
// // // #define SENSOR_THRESHOLD 500
// // // #define MUX_SETTLE_US 25

// // // uint16_t calMin[NS], calMax[NS], rawv[NS], sensorVal[NS];
// // // uint16_t pattern = 0;
// // // unsigned int lastLinePos = SETPOINT;

// // // void readRaw() {
// // //     for (int i = 0; i < NS; i++) {
// // //         uint8_t ch = MUX_CH[i];
// // //         for (int b = 0; b < 4; b++) digitalWrite(MUX_S[b], (ch >> b) & 1);
// // //         delayMicroseconds(MUX_SETTLE_US);
// // //         analogRead(SIG_PIN);
// // //         rawv[i] = (analogRead(SIG_PIN) + analogRead(SIG_PIN)) / 2;
// // //     }
// // // }
// // // void resetCal() { for (int i = 0; i < NS; i++) { calMin[i] = 4095; calMax[i] = 0; } }
// // // void calibrateStep() {
// // //     readRaw();
// // //     for (int i = 0; i < NS; i++) {
// // //         if (rawv[i] < calMin[i]) calMin[i] = rawv[i];
// // //         if (rawv[i] > calMax[i]) calMax[i] = rawv[i];
// // //     }
// // // }
// // // bool channelOk(int i) { return calMax[i] > calMin[i] + 300; }

// // // int docSensor() {
// // //     readRaw();
// // //     uint32_t s = 0, w = 0;
// // //     int cnt = 0;
// // //     pattern = 0;
// // //     for (int i = 0; i < NS; i++) {
// // //         int range = (int)calMax[i] - (int)calMin[i];
// // //         int v = 0;
// // //         if (range > 50) {
// // //             v = constrain(((long)rawv[i] - calMin[i]) * 1000 / range, 0, 1000);
// // //             if (INVERT_COLOR) v = 1000 - v;
// // //         }
// // //         sensorVal[i] = (uint16_t)v;
// // //         if (v > SENSOR_THRESHOLD) { pattern |= (1 << i); cnt++; }
// // //         if (v > 100) {
// // //             uint32_t weight = v - 100;
// // //             s += weight * POS[i];
// // //             w += weight;
// // //         }
// // //     }
// // //     if (w) lastLinePos = s / w;
// // //     return cnt;
// // // }

// // // unsigned int getMaskedLinePos(int fromIdx, int toIdx, unsigned int fallbackPos) {
// // //     uint32_t s = 0, w = 0;
// // //     bool hasStrong = false;
// // //     for (int i = fromIdx; i <= toIdx; i++) {
// // //         if (sensorVal[i] > SENSOR_THRESHOLD) hasStrong = true;
// // //         if (sensorVal[i] > 100) {
// // //             uint32_t weight = sensorVal[i] - 100;
// // //             s += weight * POS[i];
// // //             w += weight;
// // //         }
// // //     }
// // //     return (hasStrong && w) ? (unsigned int)(s / w) : fallbackPos;
// // // }

// // // // ==================== PID & SPEED ====================
// // // float KP = 0.04f, KI = 0.00005f, KD = 0.8.f;
// // // int   BASE      = 100;
// // // int   SPEED_MIN = 100;
// // // int   E_SLOW    = 2500;

// // // int   TURN_BASE = 100, TURN_OUT = 50, TURN_IN = 50; // Cua cham, banh ngoai 100, banh trong -50 chong vuot line
// // // #define TURN_MAX_COUNT    8
// // // #define TURN_HOLD_MS      20
// // // #define LOST_TURN_SPEED   85    // Toc do xoay tim line cham, binh tinh khi mat vach
// // // const int BOOST = 15, BOOST_THRESHOLD = 160;

// // // #define REF_DT_US   4000
// // // #define D_FILTER    0.4f
// // // #define I_MAX       30.0f
// // // #define I_ZONE      2500
// // // #define STEER_SLOW  100.0f
// // // #define ACCEL_PER_S 500.0f

// // // // ==================== START BLACK BOX ====================
// // // #define START_BOX_SPEED         90    // Toc do chay thang binh tinh khi thoat khoi o START
// // // #define START_BOX_BLOCK_CNT     6     // >= 6 sensors (or span >= 6) = inside / crossing black block
// // // #define START_EXIT_CENTER_MS    15    // Confirm single line in center (both outer 2 sensors white)
// // // #define START_EXIT_NEAR_EDGE_MS 20    // Confirm single line when outermost tips (0 & 11) are white
// // // #define START_EXIT_EXTREME_MS   35    // Confirm single line if offset onto extreme sensor 0 or 11
// // // #define START_WHITE_LOST_MS     45    // All-white after black block -> slid off line, pivot back
// // // #define START_BOX_MAX_MS        2500  // Safety timeout for start-box exit mode
// // // #define POST_BOX_TURN_GUARD_MS  120   // Suppress false 90-deg turn right at black-box exit
// // // #define POST_BOX_STOP_GUARD_MS  600   // Extra finish-line lock after leaving start black box

// // // // ==================== KICH BAN DAU MAP: GOC CONG DAU -> RE VONG TRON -> BO QUA NHANH CONG TREN ====================
// // // // 1. Goc cong dau (VD: 001111100000 ben Trai hoac 000001111100 ben Phai):
// // // //    -> Ghi nho ben den, KHOA DI THANG vuot qua goc cong dau.
// // // // 2. Di thang tiep len vach ngang tren: sang >= 4/6 mat thang giua (hoac cnt >= 4, span >= 4):
// // // //    + Goc cong dau ben TRAI (001111100000) -> RE PHAI (+1: banh trai tien, banh phai lui)
// // // //    + Goc cong dau ben PHAI (000001111100) -> RE TRAI (-1: banh trai lui, banh phai tien)
// // // // 3. Tren cung tron: khi gap nhanh vong cong phia tren (VD: 000001110000 pos~6095 hoac 000001111000 pos~6519):
// // // //    -> Che mat phia ngoai, tiep tuc om cung tron trong bo qua nhanh re, vuot qua xong tra ve PID chuan.
// // // #define ENABLE_ROUNDABOUT_START 1
// // // #define ROUND_APPROACH_SPEED    85    // Toc do di thang binh tinh qua goc cong dau & len vach ngang tren
// // // #define ROUND_ARC_SPEED         85    // Toc do bam cung tron on dinh
// // // #define ROUND_ARM_LINE_MS       25    // On dinh tren vach thang don (1..3 mat) sau o START
// // // #define ROUND_J1_HOLD_MS        55    // Thoi gian khoa thang toi thieu de vuot qua goc cong dau
// // // #define ROUND_J1_MAX_MS         175   // Thoi gian khoa thang toi da khi vuot goc cong dau
// // // #define ROUND_T_ARM_MS          35    // Quang nghi ngan sau khi thoat goc cong dau truoc khi canh vach ngang tren
// // // #define ROUND_PRE_BRAKE_MS      22    // Nhap phanh ngan triet quan tinh truoc khi xoay vao vong tron
// // // #define ROUND_PRE_BRAKE_PWM     160   // Luc phanh nguoc ngan truoc khi cua
// // // #define ROUND_TURN_HOLD_MS      85    // Thoi gian xoay toi thieu de tach khoi vach ngang tren
// // // #define ROUND_TURN_MAX_MS       420   // Thoi gian xoay toi da khi re vao vong tron
// // // #define ROUND_TURN_BRAKE_MS     10    // Phanh nguoc chong quan tinh xoay khi vua bat duoc line vong tron
// // // #define ROUND_ARC_SETTLE_MS     100   // On dinh tren cung tron sau cu re truoc khi canh nhanh cong phia tren
// // // #define ROUND_MID_HOLD_MS       80    // Thoi gian che mat ngoai toi thieu de luot qua nhanh cong phia tren
// // // #define ROUND_MID_MAX_MS        240   // Thoi gian che mat ngoai toi da khi bo qua nhanh cong phia tren

// // // enum RoundStage : uint8_t {
// // //     RND_ARM_J1        = 0,
// // //     RND_WAIT_J1       = 1,
// // //     RND_PASS_J1       = 2,
// // //     RND_WAIT_CIRCLE_T = 3,
// // //     RND_TURN_CIRCLE   = 4,
// // //     RND_ARC_WAIT_MID  = 5,
// // //     RND_ARC_PASS_MID  = 6,
// // //     RND_DONE          = 7
// // // };

// // // // ==================== GAP & FINISH ====================
// // // #define GAP_ERR_MAX       1800
// // // #define GAP_EDGE_GUARD_MS 120
// // // #define GAP_MAX_COUNT     4
// // // #define GAP_TIMEOUT_MS    1500

// // // #define STOP_MIN_CNT    10
// // // #define STOP_CNT2       1
// // // #define END_CONFIRM     2
// // // #define START_GRACE_MS  1500
// // // #define BRAKE_REV_SPEED 255
// // // #define BRAKE_REV_MS    65

// // // int  cnt1 = 0, cnt2 = 0;
// // // bool lostNow = false, gapStraight = false, blackLatched = false;
// // // int  fullBlackFrames = 0, gapSpeed = 0;
// // // int  activeTurnDir = 0;
// // // unsigned long gapStart = 0, armStopAt = 0, lastEdgeMs = 0, turnHoldUntil = 0;

// // // bool inStartBox = false, sawStartBlack = false, startCenteredOnce = false;
// // // int  lastBoxSide = 0;
// // // unsigned long startLineConfirmMs = 0, startWhiteStartMs = 0, postBoxTurnEnableAt = 0;

// // // uint8_t roundStage = RND_DONE;
// // // unsigned long roundStageMs = 0;
// // // int  j1BranchSide  = 0;   // -1 = goc cong dau ben TRAI (001111100000), +1 = goc cong dau ben PHAI (000001111100)
// // // int  circleTurnDir = 1;   // +1 = re PHAI theo mui xe, -1 = re TRAI theo mui xe
// // // int  j1LeftFrames  = 0, j1RightFrames = 0;
// // // int  stageConfirmFrames = 0, stageWhiteFrames = 0;
// // // int  arcSettleFrames = 0;

// // // float integral = 0, lastE = 0, dFiltered = 0, curV = 100;
// // // unsigned long lastPidUs = 0;
// // // int   speed_dk = 120;
// // // int   lastError = 0;

// // // bool timing = false;
// // // unsigned long runStartMs = 0, runElapsedMs = 0;
// // // void startTimer() { runStartMs = millis(); runElapsedMs = 0; timing = true; }
// // // void stopTimer()  { if (timing) { runElapsedMs = millis() - runStartMs; timing = false; } }

// // // bool runLine = false, testMode = false;
// // // bool calibrated = false, calibrating = false;
// // // unsigned long calEnd = 0, lastRawTime = 0, lastStatusTime = 0;
// // // #define CAL_TIME 5000
// // // #define STATUS_INTERVAL 150
// // // const char* currentModeName = "IDLE";
// // // const char* currentCaseName = "-";

// // // BLEServer*         pServer = nullptr;
// // // BLECharacteristic* pTx     = nullptr;
// // // bool deviceConnected = false, oldDeviceConnected = false;

// // // portMUX_TYPE cmdMux = portMUX_INITIALIZER_UNLOCKED;
// // // String pendingCmd = "";
// // // volatile bool hasPendingCmd = false;

// // // // ==================== NON-BLOCKING BUZZER ====================
// // // int  buzRemain = 0;
// // // bool buzState  = false;
// // // unsigned long buzNextMs = 0, buzOnDur = 80, buzOffDur = 60;

// // // inline void setBuzzerPin(bool on) {
// // //     digitalWrite(BUZZER_PIN, (on ^ !BUZZER_ACTIVE_HIGH) ? HIGH : LOW);
// // // }

// // // void beep(int count, unsigned long onMs = 80, unsigned long offMs = 60) {
// // //     if (count <= 0) { buzRemain = 0; buzState = false; setBuzzerPin(false); return; }
// // //     buzRemain = count;
// // //     buzOnDur  = onMs;
// // //     buzOffDur = offMs;
// // //     buzState  = true;
// // //     setBuzzerPin(true);
// // //     buzNextMs = millis() + onMs;
// // // }

// // // void updateBuzzer() {
// // //     if (buzRemain <= 0 && !buzState) return;
// // //     unsigned long now = millis();
// // //     if ((long)(now - buzNextMs) < 0) return;

// // //     if (buzState) {
// // //         buzState = false;
// // //         setBuzzerPin(false);
// // //         buzRemain--;
// // //         if (buzRemain > 0) buzNextMs = now + buzOffDur;
// // //     } else if (buzRemain > 0) {
// // //         buzState = true;
// // //         setBuzzerPin(true);
// // //         buzNextMs = now + buzOnDur;
// // //     }
// // // }

// // // // ==================== MOTOR DRIVE ====================
// // // void pwmSetup() {
// // // #if ESP_ARDUINO_VERSION_MAJOR >= 3
// // //     ledcAttach(R_PWM, PWM_FREQ, PWM_RES);
// // //     ledcAttach(L_PWM, PWM_FREQ, PWM_RES);
// // // #else
// // //     ledcSetup(0, PWM_FREQ, PWM_RES); ledcAttachPin(R_PWM, 0);
// // //     ledcSetup(1, PWM_FREQ, PWM_RES); ledcAttachPin(L_PWM, 1);
// // // #endif
// // // }
// // // void pwmWrite(bool right, int duty) {
// // // #if ESP_ARDUINO_VERSION_MAJOR >= 3
// // //     ledcWrite(right ? R_PWM : L_PWM, duty);
// // // #else
// // //     ledcWrite(right ? 0 : 1, duty);
// // // #endif
// // // }
// // // void setMotor(bool right, int sp) {
// // //     sp = constrain(sp, -255, 255);
// // //     bool fwd = (sp >= 0) ^ (right ? R_INVERT : L_INVERT);
// // //     int in1 = right ? R_IN1 : L_IN1, in2 = right ? R_IN2 : L_IN2;
// // //     if (sp == 0) {
// // //         digitalWrite(in1, HIGH);
// // //         digitalWrite(in2, HIGH);
// // //         pwmWrite(right, 255);
// // //     } else {
// // //         digitalWrite(in1, fwd ? HIGH : LOW);
// // //         digitalWrite(in2, fwd ? LOW : HIGH);
// // //         pwmWrite(right, abs(sp));
// // //     }
// // // }
// // // void move(int left, int right) { setMotor(false, left); setMotor(true, right); }
// // // void stopMotor() { move(0, 0); }

// // // void hardBrakeStop() {
// // //     unsigned long brakeMs = BRAKE_REV_MS;
// // //     if (brakeMs > 0) {
// // //         bool encActive = (pulseLeft > 30 && pulseRight > 30);
// // //         unsigned long minHold = encActive ? (brakeMs * 65 / 100) : brakeMs;
// // //         move(-BRAKE_REV_SPEED, -BRAKE_REV_SPEED);
// // //         unsigned long t0 = millis();
// // //         long lastPL = pulseLeft, lastPR = pulseRight;
// // //         unsigned long lastMoveMs = t0;
// // //         while (millis() - t0 < brakeMs) {
// // //             long pL = pulseLeft, pR = pulseRight;
// // //             unsigned long now = millis();
// // //             if (pL != lastPL || pR != lastPR) {
// // //                 lastPL = pL;
// // //                 lastPR = pR;
// // //                 lastMoveMs = now;
// // //             }
// // //             if (encActive && (now - t0 >= minHold) && (now - lastMoveMs >= 12)) {
// // //                 break;
// // //             }
// // //             delay(1);
// // //         }
// // //     }
// // //     stopMotor();
// // // }

// // // void movePID(int base, int steer) {
// // //     int L = base + steer, R = base - steer;
// // //     if (steer >  BOOST_THRESHOLD) L += BOOST;
// // //     if (steer < -BOOST_THRESHOLD) R += BOOST;
// // //     move(constrain(L, -255, 255), constrain(R, -255, 255));
// // // }

// // // inline void driveSharpTurn(int dir) {
// // //     // dir > 0: RE PHAI theo mui xe (banh trai tien, banh phai lui)
// // //     // dir < 0: RE TRAI theo mui xe (banh trai lui, banh phai tien)
// // //     if (dir > 0) move(TURN_BASE + TURN_OUT, TURN_BASE - TURN_IN);
// // //     else         move(TURN_BASE - TURN_IN, TURN_BASE + TURN_OUT);
// // // }

// // // // ==================== BLE SEND ====================
// // // void bleSend(const char* msg) {
// // //     if (deviceConnected && pTx) {
// // //         pTx->setValue((uint8_t*)msg, strlen(msg));
// // //         pTx->notify();
// // //     }
// // //     Serial.println(msg);
// // // }

// // // void sendStatus() {
// // //     if (!deviceConnected || millis() - lastStatusTime < STATUS_INTERVAL) return;
// // //     lastStatusTime = millis();
// // //     char pat[NS + 1];
// // //     for (int i = 0; i < NS; i++) pat[i] = (pattern & (1 << i)) ? '1' : '0';
// // //     pat[NS] = 0;
// // //     char buf[256];
// // //     snprintf(buf, sizeof(buf),
// // //         "{\"mode\":\"%s\",\"case\":\"%s\",\"pos\":%u,\"pat\":\"%s\",\"spd\":%d,\"err\":%d,\"cal\":%d,"
// // //         "\"c1\":%d,\"c2\":%d,\"v\":%d,\"t\":%lu}",
// // //         currentModeName, currentCaseName, lastLinePos, pat,
// // //         speed_dk, lastError, calibrating ? 2 : (calibrated ? 1 : 0), cnt1, cnt2,
// // //         (int)curV, timing ? (millis() - runStartMs) : runElapsedMs);
// // //     pTx->setValue((uint8_t*)buf, strlen(buf));
// // //     pTx->notify();
// // // }

// // // // ==================== ACTIONS ====================
// // // void startCalibration() {
// // //     runLine = false; testMode = false; stopTimer(); stopMotor();
// // //     calibrated = false; calibrating = true; calEnd = millis() + CAL_TIME;
// // //     resetCal(); pattern = 0;
// // //     currentModeName = "CALIB"; currentCaseName = "CALIBRATING";
// // //     beep(1, 150, 0);
// // //     bleSend("LOG:CAL_START");
// // // }

// // // void startAutoRun() {
// // //     if (!calibrated) {
// // //         beep(2, 150, 100);
// // //         bleSend("LOG:NEED_CAL");
// // //         return;
// // //     }
// // //     testMode = false; runLine = true;
// // //     lastError = 0; lastE = 0; integral = 0; dFiltered = 0;
// // //     lastPidUs = 0; lastLinePos = SETPOINT;
// // //     pulseLeft = pulseRight = 0;
// // //     cnt1 = 0; cnt2 = 0;
// // //     lostNow = false; gapStraight = false;
// // //     blackLatched = false; fullBlackFrames = 0;
// // //     activeTurnDir = 0; lastEdgeMs = 0; turnHoldUntil = 0;

// // //     inStartBox = true;
// // //     sawStartBlack = false;
// // //     startCenteredOnce = false;
// // //     lastBoxSide = 0;
// // //     startLineConfirmMs = 0;
// // //     startWhiteStartMs = 0;
// // //     postBoxTurnEnableAt = 0;

// // //     roundStage = ENABLE_ROUNDABOUT_START ? RND_ARM_J1 : RND_DONE;
// // //     roundStageMs = millis();
// // //     j1BranchSide = 0;
// // //     circleTurnDir = 1;
// // //     j1LeftFrames = 0;
// // //     j1RightFrames = 0;
// // //     stageConfirmFrames = 0;
// // //     stageWhiteFrames = 0;
// // //     arcSettleFrames = 0;

// // //     curV = min(START_BOX_SPEED, min(SPEED_MIN, BASE));
// // //     armStopAt = millis() + START_GRACE_MS;
// // //     startTimer();
// // //     currentModeName = "AUTO"; currentCaseName = "START_BOX";
// // //     beep(1, 120, 0);
// // //     bleSend("LOG:AUTO_START");
// // // }

// // // // ==================== BUTTON IO35 ====================
// // // bool btnLastRaw = false, btnStable = false, btnLongHandled = false;
// // // unsigned long btnDebounceMs = 0, btnPressStartMs = 0;

// // // void updateButton() {
// // //     unsigned long now = millis();
// // //     bool pressed = (digitalRead(BTN_PIN) == BTN_PRESSED_LEVEL);

// // //     if (pressed != btnLastRaw) {
// // //         btnLastRaw = pressed;
// // //         btnDebounceMs = now;
// // //     }
// // //     unsigned long debounceReq = runLine ? 250 : BTN_DEBOUNCE_MS;
// // //     if (now - btnDebounceMs < debounceReq) return;

// // //     if (pressed != btnStable) {
// // //         btnStable = pressed;
// // //         if (btnStable) {
// // //             btnPressStartMs = now;
// // //             btnLongHandled = false;
// // //             if (runLine && !calibrating) {
// // //                 btnLongHandled = true;
// // //                 hardBrakeStop();
// // //                 runLine = false; stopTimer();
// // //                 currentModeName = "MANUAL"; currentCaseName = "IDLE";
// // //                 beep(1, 200, 0);
// // //                 bleSend("LOG:MODE_MANUAL");
// // //             }
// // //         } else {
// // //             if (!btnLongHandled && !calibrating && !runLine) {
// // //                 if (!calibrated) startCalibration();
// // //                 else             startAutoRun();
// // //             }
// // //         }
// // //     } else if (btnStable && !btnLongHandled && !calibrating && !runLine && calibrated) {
// // //         if (now - btnPressStartMs >= BTN_RECAL_HOLD_MS) {
// // //             btnLongHandled = true;
// // //             startCalibration();
// // //         }
// // //     }
// // // }

// // // void finishRun() {
// // //     stopTimer();
// // //     hardBrakeStop();
// // //     runLine = false;
// // //     currentModeName = "DONE"; currentCaseName = "FINISH";
// // //     beep(2, 200, 100);
// // //     char b[40]; snprintf(b, sizeof(b), "LOG:FINISH %.2fs", runElapsedMs / 1000.0f);
// // //     bleSend(b);
// // // }

// // // // ==================== LINE FOLLOW PID ====================
// // // void lineFollowPID() {
// // //     int cnt = docSensor();
// // //     bool edgeL = (pattern & 0x03) != 0;                // mat 0, 1 (bien TRAI)
// // //     bool edgeR = (pattern & (0x03 << (NS - 2))) != 0;  // mat 10, 11 (bien PHAI)
// // //     bool centerOn = (pattern & 0x00F0) != 0;           // 4 mat chinh giua doi xung: mat 4, 5, 6, 7
// // //     bool mid6On   = (pattern & 0x01F8) != 0;           // 6 mat thang giua: mat 3, 4, 5, 6, 7, 8
// // //     int  mid6Cnt  = __builtin_popcount(pattern & 0x01F8);
// // //     bool leftOuter3  = (pattern & 0x0007) != 0;        // 3 mat ngoai ben TRAI: 0, 1, 2
// // //     bool rightOuter3 = (pattern & 0x0E00) != 0;        // 3 mat ngoai ben PHAI: 9, 10, 11
// // //     unsigned long nowMs = millis();
// // //     int vmin = min(SPEED_MIN, BASE);

// // //     int firstIdx = -1, lastIdx = -1;
// // //     int leftHalfCnt = 0, rightHalfCnt = 0;
// // //     for (int i = 0; i < NS; i++) {
// // //         if (pattern & (1 << i)) {
// // //             if (firstIdx < 0) firstIdx = i;
// // //             lastIdx = i;
// // //             if (i < 6) leftHalfCnt++;
// // //             else       rightHalfCnt++;
// // //         }
// // //     }
// // //     int span = (cnt > 0) ? (lastIdx - firstIdx + 1) : 0;

// // //     // Neu bi loa bat trong khoi den xuat phat lam thoat som khi van con trong o START
// // //     if (!inStartBox && roundStage == RND_ARM_J1 && cnt1 == 0 && (nowMs - runStartMs < START_GRACE_MS) &&
// // //         (cnt > TURN_MAX_COUNT || (cnt >= 7 && edgeL && edgeR))) {
// // //         inStartBox = true;
// // //         sawStartBlack = true;
// // //         startLineConfirmMs = 0;
// // //         startWhiteStartMs = 0;
// // //         activeTurnDir = 0;
// // //     }

// // //     // ==================== CHAY THANG THOAT KHOI O VUONG DEN -> BAM PID ====================
// // //     if (inStartBox) {
// // //         armStopAt = max(armStopAt, nowMs + POST_BOX_STOP_GUARD_MS);
// // //         fullBlackFrames = 0;
// // //         blackLatched = false;
// // //         activeTurnDir = 0;
// // //         lostNow = false;
// // //         lastEdgeMs = 0;
// // //         roundStageMs = nowMs;

// // //         if (cnt > 0 && (firstIdx + lastIdx) != (NS - 1)) {
// // //             lastBoxSide = ((firstIdx + lastIdx) > (NS - 1)) ? 1 : -1;
// // //         }

// // //         if (cnt >= START_BOX_BLOCK_CNT || span >= START_BOX_BLOCK_CNT) {
// // //             sawStartBlack = true;
// // //             startLineConfirmMs = 0;
// // //             startWhiteStartMs = 0;
// // //         }

// // //         bool clearBothEdges = !edgeL && !edgeR;
// // //         bool clearOuterTips = ((pattern & 1) == 0) && ((pattern & (1 << (NS - 1))) == 0);
// // //         unsigned long confirmReqMs = 0;
// // //         bool singleLineCandidate = false;

// // //         if (clearBothEdges && cnt >= 1 && cnt <= 5 && span <= 5) {
// // //             singleLineCandidate = true;
// // //             confirmReqMs = START_EXIT_CENTER_MS;
// // //         } else if (clearOuterTips && cnt >= 1 && cnt <= 4 && span <= 4) {
// // //             singleLineCandidate = true;
// // //             confirmReqMs = START_EXIT_NEAR_EDGE_MS;
// // //         } else if (cnt >= 1 && cnt <= 4 && span <= 4) {
// // //             singleLineCandidate = true;
// // //             confirmReqMs = START_EXIT_EXTREME_MS;
// // //         }

// // //         bool exitedToLine = false;
// // //         bool slidOffToWhite = false;

// // //         if (singleLineCandidate) {
// // //             startWhiteStartMs = 0;
// // //             if (startLineConfirmMs == 0) startLineConfirmMs = nowMs;
// // //             if (nowMs - startLineConfirmMs >= confirmReqMs) {
// // //                 exitedToLine = true;
// // //             }
// // //         } else {
// // //             startLineConfirmMs = 0;
// // //             if (cnt == 0 && sawStartBlack) {
// // //                 if (startWhiteStartMs == 0) startWhiteStartMs = nowMs;
// // //                 else if (nowMs - startWhiteStartMs >= START_WHITE_LOST_MS) {
// // //                     slidOffToWhite = true;
// // //                 }
// // //             } else {
// // //                 startWhiteStartMs = 0;
// // //             }
// // //         }

// // //         if (!exitedToLine && !slidOffToWhite && (nowMs - runStartMs >= START_BOX_MAX_MS)) {
// // //             exitedToLine = true;
// // //         }

// // //         if (!exitedToLine && !slidOffToWhite) {
// // //             if (!singleLineCandidate) lastLinePos = SETPOINT;
// // //             lastError = 0;
// // //             lastE = 0;
// // //             integral = 0;
// // //             dFiltered = 0;

// // //             unsigned long us = micros();
// // //             float dtn = lastPidUs ? (us - lastPidUs) / (float)REF_DT_US : 1.0f;
// // //             lastPidUs = us;
// // //             dtn = constrain(dtn, 0.1f, 4.0f);

// // //             float boxTarget = (float)min(BASE, START_BOX_SPEED);
// // //             if (boxTarget < curV) curV = boxTarget;
// // //             else                  curV = min(boxTarget, curV + ACCEL_PER_S * dtn * REF_DT_US / 1e6f);

// // //             currentCaseName = "START_BOX";
// // //             move((int)curV, (int)curV);
// // //             return;
// // //         }

// // //         inStartBox = false;
// // //         postBoxTurnEnableAt = nowMs + POST_BOX_TURN_GUARD_MS;
// // //         roundStageMs = nowMs;
// // //         integral = 0;
// // //         dFiltered = 0;
// // //         lastPidUs = 0;

// // //         if (slidOffToWhite) {
// // //             startCenteredOnce = false;
// // //             lastLinePos = (lastBoxSide >= 0) ? POS[NS - 1] : POS[0];
// // //             lastError = (int)lastLinePos - SETPOINT;
// // //             lastE = (float)lastError;
// // //             lastEdgeMs = nowMs;
// // //             bleSend("LOG:BOX_RECOVER");
// // //         } else {
// // //             startCenteredOnce = (centerOn && !edgeL && !edgeR);
// // //             float initE = (cnt > 0) ? ((float)lastLinePos - SETPOINT) : 0.0f;
// // //             lastE = initE;
// // //             lastError = (int)initE;
// // //             lastEdgeMs = 0;
// // //             bleSend("LOG:BOX_EXIT_PID");
// // //         }
// // //     }

// // //     // ==================== XU LY GOC CONG DAU -> RE VONG TRON -> BO QUA VONG CONG PHIA TREN ====================
// // //     unsigned int pidPos = lastLinePos;
// // //     bool smoothResetPid = false;
// // //     const char* stageCaseOverride = nullptr;

// // //     if (roundStage < RND_DONE) {
// // //         armStopAt = max(armStopAt, nowMs + 1000);
// // //         fullBlackFrames = 0;
// // //         blackLatched = false;
// // //         activeTurnDir = 0;

// // //         int straightSpd = min(BASE, ROUND_APPROACH_SPEED);

// // //         if (roundStage == RND_ARM_J1) {
// // //             stageCaseOverride = "RND_ARM_J1";
// // //             bool singleStraight = (cnt >= 1 && cnt <= 3 && span <= 3 && !leftOuter3 && !rightOuter3) ||
// // //                                   (cnt == 4 && firstIdx == 4 && lastIdx == 7);
// // //             if (singleStraight) {
// // //                 if (nowMs - roundStageMs >= ROUND_ARM_LINE_MS) {
// // //                     roundStage = RND_WAIT_J1;
// // //                     roundStageMs = nowMs;
// // //                     j1LeftFrames = 0;
// // //                     j1RightFrames = 0;
// // //                 }
// // //             } else {
// // //                 roundStageMs = nowMs;
// // //             }
// // //         }
// // //         else if (roundStage == RND_WAIT_J1) {
// // //             stageCaseOverride = "RND_WAIT_J1";

// // //             // Nhan dien goc cong dau ngay khi vach phinh lech 1 ben (VD: 001111100000 ben Trai hoac 000001111100 ben Phai)
// // //             bool wideOrSplit = (cnt >= 4 && span >= 4) || (cnt >= 3 && span >= 5);
// // //             bool candLeft  = wideOrSplit &&
// // //                              !rightOuter3 &&
// // //                              (firstIdx >= 0 && firstIdx <= 3) &&
// // //                              (lastIdx >= 4 && lastIdx <= 8) &&
// // //                              ((firstIdx + lastIdx) <= 9) &&
// // //                              (leftHalfCnt > rightHalfCnt);
// // //             bool candRight = wideOrSplit &&
// // //                              !leftOuter3 &&
// // //                              (lastIdx >= 8 && lastIdx < NS) &&
// // //                              (firstIdx >= 3 && firstIdx <= 7) &&
// // //                              ((firstIdx + lastIdx) >= 13) &&
// // //                              (rightHalfCnt > leftHalfCnt);

// // //             if (candLeft) {
// // //                 j1LeftFrames++;
// // //                 j1RightFrames = 0;
// // //                 if (j1LeftFrames >= 2) {
// // //                     j1BranchSide  = -1; // Goc cong dau lech TRAI (001111100000)
// // //                     circleTurnDir =  1; // -> Len vach ngang tren RE PHAI theo mui xe
// // //                     roundStage = RND_PASS_J1;
// // //                     roundStageMs = nowMs;
// // //                     stageConfirmFrames = 0;
// // //                     beep(1, 45, 0);
// // //                     bleSend("LOG:J1_LEFT->TURN_R");
// // //                 }
// // //                 lastPidUs = micros();
// // //                 currentCaseName = "PASS_J1_L";
// // //                 move(straightSpd, straightSpd);
// // //                 return;
// // //             }
// // //             else if (candRight) {
// // //                 j1RightFrames++;
// // //                 j1LeftFrames = 0;
// // //                 if (j1RightFrames >= 2) {
// // //                     j1BranchSide  =  1; // Goc cong dau lech PHAI (000001111100)
// // //                     circleTurnDir = -1; // -> Len vach ngang tren RE TRAI theo mui xe
// // //                     roundStage = RND_PASS_J1;
// // //                     roundStageMs = nowMs;
// // //                     stageConfirmFrames = 0;
// // //                     beep(1, 45, 0);
// // //                     bleSend("LOG:J1_RIGHT->TURN_L");
// // //                 }
// // //                 lastPidUs = micros();
// // //                 currentCaseName = "PASS_J1_R";
// // //                 move(straightSpd, straightSpd);
// // //                 return;
// // //             }
// // //             else {
// // //                 if (j1LeftFrames > 0 || j1RightFrames > 0) smoothResetPid = true;
// // //                 j1LeftFrames = 0;
// // //                 j1RightFrames = 0;

// // //                 if ((nowMs - roundStageMs >= 250) && edgeL && edgeR && cnt >= 6) {
// // //                     circleTurnDir = (j1BranchSide != 0) ? -j1BranchSide : 1;
// // //                     if (ROUND_PRE_BRAKE_MS > 0) {
// // //                         move(-ROUND_PRE_BRAKE_PWM, -ROUND_PRE_BRAKE_PWM);
// // //                         delay(ROUND_PRE_BRAKE_MS);
// // //                     }
// // //                     roundStage = RND_TURN_CIRCLE;
// // //                     roundStageMs = millis();
// // //                     bleSend("LOG:T_FALLBACK_TURN");
// // //                     currentCaseName = (circleTurnDir > 0) ? "CIRCLE_TURN_R" : "CIRCLE_TURN_L";
// // //                     driveSharpTurn(circleTurnDir);
// // //                     return;
// // //                 }
// // //             }
// // //         }
// // //         else if (roundStage == RND_PASS_J1) {
// // //             unsigned long el = nowMs - roundStageMs;
// // //             bool cleanSingleLine = (cnt >= 1 && cnt <= 3 && span <= 3) && !leftOuter3 && !rightOuter3;
// // //             if (el >= ROUND_J1_HOLD_MS && cleanSingleLine) {
// // //                 stageConfirmFrames++;
// // //             } else {
// // //                 stageConfirmFrames = 0;
// // //             }

// // //             if (stageConfirmFrames < 3 && el < ROUND_J1_MAX_MS) {
// // //                 lastPidUs = micros();
// // //                 currentCaseName = (j1BranchSide < 0) ? "PASS_J1_L" : "PASS_J1_R";
// // //                 move(straightSpd, straightSpd);
// // //                 return;
// // //             }

// // //             roundStage = RND_WAIT_CIRCLE_T;
// // //             roundStageMs = nowMs;
// // //             stageConfirmFrames = 0;
// // //             stageWhiteFrames = 0;
// // //             lostNow = false;
// // //             lastEdgeMs = 0;
// // //             curV = (float)straightSpd;
// // //             smoothResetPid = true;
// // //             bleSend("LOG:PASS_J1_STRAIGHT");
// // //             stageCaseOverride = (circleTurnDir > 0) ? "TO_TOP_BAR_R" : "TO_TOP_BAR_L";
// // //         }

// // //         if (roundStage == RND_WAIT_CIRCLE_T) {
// // //             stageCaseOverride = (circleTurnDir > 0) ? "TO_TOP_BAR_R" : "TO_TOP_BAR_L";
// // //             unsigned long el = nowMs - roundStageMs;

// // //             if (el >= ROUND_T_ARM_MS) {
// // //                 bool hitTopBar = (mid6Cnt >= 4) ||
// // //                                  (cnt >= 4 && span >= 4 && mid6Cnt >= 3) ||
// // //                                  (cnt >= 5 && span >= 5) ||
// // //                                  (edgeL && edgeR && cnt >= 4);

// // //                 if (hitTopBar) {
// // //                     stageConfirmFrames++;
// // //                     stageWhiteFrames = 0;
// // //                     if (stageConfirmFrames >= 2) {
// // //                         if (ROUND_PRE_BRAKE_MS > 0) {
// // //                             move(-ROUND_PRE_BRAKE_PWM, -ROUND_PRE_BRAKE_PWM);
// // //                             delay(ROUND_PRE_BRAKE_MS);
// // //                         }
// // //                         roundStage = RND_TURN_CIRCLE;
// // //                         roundStageMs = millis();
// // //                         stageConfirmFrames = 0;
// // //                         integral = 0; dFiltered = 0;
// // //                         beep(1, 60, 0);
// // //                         bleSend((circleTurnDir > 0) ? "LOG:TOP_BAR_TURN_R" : "LOG:TOP_BAR_TURN_L");
// // //                         currentCaseName = (circleTurnDir > 0) ? "CIRCLE_TURN_R" : "CIRCLE_TURN_L";
// // //                         driveSharpTurn(circleTurnDir);
// // //                         return;
// // //                     }
// // //                     lastPidUs = micros();
// // //                     currentCaseName = "TOP_BAR_HIT";
// // //                     move(straightSpd, straightSpd);
// // //                     return;
// // //                 } else {
// // //                     if (stageConfirmFrames > 0) smoothResetPid = true;
// // //                     stageConfirmFrames = 0;

// // //                     if (cnt == 0 && el >= 80) {
// // //                         stageWhiteFrames++;
// // //                         if (stageWhiteFrames >= 4) {
// // //                             roundStage = RND_TURN_CIRCLE;
// // //                             roundStageMs = nowMs;
// // //                             stageWhiteFrames = 0;
// // //                             integral = 0; dFiltered = 0;
// // //                             beep(1, 60, 0);
// // //                             bleSend((circleTurnDir > 0) ? "LOG:TOP_WHITE_TURN_R" : "LOG:TOP_WHITE_TURN_L");
// // //                             currentCaseName = (circleTurnDir > 0) ? "CIRCLE_TURN_R" : "CIRCLE_TURN_L";
// // //                             driveSharpTurn(circleTurnDir);
// // //                             return;
// // //                         }
// // //                         lastPidUs = micros();
// // //                         currentCaseName = "TOP_CONFIRM_W";
// // //                         move(60, 60);
// // //                         return;
// // //                     }
// // //                     stageWhiteFrames = 0;
// // //                 }
// // //             }
// // //         }
// // //         else if (roundStage == RND_TURN_CIRCLE) {
// // //             // Xoay vao vong tron theo circleTurnDir (+1 = Re Phai, -1 = Re Trai) -> chuyen sang RND_ARC_WAIT_MID
// // //             unsigned long el = nowMs - roundStageMs;
// // //             bool caughtArc = (el >= ROUND_TURN_HOLD_MS) &&
// // //                              mid6On && !edgeL && !edgeR &&
// // //                              (cnt >= 1 && cnt <= 5 && span <= 5);
// // //             if (!caughtArc && el < ROUND_TURN_MAX_MS) {
// // //                 currentCaseName = (circleTurnDir > 0) ? "CIRCLE_TURN_R" : "CIRCLE_TURN_L";
// // //                 driveSharpTurn(circleTurnDir);
// // //                 return;
// // //             }
// // //             if (ROUND_TURN_BRAKE_MS > 0) {
// // //                 if (circleTurnDir > 0) move(-60, 60);
// // //                 else                   move(60, -60);
// // //                 delay(ROUND_TURN_BRAKE_MS);
// // //                 cnt = docSensor();
// // //                 edgeL = (pattern & 0x03) != 0;
// // //                 edgeR = (pattern & (0x03 << (NS - 2))) != 0;
// // //                 centerOn = (pattern & 0x00F0) != 0;
// // //                 mid6On   = (pattern & 0x01F8) != 0;
// // //                 firstIdx = -1; lastIdx = -1;
// // //                 leftHalfCnt = 0; rightHalfCnt = 0;
// // //                 for (int i = 0; i < NS; i++) {
// // //                     if (pattern & (1 << i)) {
// // //                         if (firstIdx < 0) firstIdx = i;
// // //                         lastIdx = i;
// // //                         if (i < 6) leftHalfCnt++; else rightHalfCnt++;
// // //                     }
// // //                 }
// // //                 span = (cnt > 0) ? (lastIdx - firstIdx + 1) : 0;
// // //             }
// // //             nowMs = millis();
// // //             roundStage = RND_ARC_WAIT_MID;
// // //             roundStageMs = nowMs;
// // //             stageConfirmFrames = 0;
// // //             stageWhiteFrames = 0;
// // //             arcSettleFrames = 0;
// // //             startCenteredOnce = true;
// // //             lostNow = false;
// // //             lastEdgeMs = (cnt == 0) ? nowMs : 0;
// // //             lastError = (cnt > 0) ? ((int)lastLinePos - SETPOINT) : ((circleTurnDir > 0) ? -3000 : 3000);
// // //             lastE = (float)lastError;
// // //             lastPidUs = 0;
// // //             curV = (float)min(BASE, ROUND_ARC_SPEED);
// // //             smoothResetPid = true;
// // //             pidPos = lastLinePos;
// // //             bleSend("LOG:CIRCLE_ARC_START");
// // //             stageCaseOverride = "CIRCLE_ARC";
// // //         }
// // //         else if (roundStage == RND_ARC_WAIT_MID) {
// // //             // Dang om vong cong: canh nhanh vong cong phia tren (VD: 000001110000 pos~6095 hoac 000001111000 pos~6519)
// // //             stageCaseOverride = "CIRCLE_ARC";
// // //             unsigned long el = nowMs - roundStageMs;

// // //             bool huggingInner = (cnt >= 1 && cnt <= 3) &&
// // //                                 ((circleTurnDir > 0) ? (lastIdx <= 6 && lastLinePos <= 5650)
// // //                                                      : (firstIdx >= 5 && lastLinePos >= 5350));
// // //             if (huggingInner && arcSettleFrames < 100) arcSettleFrames++;

// // //             if (cnt == 0) {
// // //                 // Neu di qua xa tren cung tron (> 700ms) ma gap trang tinh -> da toi dau mut cuoi cung tron, tra ve PID
// // //                 if (el >= 700) {
// // //                     stageWhiteFrames++;
// // //                     if (stageWhiteFrames >= 4) {
// // //                         roundStage = RND_DONE;
// // //                         roundStageMs = nowMs;
// // //                         postBoxTurnEnableAt = nowMs;
// // //                         lastEdgeMs = nowMs;
// // //                         lastError = (circleTurnDir > 0) ? 3500 : -3500;
// // //                         lastE = (float)lastError;
// // //                         bleSend("LOG:ARC_WHITE_DONE");
// // //                     }
// // //                 } else {
// // //                     lastError = (circleTurnDir > 0) ? -3000 : 3000;
// // //                 }
// // //             } else {
// // //                 stageWhiteFrames = 0;
// // //             }

// // //             bool armedForMid = (el >= ROUND_ARC_SETTLE_MS) && (arcSettleFrames >= 3 || el >= 200);
// // //             if (armedForMid && cnt > 0) {
// // //                 bool sawMidBranch = false;
// // //                 if (circleTurnDir > 0) {
// // //                     // Cung tron uon TRAI (mat <= 5), nhanh cong phia tren nho ra PHAI (000001110000 / 000001111000 / mat 8..11)
// // //                     sawMidBranch = !edgeL && (
// // //                         (cnt >= 3 && firstIdx >= 5 && lastIdx >= 7 && lastLinePos >= 5850) ||
// // //                         (cnt >= 4 && span >= 4 && lastIdx >= 7) ||
// // //                         (cnt >= 2 && span >= 4 && lastIdx >= 7) ||
// // //                         ((pattern & 0x0F00) != 0)
// // //                     );
// // //                 } else {
// // //                     // Doi xung: cung tron uon PHAI (mat >= 6), nhanh cong nho ra TRAI (000011100000 / 000111100000 / mat 0..3)
// // //                     sawMidBranch = !edgeR && (
// // //                         (cnt >= 3 && lastIdx <= 6 && firstIdx <= 4 && lastLinePos <= 5150) ||
// // //                         (cnt >= 4 && span >= 4 && firstIdx <= 4) ||
// // //                         (cnt >= 2 && span >= 4 && firstIdx <= 4) ||
// // //                         ((pattern & 0x000F) != 0)
// // //                     );
// // //                 }

// // //                 if (sawMidBranch) {
// // //                     if (stageConfirmFrames == 0) smoothResetPid = true;
// // //                     // Che han cum mat phia ngoai (6..11 hoac 0..5), giu xe tiep tuc bam muot theo mep trong vong tron!
// // //                     if (circleTurnDir > 0) {
// // //                         pidPos = min(getMaskedLinePos(0, 5, 4400), 4700U);
// // //                     } else {
// // //                         pidPos = max(getMaskedLinePos(6, 11, 6600), 6300U);
// // //                     }
// // //                     stageCaseOverride = "SKIP_UPPER_ARC";
// // //                     stageConfirmFrames++;
// // //                     if (stageConfirmFrames >= 2) {
// // //                         roundStage = RND_ARC_PASS_MID;
// // //                         roundStageMs = nowMs;
// // //                         stageConfirmFrames = 0;
// // //                         beep(1, 45, 0);
// // //                         bleSend("LOG:SKIP_UPPER_ARC");
// // //                     }
// // //                 } else {
// // //                     if (stageConfirmFrames > 0) smoothResetPid = true;
// // //                     stageConfirmFrames = 0;
// // //                 }
// // //             }
// // //         }
// // //         else if (roundStage == RND_ARC_PASS_MID) {
// // //             // Tiep tuc che mat phia ngoai de bo qua nhanh vong cong phia tren va chay tiep tren cung tron
// // //             stageCaseOverride = "SKIP_UPPER_ARC";
// // //             unsigned long el = nowMs - roundStageMs;

// // //             if (circleTurnDir > 0) {
// // //                 pidPos = min(getMaskedLinePos(0, 5, 4400), 4700U);
// // //             } else {
// // //                 pidPos = max(getMaskedLinePos(6, 11, 6600), 6300U);
// // //             }

// // //             bool cleanInnerArc = (cnt >= 1 && cnt <= 3 && span <= 3) &&
// // //                                  ((circleTurnDir > 0) ? (lastIdx <= 6 && lastLinePos <= 5750)
// // //                                                       : (firstIdx >= 5 && lastLinePos >= 5250));
// // //             if (el >= ROUND_MID_HOLD_MS && cleanInnerArc) {
// // //                 stageConfirmFrames++;
// // //             } else {
// // //                 stageConfirmFrames = 0;
// // //             }

// // //             if (stageConfirmFrames >= 4 || el >= ROUND_MID_MAX_MS) {
// // //                 roundStage = RND_DONE;
// // //                 roundStageMs = nowMs;
// // //                 postBoxTurnEnableAt = nowMs + 60;
// // //                 armStopAt = max(armStopAt, nowMs + 800);
// // //                 startCenteredOnce = true;
// // //                 lostNow = false;
// // //                 lastEdgeMs = 0;
// // //                 pidPos = lastLinePos;
// // //                 smoothResetPid = true;
// // //                 bleSend("LOG:UPPER_ARC_PASSED");
// // //             }
// // //         }
// // //     }

// // //     if (edgeL || edgeR) lastEdgeMs = nowMs;
// // //     if (centerOn && !edgeL && !edgeR) startCenteredOnce = true;

// // //     if (roundStage >= RND_DONE && cnt >= STOP_MIN_CNT && edgeL && edgeR) {
// // //         if (fullBlackFrames < 1000) fullBlackFrames++;
// // //         if (fullBlackFrames >= END_CONFIRM && !blackLatched) {
// // //             blackLatched = true;
// // //             if (nowMs >= armStopAt) {
// // //                 cnt2++;
// // //                 if (cnt2 >= STOP_CNT2) { finishRun(); return; }
// // //             }
// // //         }
// // //     } else {
// // //         fullBlackFrames = 0;
// // //         blackLatched = false;
// // //     }

// // //     if (activeTurnDir != 0) {
// // //         if (cnt > 0 && (nowMs < turnHoldUntil || !mid6On)) {
// // //             if (activeTurnDir > 0) {
// // //                 currentCaseName = "TURN_RIGHT";
// // //                 move(TURN_BASE + TURN_OUT, TURN_BASE - TURN_IN);
// // //             } else {
// // //                 currentCaseName = "TURN_LEFT";
// // //                 move(TURN_BASE - TURN_IN, TURN_BASE + TURN_OUT);
// // //             }
// // //             return;
// // //         }
// // //         activeTurnDir = 0;
// // //     }

// // //     if (cnt == 0) {
// // //         integral = 0;
// // //         dFiltered = 0;
// // //         if (!lostNow) {
// // //             lostNow = true;
// // //             gapStart = nowMs;
// // //             bool recentEdge = (lastEdgeMs != 0) && (nowMs - lastEdgeMs <= GAP_EDGE_GUARD_MS);
// // //             gapStraight = (roundStage >= RND_DONE) &&
// // //                           (nowMs >= postBoxTurnEnableAt) &&
// // //                           !recentEdge &&
// // //                           (abs(lastError) <= GAP_ERR_MAX) &&
// // //                           (cnt1 < GAP_MAX_COUNT);
// // //             if (gapStraight) cnt1++;
// // //             gapSpeed = constrain((int)curV, vmin, BASE);
// // //         }
// // //         if (gapStraight && (GAP_TIMEOUT_MS == 0 || nowMs - gapStart < GAP_TIMEOUT_MS)) {
// // //             currentCaseName = "GAP_STRAIGHT";
// // //             move(gapSpeed, gapSpeed);
// // //             return;
// // //         }
// // //         currentCaseName = "LOST_LINE";
// // //         if (lastError >= 0) move(LOST_TURN_SPEED, -LOST_TURN_SPEED);
// // //         else                move(-LOST_TURN_SPEED, LOST_TURN_SPEED);
// // //         return;
// // //     }
// // //     bool reacquired = lostNow;
// // //     lostNow = false;

// // //     bool allowTurn90 = (roundStage >= RND_DONE) &&
// // //                        (nowMs >= postBoxTurnEnableAt) &&
// // //                        (startCenteredOnce || (nowMs >= postBoxTurnEnableAt + 250));

// // //     if (allowTurn90 && (pattern & (1 << (NS - 1))) && !(pattern & 1) && cnt >= 3 && cnt <= TURN_MAX_COUNT) {
// // //         currentCaseName = "TURN_RIGHT";
// // //         activeTurnDir = 1;
// // //         turnHoldUntil = nowMs + TURN_HOLD_MS;
// // //         lastLinePos = POS[NS - 1];
// // //         lastError = POS[NS - 1] - SETPOINT;
// // //         lastE = (float)lastError;
// // //         curV = min(curV, (float)vmin);
// // //         integral = 0; dFiltered = 0;
// // //         move(TURN_BASE + TURN_OUT, TURN_BASE - TURN_IN);
// // //         return;
// // //     }
// // //     if (allowTurn90 && (pattern & 1) && !(pattern & (1 << (NS - 1))) && cnt >= 3 && cnt <= TURN_MAX_COUNT) {
// // //         currentCaseName = "TURN_LEFT";
// // //         activeTurnDir = -1;
// // //         turnHoldUntil = nowMs + TURN_HOLD_MS;
// // //         lastLinePos = POS[0];
// // //         lastError = POS[0] - SETPOINT;
// // //         lastE = (float)lastError;
// // //         curV = min(curV, (float)vmin);
// // //         integral = 0; dFiltered = 0;
// // //         move(TURN_BASE - TURN_IN, TURN_BASE + TURN_OUT);
// // //         return;
// // //     }

// // //     float e = (float)pidPos - SETPOINT;
// // //     unsigned long us = micros();
// // //     float dtn = lastPidUs ? (us - lastPidUs) / (float)REF_DT_US : 1.0f;
// // //     lastPidUs = us;
// // //     dtn = constrain(dtn, 0.1f, 4.0f);
// // //     if (reacquired || smoothResetPid) {
// // //         lastE = e;
// // //         integral = 0;
// // //         dFiltered = 0;
// // //     }

// // //     if (fabsf(e) < I_ZONE) integral += e * dtn;
// // //     else                   integral *= 0.8f;
// // //     if (KI > 0) integral = constrain(integral, -I_MAX / KI, I_MAX / KI);
// // //     else        integral = 0;

// // //     float rawD = KD * (e - lastE) / dtn;
// // //     dFiltered = dFiltered + D_FILTER * (rawD - dFiltered);
// // //     float steer = KP * e + KI * integral + dFiltered;
// // //     lastE = e;
// // //     lastError = (int)e;

// // //     int activeBase = BASE;
// // //     int activeMin  = vmin;
// // //     if (roundStage < RND_DONE) {
// // //         int cap = (roundStage >= RND_ARC_WAIT_MID) ? ROUND_ARC_SPEED : ROUND_APPROACH_SPEED;
// // //         activeBase = min(BASE, cap + 10);
// // //         activeMin  = min(vmin, max(65, cap - 10));
// // //     }

// // //     float f = max(fabsf(e) / (float)max(E_SLOW, 1), fabsf(steer) / STEER_SLOW);
// // //     f = constrain(f, 0.0f, 1.0f);
// // //     float target = activeBase - (activeBase - activeMin) * f;
// // //     if (target < curV) curV = target;
// // //     else               curV = min(target, curV + ACCEL_PER_S * dtn * REF_DT_US / 1e6f);

// // //     if (stageCaseOverride != nullptr) {
// // //         currentCaseName = stageCaseOverride;
// // //     } else {
// // //         currentCaseName = (f < 0.2f) ? "STRAIGHT_FAST" : (f > 0.6f ? "SLOW_CURVE" : "PID_FOLLOW");
// // //     }
// // //     movePID((int)curV, (int)steer);
// // // }

// // // // ==================== BLE COMMANDS ====================
// // // void handleBLECommand(String cmd) {
// // //     cmd.trim();
// // //     if (cmd.startsWith("SPD:")) {
// // //         speed_dk = constrain(cmd.substring(4).toInt(), 30, 255);
// // //         char b[30]; snprintf(b, sizeof(b), "LOG:SPD=%d", speed_dk); bleSend(b); return;
// // //     }
// // //     if (cmd == "TEST") {
// // //         testMode = !testMode; runLine = false; stopTimer(); stopMotor();
// // //         beep(1, 60, 0);
// // //         bleSend(testMode ? "LOG:TEST_ON" : "LOG:TEST_OFF"); return;
// // //     }
// // //     if (cmd == "CAL") {
// // //         startCalibration();
// // //         return;
// // //     }
// // //     if (cmd.startsWith("PID:")) {
// // //         float kp, kd, ki = KI; int base;
// // //         int n = sscanf(cmd.c_str() + 4, "%f,%f,%d,%f", &kp, &kd, &base, &ki);
// // //         if (n >= 3) {
// // //             KP = kp; KD = kd; BASE = constrain(base, 0, 255);
// // //             if (n == 4) KI = max(ki, 0.0f);
// // //             beep(1, 50, 0);
// // //             bleSend("LOG:PID_OK");
// // //         } else bleSend("LOG:PID_ERR");
// // //         return;
// // //     }
// // //     if (cmd.startsWith("PROF:")) {
// // //         int vm, es;
// // //         if (sscanf(cmd.c_str() + 5, "%d,%d", &vm, &es) == 2) {
// // //             SPEED_MIN = constrain(vm, 0, 255); E_SLOW = constrain(es, 200, 5500);
// // //             beep(1, 50, 0);
// // //             bleSend("LOG:PROF_OK");
// // //         } else bleSend("LOG:PROF_ERR");
// // //         return;
// // //     }
// // //     if (cmd.startsWith("TURN:")) {
// // //         int o, i, b;
// // //         int n = sscanf(cmd.c_str() + 5, "%d,%d,%d", &o, &i, &b);
// // //         if (n >= 2) {
// // //             TURN_OUT = constrain(o, 0, 255); TURN_IN = constrain(i, -255, 400);
// // //             if (n == 3) TURN_BASE = constrain(b, 0, 255);
// // //             beep(1, 50, 0);
// // //             bleSend("LOG:TURN_OK");
// // //         } else bleSend("LOG:TURN_ERR");
// // //         return;
// // //     }
// // //     if (cmd == "GETPID") {
// // //         char b[120];
// // //         snprintf(b, sizeof(b), "PID:%.4f,%.4f,%d,%d,%d,%.5f,%d,%d,%d",
// // //                  KP, KD, BASE, TURN_OUT, TURN_IN, KI, SPEED_MIN, E_SLOW, TURN_BASE);
// // //         bleSend(b); return;
// // //     }
// // //     if (cmd == "AUTO") {
// // //         startAutoRun();
// // //         return;
// // //     }
// // //     if (cmd == "MANUAL") {
// // //         if (runLine) hardBrakeStop();
// // //         else         stopMotor();
// // //         runLine = false; stopTimer();
// // //         currentModeName = "MANUAL"; currentCaseName = "IDLE";
// // //         beep(1, 100, 0);
// // //         bleSend("LOG:MODE_MANUAL"); return;
// // //     }
// // //     if (!runLine) {
// // //         if      (cmd == "FWD") move(speed_dk, speed_dk);
// // //         else if (cmd == "BWD") move(-speed_dk, -speed_dk);
// // //         else if (cmd == "LFT") move(-speed_dk, speed_dk);
// // //         else if (cmd == "RGT") move(speed_dk, -speed_dk);
// // //         else if (cmd == "STP") stopMotor();
// // //     }
// // // }

// // // class MyCallbacks : public BLECharacteristicCallbacks {
// // //     void onWrite(BLECharacteristic* c) override {
// // //         auto v = c->getValue();
// // //         if (v.length() > 0) {
// // //             portENTER_CRITICAL(&cmdMux);
// // //             pendingCmd = String(v.c_str());
// // //             hasPendingCmd = true;
// // //             portEXIT_CRITICAL(&cmdMux);
// // //         }
// // //     }
// // // };
// // // class MyServerCallbacks : public BLEServerCallbacks {
// // //     void onConnect(BLEServer*) override { deviceConnected = true; }
// // //     void onDisconnect(BLEServer*) override {
// // //         deviceConnected = false;
// // //         if (!runLine) stopMotor();
// // //         BLEDevice::startAdvertising();
// // //     }
// // // };

// // // // ==================== SETUP ====================
// // // void setup() {
// // //     Serial.begin(115200);

// // //     pinMode(BUZZER_PIN, OUTPUT);
// // //     setBuzzerPin(false);
// // //     pinMode(BTN_PIN, INPUT);

// // //     pinMode(STBY, OUTPUT); digitalWrite(STBY, LOW);
// // //     pinMode(R_IN1, OUTPUT); pinMode(R_IN2, OUTPUT);
// // //     pinMode(L_IN1, OUTPUT); pinMode(L_IN2, OUTPUT);
// // //     pwmSetup();
// // //     stopMotor();

// // //     for (int b = 0; b < 4; b++) pinMode(MUX_S[b], OUTPUT);
// // //     pinMode(SIG_PIN, INPUT);
// // //     analogReadResolution(12);
// // //     analogSetPinAttenuation(SIG_PIN, ADC_11db);
// // //     resetCal();

// // //     pinMode(ENC_L_A, INPUT); pinMode(ENC_R_A, INPUT);
// // //     attachInterrupt(digitalPinToInterrupt(ENC_L_A), isrL, RISING);
// // //     attachInterrupt(digitalPinToInterrupt(ENC_R_A), isrR, RISING);

// // //     BLEDevice::init("ROBOT_BLE");
// // //     BLEDevice::setMTU(517);
// // //     pServer = BLEDevice::createServer();
// // //     pServer->setCallbacks(new MyServerCallbacks());
// // //     BLEService* svc = pServer->createService(SERVICE_UUID);
// // //     pTx = svc->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
// // //     pTx->addDescriptor(new BLE2902());
// // //     BLECharacteristic* rx = svc->createCharacteristic(CHARACTERISTIC_UUID_RX,
// // //         BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
// // //     rx->setCallbacks(new MyCallbacks());
// // //     svc->start();
// // //     BLEAdvertising* adv = BLEDevice::getAdvertising();
// // //     adv->addServiceUUID(SERVICE_UUID);
// // //     adv->setScanResponse(true);
// // //     BLEDevice::startAdvertising();

// // //     digitalWrite(STBY, HIGH);
// // //     beep(2, 70, 60);
// // //     Serial.println("READY! Press 1: CAL -> Press 2: AUTO");
// // // }

// // // // ==================== LOOP ====================
// // // void loop() {
// // //     updateBuzzer();
// // //     updateButton();

// // //     if (!deviceConnected && oldDeviceConnected) {
// // //         delay(300); BLEDevice::startAdvertising(); oldDeviceConnected = false;
// // //     }
// // //     if (deviceConnected && !oldDeviceConnected) {
// // //         oldDeviceConnected = true;
// // //         beep(1, 60, 0);
// // //         bleSend("LOG:BLE_CONNECTED");
// // //     }

// // //     if (hasPendingCmd) {
// // //         String cmd;
// // //         portENTER_CRITICAL(&cmdMux);
// // //         cmd = pendingCmd;
// // //         hasPendingCmd = false;
// // //         portEXIT_CRITICAL(&cmdMux);
// // //         handleBLECommand(cmd);
// // //     }

// // //     if (calibrating) {
// // //         calibrateStep();
// // //         if (millis() >= calEnd) {
// // //             calibrating = false; calibrated = true;
// // //             currentModeName = "IDLE"; currentCaseName = "-";
// // //             char bad[64] = "LOG:CAL_WARN mat:";
// // //             bool anyBad = false;
// // //             for (int c = 0; c < NS; c++) if (!channelOk(c)) {
// // //                 char t[5]; snprintf(t, sizeof(t), " %d", c + 1);
// // //                 strncat(bad, t, sizeof(bad) - strlen(bad) - 1); anyBad = true;
// // //             }
// // //             if (anyBad) bleSend(bad);
// // //             beep(1, 1000, 0);
// // //             bleSend("LOG:CAL_DONE");
// // //         }
// // //     }

// // //     if (runLine)                          lineFollowPID();
// // //     else if (calibrated && !calibrating)  docSensor();
// // //     else                                  pattern = 0;

// // //     if (testMode && !calibrating && !runLine && deviceConnected && millis() - lastRawTime > 200) {
// // //         lastRawTime = millis();
// // //         char raw[100]; int n = snprintf(raw, sizeof(raw), "RAW:");
// // //         for (int i = 0; i < NS; i++)
// // //             n += snprintf(raw + n, sizeof(raw) - n, i ? ",%u" : "%u", rawv[i]);
// // //         bleSend(raw);
// // //     }

// // //     sendStatus();
// // // }
// // ////////
// // #include <Arduino.h>
// // #include <BLEDevice.h>
// // #include <BLEServer.h>
// // #include <BLEUtils.h>
// // #include <BLE2902.h>

// // #define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
// // #define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
// // #define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// // // ==================== BUTTON & BUZZER ====================
// // const int BTN_PIN    = 35;     // 3.3V -> Button -> IO35 + 1k pull-down to GND
// // const int BUZZER_PIN = 13;     // IO13 -> Buzzer (+) -> GND
// // #define BTN_PRESSED_LEVEL  HIGH
// // #define BUZZER_ACTIVE_HIGH true
// // #define BTN_DEBOUNCE_MS    40
// // #define BTN_RECAL_HOLD_MS  1200

// // // ==================== MOTOR ====================
// // const int R_PWM = 23, R_IN1 = 21, R_IN2 = 22;   // U6 - right
// // const int L_PWM = 25, L_IN1 = 27, L_IN2 = 26;   // U7 - left
// // const int STBY  = 33;
// // #define R_INVERT false
// // #define L_INVERT false
// // #define PWM_FREQ 20000
// // #define PWM_RES  8

// // // ==================== ENCODER ====================
// // const int ENC_L_A = 4, ENC_R_A = 14;
// // volatile long pulseLeft = 0, pulseRight = 0;
// // void IRAM_ATTR isrL() { pulseLeft++; }
// // void IRAM_ATTR isrR() { pulseRight++; }

// // // ==================== 12 SENSORS (CD74HC4067) ====================
// // #define NS 12
// // const uint8_t MUX_S[4] = {19, 18, 17, 16};
// // const uint8_t SIG_PIN  = 34;

// // const uint8_t MUX_CH[NS] = {11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
// // const uint16_t POS[NS] = {0, 1000, 2000, 3000, 4000, 5000,
// //                           6000, 7000, 8000, 9000, 10000, 11000};
// // #define SETPOINT 5500
// // #define INVERT_COLOR false
// // #define SENSOR_THRESHOLD 500
// // #define MUX_SETTLE_US 25

// // uint16_t calMin[NS], calMax[NS], rawv[NS], sensorVal[NS];
// // uint16_t pattern = 0;
// // unsigned int lastLinePos = SETPOINT;

// // void readRaw() {
// //     for (int i = 0; i < NS; i++) {
// //         uint8_t ch = MUX_CH[i];
// //         for (int b = 0; b < 4; b++) digitalWrite(MUX_S[b], (ch >> b) & 1);
// //         delayMicroseconds(MUX_SETTLE_US);
// //         analogRead(SIG_PIN);
// //         rawv[i] = (analogRead(SIG_PIN) + analogRead(SIG_PIN)) / 2;
// //     }
// // }
// // void resetCal() { for (int i = 0; i < NS; i++) { calMin[i] = 4095; calMax[i] = 0; } }
// // void calibrateStep() {
// //     readRaw();
// //     for (int i = 0; i < NS; i++) {
// //         if (rawv[i] < calMin[i]) calMin[i] = rawv[i];
// //         if (rawv[i] > calMax[i]) calMax[i] = rawv[i];
// //     }
// // }
// // bool channelOk(int i) { return calMax[i] > calMin[i] + 300; }

// // int docSensor() {
// //     readRaw();
// //     uint32_t s = 0, w = 0;
// //     int cnt = 0;
// //     pattern = 0;
// //     for (int i = 0; i < NS; i++) {
// //         int range = (int)calMax[i] - (int)calMin[i];
// //         int v = 0;
// //         if (range > 50) {
// //             v = constrain(((long)rawv[i] - calMin[i]) * 1000 / range, 0, 1000);
// //             if (INVERT_COLOR) v = 1000 - v;
// //         }
// //         sensorVal[i] = (uint16_t)v;
// //         if (v > SENSOR_THRESHOLD) { pattern |= (1 << i); cnt++; }
// //         if (v > 100) {
// //             uint32_t weight = v - 100;
// //             s += weight * POS[i];
// //             w += weight;
// //         }
// //     }
// //     if (w) lastLinePos = s / w;
// //     return cnt;
// // }

// // unsigned int getMaskedLinePos(int fromIdx, int toIdx, unsigned int fallbackPos) {
// //     uint32_t s = 0, w = 0;
// //     bool hasStrong = false;
// //     for (int i = fromIdx; i <= toIdx; i++) {
// //         if (sensorVal[i] > SENSOR_THRESHOLD) hasStrong = true;
// //         if (sensorVal[i] > 100) {
// //             uint32_t weight = sensorVal[i] - 100;
// //             s += weight * POS[i];
// //             w += weight;
// //         }
// //     }
// //     return (hasStrong && w) ? (unsigned int)(s / w) : fallbackPos;
// // }

// // // ==================== PID & SPEED ====================
// // float KP = 0.04f, KI = 0.00005f, KD = 0.8f;
// // int   BASE      = 100;
// // int   SPEED_MIN = 100;
// // int   E_SLOW    = 2500;

// // int   TURN_BASE = 100, TURN_OUT = 50, TURN_IN = 50;
// // #define TURN_MAX_COUNT    8
// // #define TURN_HOLD_MS      20
// // #define LOST_TURN_SPEED   85    // Toc do xoay tim line cham, binh tinh khi mat vach
// // const int BOOST = 15, BOOST_THRESHOLD = 160;

// // #define REF_DT_US   4000
// // #define D_FILTER    0.4f
// // #define I_MAX       30.0f
// // #define I_ZONE      2500
// // #define STEER_SLOW  100.0f
// // #define ACCEL_PER_S 500.0f

// // // ==================== START BLACK BOX ====================
// // #define START_BOX_SPEED         90    // Toc do chay thang binh tinh khi thoat khoi o START
// // #define START_BOX_BLOCK_CNT     6     // >= 6 sensors (or span >= 6) = inside / crossing black block
// // #define START_EXIT_CENTER_MS    15    // Confirm single line in center (both outer 2 sensors white)
// // #define START_EXIT_NEAR_EDGE_MS 20    // Confirm single line when outermost tips (0 & 11) are white
// // #define START_EXIT_EXTREME_MS   35    // Confirm single line if offset onto extreme sensor 0 or 11
// // #define START_WHITE_LOST_MS     45    // All-white after black block -> slid off line, pivot back
// // #define START_BOX_MAX_MS        2500  // Safety timeout for start-box exit mode
// // #define POST_BOX_TURN_GUARD_MS  120   // Suppress false 90-deg turn right at black-box exit
// // #define POST_BOX_STOP_GUARD_MS  600   // Extra finish-line lock after leaving start black box

// // // ==================== KICH BAN DAU MAP: GOC CONG DAU -> RE VONG TRON -> BO QUA NHANH CONG TREN ====================
// // // 1. Goc cong dau (VD: 001111100000 ben Trai hoac 000001111100 ben Phai):
// // //    -> Ghi nho ben den, KHOA DI THANG vuot qua goc cong dau.
// // // 2. Di thang tiep len vach ngang tren: sang >= 4/6 mat thang giua (hoac cnt >= 4, span >= 4):
// // //    + Goc cong dau ben TRAI (001111100000) -> RE PHAI (+1: banh trai tien, banh phai lui)
// // //    + Goc cong dau ben PHAI (000001111100) -> RE TRAI (-1: banh trai lui, banh phai tien)
// // // 3. Tren cung tron: khi gap nhanh vong cong phia tren (VD: 000001110000 pos~6095 hoac 000001111000 pos~6519):
// // //    -> Che mat phia ngoai, tiep tuc om cung tron trong bo qua nhanh re, vuot qua xong tra ve PID chuan.
// // #define ENABLE_ROUNDABOUT_START 1
// // #define ROUND_APPROACH_SPEED    85    // Toc do di thang binh tinh qua goc cong dau & len vach ngang tren
// // #define ROUND_ARC_SPEED         85    // Toc do bam cung tron on dinh
// // #define ROUND_ARM_LINE_MS       25    // On dinh tren vach thang don (1..3 mat) sau o START
// // #define ROUND_J1_HOLD_MS        55    // Thoi gian khoa thang toi thieu de vuot qua goc cong dau
// // #define ROUND_J1_MAX_MS         175   // Thoi gian khoa thang toi da khi vuot goc cong dau
// // #define ROUND_T_ARM_MS          35    // Quang nghi ngan sau khi thoat goc cong dau truoc khi canh vach ngang tren
// // #define ROUND_PRE_BRAKE_MS      22    // Nhap phanh ngan triet quan tinh truoc khi xoay vao vong tron
// // #define ROUND_PRE_BRAKE_PWM     160   // Luc phanh nguoc ngan truoc khi cua
// // #define ROUND_TURN_HOLD_MS      85    // Thoi gian xoay toi thieu de tach khoi vach ngang tren
// // #define ROUND_TURN_MAX_MS       420   // Thoi gian xoay toi da khi re vao vong tron
// // #define ROUND_TURN_BRAKE_MS     10    // Phanh nguoc chong quan tinh xoay khi vua bat duoc line vong tron
// // #define ROUND_ARC_SETTLE_MS     100   // On dinh tren cung tron sau cu re truoc khi canh nhanh cong phia tren
// // #define ROUND_MID_HOLD_MS       80    // Thoi gian che mat ngoai toi thieu de luot qua nhanh cong phia tren
// // #define ROUND_MID_MAX_MS        240   // Thoi gian che mat ngoai toi da khi bo qua nhanh cong phia tren

// // enum RoundStage : uint8_t {
// //     RND_ARM_J1        = 0,
// //     RND_WAIT_J1       = 1,
// //     RND_PASS_J1       = 2,
// //     RND_WAIT_CIRCLE_T = 3,
// //     RND_TURN_CIRCLE   = 4,
// //     RND_ARC_WAIT_MID  = 5,
// //     RND_ARC_PASS_MID  = 6,
// //     RND_DONE          = 7
// // };

// // // ==================== GAP, DASH SPRINT & FINISH ====================
// // #define GAP_ERR_MAX         1800
// // #define GAP_EDGE_GUARD_MS   120
// // #define GAP_MAX_COUNT       30      // Cho phep vuot nhieu doan net dut lien tiep
// // #define GAP_TIMEOUT_MS      1500

// // #define DASH_SPRINT_SPEED   150     // Toc do chay khi gap net dut cho den khi dung o dich
// // #define DASH_SPRINT_MIN     130     // Toc do toi thieu neu co khuc cua sau net dut
// // #define DASH_CONFIRM_MS     10      // Xac nhan khoang trang net dut that (loc nhieu ADC ngan)
// // #define DASH_ACCEL_PER_S    800.0f  // Gia toc muot len toc 150

// // #define STOP_MIN_CNT        10
// // #define STOP_CNT2           1
// // #define END_CONFIRM         2
// // #define START_GRACE_MS      1500
// // #define BRAKE_REV_SPEED     255
// // #define BRAKE_REV_MS        65
// // #define SPRINT_BRAKE_REV_MS 95      // Phanh nguoc du luc khi dang chay toc 150 vao DICH

// // int  cnt1 = 0, cnt2 = 0;
// // bool lostNow = false, gapStraight = false, blackLatched = false;
// // bool dashSprint = false;
// // int  fullBlackFrames = 0, gapSpeed = 0;
// // int  activeTurnDir = 0;
// // unsigned long gapStart = 0, armStopAt = 0, lastEdgeMs = 0, turnHoldUntil = 0;

// // bool inStartBox = false, sawStartBlack = false, startCenteredOnce = false;
// // int  lastBoxSide = 0;
// // unsigned long startLineConfirmMs = 0, startWhiteStartMs = 0, postBoxTurnEnableAt = 0;

// // uint8_t roundStage = RND_DONE;
// // unsigned long roundStageMs = 0;
// // int  j1BranchSide  = 0;   // -1 = goc cong dau ben TRAI (001111100000), +1 = goc cong dau ben PHAI (000001111100)
// // int  circleTurnDir = 1;   // +1 = re PHAI theo mui xe, -1 = re TRAI theo mui xe
// // int  j1LeftFrames  = 0, j1RightFrames = 0;
// // int  stageConfirmFrames = 0, stageWhiteFrames = 0;
// // int  arcSettleFrames = 0;

// // float integral = 0, lastE = 0, dFiltered = 0, curV = 100;
// // unsigned long lastPidUs = 0;
// // int   speed_dk = 120;
// // int   lastError = 0;

// // bool timing = false;
// // unsigned long runStartMs = 0, runElapsedMs = 0;
// // void startTimer() { runStartMs = millis(); runElapsedMs = 0; timing = true; }
// // void stopTimer()  { if (timing) { runElapsedMs = millis() - runStartMs; timing = false; } }

// // bool runLine = false, testMode = false;
// // bool calibrated = false, calibrating = false;
// // unsigned long calEnd = 0, lastRawTime = 0, lastStatusTime = 0;
// // #define CAL_TIME 5000
// // #define STATUS_INTERVAL 150
// // const char* currentModeName = "IDLE";
// // const char* currentCaseName = "-";

// // BLEServer*         pServer = nullptr;
// // BLECharacteristic* pTx     = nullptr;
// // bool deviceConnected = false, oldDeviceConnected = false;

// // portMUX_TYPE cmdMux = portMUX_INITIALIZER_UNLOCKED;
// // String pendingCmd = "";
// // volatile bool hasPendingCmd = false;

// // // ==================== NON-BLOCKING BUZZER ====================
// // int  buzRemain = 0;
// // bool buzState  = false;
// // unsigned long buzNextMs = 0, buzOnDur = 80, buzOffDur = 60;

// // inline void setBuzzerPin(bool on) {
// //     digitalWrite(BUZZER_PIN, (on ^ !BUZZER_ACTIVE_HIGH) ? HIGH : LOW);
// // }

// // void beep(int count, unsigned long onMs = 80, unsigned long offMs = 60) {
// //     if (count <= 0) { buzRemain = 0; buzState = false; setBuzzerPin(false); return; }
// //     buzRemain = count;
// //     buzOnDur  = onMs;
// //     buzOffDur = offMs;
// //     buzState  = true;
// //     setBuzzerPin(true);
// //     buzNextMs = millis() + onMs;
// // }

// // void updateBuzzer() {
// //     if (buzRemain <= 0 && !buzState) return;
// //     unsigned long now = millis();
// //     if ((long)(now - buzNextMs) < 0) return;

// //     if (buzState) {
// //         buzState = false;
// //         setBuzzerPin(false);
// //         buzRemain--;
// //         if (buzRemain > 0) buzNextMs = now + buzOffDur;
// //     } else if (buzRemain > 0) {
// //         buzState = true;
// //         setBuzzerPin(true);
// //         buzNextMs = now + buzOnDur;
// //     }
// // }

// // // ==================== MOTOR DRIVE ====================
// // void pwmSetup() {
// // #if ESP_ARDUINO_VERSION_MAJOR >= 3
// //     ledcAttach(R_PWM, PWM_FREQ, PWM_RES);
// //     ledcAttach(L_PWM, PWM_FREQ, PWM_RES);
// // #else
// //     ledcSetup(0, PWM_FREQ, PWM_RES); ledcAttachPin(R_PWM, 0);
// //     ledcSetup(1, PWM_FREQ, PWM_RES); ledcAttachPin(L_PWM, 1);
// // #endif
// // }
// // void pwmWrite(bool right, int duty) {
// // #if ESP_ARDUINO_VERSION_MAJOR >= 3
// //     ledcWrite(right ? R_PWM : L_PWM, duty);
// // #else
// //     ledcWrite(right ? 0 : 1, duty);
// // #endif
// // }
// // void setMotor(bool right, int sp) {
// //     sp = constrain(sp, -255, 255);
// //     bool fwd = (sp >= 0) ^ (right ? R_INVERT : L_INVERT);
// //     int in1 = right ? R_IN1 : L_IN1, in2 = right ? R_IN2 : L_IN2;
// //     if (sp == 0) {
// //         digitalWrite(in1, HIGH);
// //         digitalWrite(in2, HIGH);
// //         pwmWrite(right, 255);
// //     } else {
// //         digitalWrite(in1, fwd ? HIGH : LOW);
// //         digitalWrite(in2, fwd ? LOW : HIGH);
// //         pwmWrite(right, abs(sp));
// //     }
// // }
// // void move(int left, int right) { setMotor(false, left); setMotor(true, right); }
// // void stopMotor() { move(0, 0); }

// // void hardBrakeStop() {
// //     unsigned long brakeMs = dashSprint ? SPRINT_BRAKE_REV_MS : BRAKE_REV_MS;
// //     if (brakeMs > 0) {
// //         bool encActive = (pulseLeft > 30 && pulseRight > 30);
// //         unsigned long minHold = encActive ? (brakeMs * 65 / 100) : brakeMs;
// //         move(-BRAKE_REV_SPEED, -BRAKE_REV_SPEED);
// //         unsigned long t0 = millis();
// //         long lastPL = pulseLeft, lastPR = pulseRight;
// //         unsigned long lastMoveMs = t0;
// //         while (millis() - t0 < brakeMs) {
// //             long pL = pulseLeft, pR = pulseRight;
// //             unsigned long now = millis();
// //             if (pL != lastPL || pR != lastPR) {
// //                 lastPL = pL;
// //                 lastPR = pR;
// //                 lastMoveMs = now;
// //             }
// //             if (encActive && (now - t0 >= minHold) && (now - lastMoveMs >= 12)) {
// //                 break;
// //             }
// //             delay(1);
// //         }
// //     }
// //     stopMotor();
// // }

// // void movePID(int base, int steer) {
// //     int L = base + steer, R = base - steer;
// //     if (steer >  BOOST_THRESHOLD) L += BOOST;
// //     if (steer < -BOOST_THRESHOLD) R += BOOST;
// //     move(constrain(L, -255, 255), constrain(R, -255, 255));
// // }

// // inline void driveSharpTurn(int dir) {
// //     // dir > 0: RE PHAI theo mui xe (banh trai tien, banh phai lui)
// //     // dir < 0: RE TRAI theo mui xe (banh trai lui, banh phai tien)
// //     if (dir > 0) move(TURN_BASE + TURN_OUT, TURN_BASE - TURN_IN);
// //     else         move(TURN_BASE - TURN_IN, TURN_BASE + TURN_OUT);
// // }

// // // ==================== BLE SEND ====================
// // void bleSend(const char* msg) {
// //     if (deviceConnected && pTx) {
// //         pTx->setValue((uint8_t*)msg, strlen(msg));
// //         pTx->notify();
// //     }
// //     Serial.println(msg);
// // }

// // void sendStatus() {
// //     if (!deviceConnected || millis() - lastStatusTime < STATUS_INTERVAL) return;
// //     lastStatusTime = millis();
// //     char pat[NS + 1];
// //     for (int i = 0; i < NS; i++) pat[i] = (pattern & (1 << i)) ? '1' : '0';
// //     pat[NS] = 0;
// //     char buf[256];
// //     snprintf(buf, sizeof(buf),
// //         "{\"mode\":\"%s\",\"case\":\"%s\",\"pos\":%u,\"pat\":\"%s\",\"spd\":%d,\"err\":%d,\"cal\":%d,"
// //         "\"c1\":%d,\"c2\":%d,\"v\":%d,\"t\":%lu}",
// //         currentModeName, currentCaseName, lastLinePos, pat,
// //         speed_dk, lastError, calibrating ? 2 : (calibrated ? 1 : 0), cnt1, cnt2,
// //         (int)curV, timing ? (millis() - runStartMs) : runElapsedMs);
// //     pTx->setValue((uint8_t*)buf, strlen(buf));
// //     pTx->notify();
// // }

// // // ==================== ACTIONS ====================
// // void startCalibration() {
// //     runLine = false; testMode = false; stopTimer(); stopMotor();
// //     calibrated = false; calibrating = true; calEnd = millis() + CAL_TIME;
// //     resetCal(); pattern = 0;
// //     currentModeName = "CALIB"; currentCaseName = "CALIBRATING";
// //     beep(1, 150, 0);
// //     bleSend("LOG:CAL_START");
// // }

// // void startAutoRun() {
// //     if (!calibrated) {
// //         beep(2, 150, 100);
// //         bleSend("LOG:NEED_CAL");
// //         return;
// //     }
// //     testMode = false; runLine = true;
// //     lastError = 0; lastE = 0; integral = 0; dFiltered = 0;
// //     lastPidUs = 0; lastLinePos = SETPOINT;
// //     pulseLeft = pulseRight = 0;
// //     cnt1 = 0; cnt2 = 0;
// //     lostNow = false; gapStraight = false;
// //     dashSprint = false;
// //     blackLatched = false; fullBlackFrames = 0;
// //     activeTurnDir = 0; lastEdgeMs = 0; turnHoldUntil = 0;

// //     inStartBox = true;
// //     sawStartBlack = false;
// //     startCenteredOnce = false;
// //     lastBoxSide = 0;
// //     startLineConfirmMs = 0;
// //     startWhiteStartMs = 0;
// //     postBoxTurnEnableAt = 0;

// //     roundStage = ENABLE_ROUNDABOUT_START ? RND_ARM_J1 : RND_DONE;
// //     roundStageMs = millis();
// //     j1BranchSide = 0;
// //     circleTurnDir = 1;
// //     j1LeftFrames = 0;
// //     j1RightFrames = 0;
// //     stageConfirmFrames = 0;
// //     stageWhiteFrames = 0;
// //     arcSettleFrames = 0;

// //     curV = min(START_BOX_SPEED, min(SPEED_MIN, BASE));
// //     armStopAt = millis() + START_GRACE_MS;
// //     startTimer();
// //     currentModeName = "AUTO"; currentCaseName = "START_BOX";
// //     beep(1, 120, 0);
// //     bleSend("LOG:AUTO_START");
// // }

// // // ==================== BUTTON IO35 ====================
// // bool btnLastRaw = false, btnStable = false, btnLongHandled = false;
// // unsigned long btnDebounceMs = 0, btnPressStartMs = 0;

// // void updateButton() {
// //     unsigned long now = millis();
// //     bool pressed = (digitalRead(BTN_PIN) == BTN_PRESSED_LEVEL);

// //     if (pressed != btnLastRaw) {
// //         btnLastRaw = pressed;
// //         btnDebounceMs = now;
// //     }
// //     unsigned long debounceReq = runLine ? 250 : BTN_DEBOUNCE_MS;
// //     if (now - btnDebounceMs < debounceReq) return;

// //     if (pressed != btnStable) {
// //         btnStable = pressed;
// //         if (btnStable) {
// //             btnPressStartMs = now;
// //             btnLongHandled = false;
// //             if (runLine && !calibrating) {
// //                 btnLongHandled = true;
// //                 hardBrakeStop();
// //                 runLine = false; stopTimer();
// //                 currentModeName = "MANUAL"; currentCaseName = "IDLE";
// //                 beep(1, 200, 0);
// //                 bleSend("LOG:MODE_MANUAL");
// //             }
// //         } else {
// //             if (!btnLongHandled && !calibrating && !runLine) {
// //                 if (!calibrated) startCalibration();
// //                 else             startAutoRun();
// //             }
// //         }
// //     } else if (btnStable && !btnLongHandled && !calibrating && !runLine && calibrated) {
// //         if (now - btnPressStartMs >= BTN_RECAL_HOLD_MS) {
// //             btnLongHandled = true;
// //             startCalibration();
// //         }
// //     }
// // }

// // void finishRun() {
// //     stopTimer();
// //     hardBrakeStop();
// //     runLine = false;
// //     currentModeName = "DONE"; currentCaseName = "FINISH";
// //     beep(2, 200, 100);
// //     char b[40]; snprintf(b, sizeof(b), "LOG:FINISH %.2fs", runElapsedMs / 1000.0f);
// //     bleSend(b);
// // }

// // // ==================== LINE FOLLOW PID ====================
// // void lineFollowPID() {
// //     int cnt = docSensor();
// //     bool edgeL = (pattern & 0x03) != 0;                // mat 0, 1 (bien TRAI)
// //     bool edgeR = (pattern & (0x03 << (NS - 2))) != 0;  // mat 10, 11 (bien PHAI)
// //     bool centerOn = (pattern & 0x00F0) != 0;           // 4 mat chinh giua doi xung: mat 4, 5, 6, 7
// //     bool mid6On   = (pattern & 0x01F8) != 0;           // 6 mat thang giua: mat 3, 4, 5, 6, 7, 8
// //     int  mid6Cnt  = __builtin_popcount(pattern & 0x01F8);
// //     bool leftOuter3  = (pattern & 0x0007) != 0;        // 3 mat ngoai ben TRAI: 0, 1, 2
// //     bool rightOuter3 = (pattern & 0x0E00) != 0;        // 3 mat ngoai ben PHAI: 9, 10, 11
// //     unsigned long nowMs = millis();
// //     int vmin = min(SPEED_MIN, BASE);

// //     int firstIdx = -1, lastIdx = -1;
// //     int leftHalfCnt = 0, rightHalfCnt = 0;
// //     for (int i = 0; i < NS; i++) {
// //         if (pattern & (1 << i)) {
// //             if (firstIdx < 0) firstIdx = i;
// //             lastIdx = i;
// //             if (i < 6) leftHalfCnt++;
// //             else       rightHalfCnt++;
// //         }
// //     }
// //     int span = (cnt > 0) ? (lastIdx - firstIdx + 1) : 0;

// //     // Neu bi loa bat trong khoi den xuat phat lam thoat som khi van con trong o START
// //     if (!inStartBox && roundStage == RND_ARM_J1 && cnt1 == 0 && (nowMs - runStartMs < START_GRACE_MS) &&
// //         (cnt > TURN_MAX_COUNT || (cnt >= 7 && edgeL && edgeR))) {
// //         inStartBox = true;
// //         sawStartBlack = true;
// //         startLineConfirmMs = 0;
// //         startWhiteStartMs = 0;
// //         activeTurnDir = 0;
// //     }

// //     // ==================== CHAY THANG THOAT KHOI O VUONG DEN -> BAM PID ====================
// //     if (inStartBox) {
// //         armStopAt = max(armStopAt, nowMs + POST_BOX_STOP_GUARD_MS);
// //         fullBlackFrames = 0;
// //         blackLatched = false;
// //         activeTurnDir = 0;
// //         lostNow = false;
// //         lastEdgeMs = 0;
// //         roundStageMs = nowMs;

// //         if (cnt > 0 && (firstIdx + lastIdx) != (NS - 1)) {
// //             lastBoxSide = ((firstIdx + lastIdx) > (NS - 1)) ? 1 : -1;
// //         }

// //         if (cnt >= START_BOX_BLOCK_CNT || span >= START_BOX_BLOCK_CNT) {
// //             sawStartBlack = true;
// //             startLineConfirmMs = 0;
// //             startWhiteStartMs = 0;
// //         }

// //         bool clearBothEdges = !edgeL && !edgeR;
// //         bool clearOuterTips = ((pattern & 1) == 0) && ((pattern & (1 << (NS - 1))) == 0);
// //         unsigned long confirmReqMs = 0;
// //         bool singleLineCandidate = false;

// //         if (clearBothEdges && cnt >= 1 && cnt <= 5 && span <= 5) {
// //             singleLineCandidate = true;
// //             confirmReqMs = START_EXIT_CENTER_MS;
// //         } else if (clearOuterTips && cnt >= 1 && cnt <= 4 && span <= 4) {
// //             singleLineCandidate = true;
// //             confirmReqMs = START_EXIT_NEAR_EDGE_MS;
// //         } else if (cnt >= 1 && cnt <= 4 && span <= 4) {
// //             singleLineCandidate = true;
// //             confirmReqMs = START_EXIT_EXTREME_MS;
// //         }

// //         bool exitedToLine = false;
// //         bool slidOffToWhite = false;

// //         if (singleLineCandidate) {
// //             startWhiteStartMs = 0;
// //             if (startLineConfirmMs == 0) startLineConfirmMs = nowMs;
// //             if (nowMs - startLineConfirmMs >= confirmReqMs) {
// //                 exitedToLine = true;
// //             }
// //         } else {
// //             startLineConfirmMs = 0;
// //             if (cnt == 0 && sawStartBlack) {
// //                 if (startWhiteStartMs == 0) startWhiteStartMs = nowMs;
// //                 else if (nowMs - startWhiteStartMs >= START_WHITE_LOST_MS) {
// //                     slidOffToWhite = true;
// //                 }
// //             } else {
// //                 startWhiteStartMs = 0;
// //             }
// //         }

// //         if (!exitedToLine && !slidOffToWhite && (nowMs - runStartMs >= START_BOX_MAX_MS)) {
// //             exitedToLine = true;
// //         }

// //         if (!exitedToLine && !slidOffToWhite) {
// //             if (!singleLineCandidate) lastLinePos = SETPOINT;
// //             lastError = 0;
// //             lastE = 0;
// //             integral = 0;
// //             dFiltered = 0;

// //             unsigned long us = micros();
// //             float dtn = lastPidUs ? (us - lastPidUs) / (float)REF_DT_US : 1.0f;
// //             lastPidUs = us;
// //             dtn = constrain(dtn, 0.1f, 4.0f);

// //             float boxTarget = (float)min(BASE, START_BOX_SPEED);
// //             if (boxTarget < curV) curV = boxTarget;
// //             else                  curV = min(boxTarget, curV + ACCEL_PER_S * dtn * REF_DT_US / 1e6f);

// //             currentCaseName = "START_BOX";
// //             move((int)curV, (int)curV);
// //             return;
// //         }

// //         inStartBox = false;
// //         postBoxTurnEnableAt = nowMs + POST_BOX_TURN_GUARD_MS;
// //         roundStageMs = nowMs;
// //         integral = 0;
// //         dFiltered = 0;
// //         lastPidUs = 0;

// //         if (slidOffToWhite) {
// //             startCenteredOnce = false;
// //             lastLinePos = (lastBoxSide >= 0) ? POS[NS - 1] : POS[0];
// //             lastError = (int)lastLinePos - SETPOINT;
// //             lastE = (float)lastError;
// //             lastEdgeMs = nowMs;
// //             bleSend("LOG:BOX_RECOVER");
// //         } else {
// //             startCenteredOnce = (centerOn && !edgeL && !edgeR);
// //             float initE = (cnt > 0) ? ((float)lastLinePos - SETPOINT) : 0.0f;
// //             lastE = initE;
// //             lastError = (int)initE;
// //             lastEdgeMs = 0;
// //             bleSend("LOG:BOX_EXIT_PID");
// //         }
// //     }

// //     // ==================== XU LY GOC CONG DAU -> RE VONG TRON -> BO QUA VONG CONG PHIA TREN ====================
// //     unsigned int pidPos = lastLinePos;
// //     bool smoothResetPid = false;
// //     const char* stageCaseOverride = nullptr;

// //     if (roundStage < RND_DONE) {
// //         armStopAt = max(armStopAt, nowMs + 1000);
// //         fullBlackFrames = 0;
// //         blackLatched = false;
// //         activeTurnDir = 0;

// //         int straightSpd = min(BASE, ROUND_APPROACH_SPEED);

// //         if (roundStage == RND_ARM_J1) {
// //             stageCaseOverride = "RND_ARM_J1";
// //             bool singleStraight = (cnt >= 1 && cnt <= 3 && span <= 3 && !leftOuter3 && !rightOuter3) ||
// //                                   (cnt == 4 && firstIdx == 4 && lastIdx == 7);
// //             if (singleStraight) {
// //                 if (nowMs - roundStageMs >= ROUND_ARM_LINE_MS) {
// //                     roundStage = RND_WAIT_J1;
// //                     roundStageMs = nowMs;
// //                     j1LeftFrames = 0;
// //                     j1RightFrames = 0;
// //                 }
// //             } else {
// //                 roundStageMs = nowMs;
// //             }
// //         }
// //         else if (roundStage == RND_WAIT_J1) {
// //             stageCaseOverride = "RND_WAIT_J1";

// //             // Nhan dien goc cong dau ngay khi vach phinh lech 1 ben (VD: 001111100000 ben Trai hoac 000001111100 ben Phai)
// //             bool wideOrSplit = (cnt >= 4 && span >= 4) || (cnt >= 3 && span >= 5);
// //             bool candLeft  = wideOrSplit &&
// //                              !rightOuter3 &&
// //                              (firstIdx >= 0 && firstIdx <= 3) &&
// //                              (lastIdx >= 4 && lastIdx <= 8) &&
// //                              ((firstIdx + lastIdx) <= 9) &&
// //                              (leftHalfCnt > rightHalfCnt);
// //             bool candRight = wideOrSplit &&
// //                              !leftOuter3 &&
// //                              (lastIdx >= 8 && lastIdx < NS) &&
// //                              (firstIdx >= 3 && firstIdx <= 7) &&
// //                              ((firstIdx + lastIdx) >= 13) &&
// //                              (rightHalfCnt > leftHalfCnt);

// //             if (candLeft) {
// //                 j1LeftFrames++;
// //                 j1RightFrames = 0;
// //                 if (j1LeftFrames >= 2) {
// //                     j1BranchSide  = -1; // Goc cong dau lech TRAI (001111100000)
// //                     circleTurnDir =  1; // -> Len vach ngang tren RE PHAI theo mui xe
// //                     roundStage = RND_PASS_J1;
// //                     roundStageMs = nowMs;
// //                     stageConfirmFrames = 0;
// //                     beep(1, 45, 0);
// //                     bleSend("LOG:J1_LEFT->TURN_R");
// //                 }
// //                 lastPidUs = micros();
// //                 currentCaseName = "PASS_J1_L";
// //                 move(straightSpd, straightSpd);
// //                 return;
// //             }
// //             else if (candRight) {
// //                 j1RightFrames++;
// //                 j1LeftFrames = 0;
// //                 if (j1RightFrames >= 2) {
// //                     j1BranchSide  =  1; // Goc cong dau lech PHAI (000001111100)
// //                     circleTurnDir = -1; // -> Len vach ngang tren RE TRAI theo mui xe
// //                     roundStage = RND_PASS_J1;
// //                     roundStageMs = nowMs;
// //                     stageConfirmFrames = 0;
// //                     beep(1, 45, 0);
// //                     bleSend("LOG:J1_RIGHT->TURN_L");
// //                 }
// //                 lastPidUs = micros();
// //                 currentCaseName = "PASS_J1_R";
// //                 move(straightSpd, straightSpd);
// //                 return;
// //             }
// //             else {
// //                 if (j1LeftFrames > 0 || j1RightFrames > 0) smoothResetPid = true;
// //                 j1LeftFrames = 0;
// //                 j1RightFrames = 0;

// //                 if ((nowMs - roundStageMs >= 250) && edgeL && edgeR && cnt >= 6) {
// //                     circleTurnDir = (j1BranchSide != 0) ? -j1BranchSide : 1;
// //                     if (ROUND_PRE_BRAKE_MS > 0) {
// //                         move(-ROUND_PRE_BRAKE_PWM, -ROUND_PRE_BRAKE_PWM);
// //                         delay(ROUND_PRE_BRAKE_MS);
// //                     }
// //                     roundStage = RND_TURN_CIRCLE;
// //                     roundStageMs = millis();
// //                     bleSend("LOG:T_FALLBACK_TURN");
// //                     currentCaseName = (circleTurnDir > 0) ? "CIRCLE_TURN_R" : "CIRCLE_TURN_L";
// //                     driveSharpTurn(circleTurnDir);
// //                     return;
// //                 }
// //             }
// //         }
// //         else if (roundStage == RND_PASS_J1) {
// //             unsigned long el = nowMs - roundStageMs;
// //             bool cleanSingleLine = (cnt >= 1 && cnt <= 3 && span <= 3) && !leftOuter3 && !rightOuter3;
// //             if (el >= ROUND_J1_HOLD_MS && cleanSingleLine) {
// //                 stageConfirmFrames++;
// //             } else {
// //                 stageConfirmFrames = 0;
// //             }

// //             if (stageConfirmFrames < 3 && el < ROUND_J1_MAX_MS) {
// //                 lastPidUs = micros();
// //                 currentCaseName = (j1BranchSide < 0) ? "PASS_J1_L" : "PASS_J1_R";
// //                 move(straightSpd, straightSpd);
// //                 return;
// //             }

// //             roundStage = RND_WAIT_CIRCLE_T;
// //             roundStageMs = nowMs;
// //             stageConfirmFrames = 0;
// //             stageWhiteFrames = 0;
// //             lostNow = false;
// //             lastEdgeMs = 0;
// //             curV = (float)straightSpd;
// //             smoothResetPid = true;
// //             bleSend("LOG:PASS_J1_STRAIGHT");
// //             stageCaseOverride = (circleTurnDir > 0) ? "TO_TOP_BAR_R" : "TO_TOP_BAR_L";
// //         }

// //         if (roundStage == RND_WAIT_CIRCLE_T) {
// //             stageCaseOverride = (circleTurnDir > 0) ? "TO_TOP_BAR_R" : "TO_TOP_BAR_L";
// //             unsigned long el = nowMs - roundStageMs;

// //             if (el >= ROUND_T_ARM_MS) {
// //                 bool hitTopBar = (mid6Cnt >= 4) ||
// //                                   (cnt >= 4 && span >= 4 && mid6Cnt >= 3) ||
// //                                   (cnt >= 5 && span >= 5) ||
// //                                   (edgeL && edgeR && cnt >= 4);

// //                 if (hitTopBar) {
// //                     stageConfirmFrames++;
// //                     stageWhiteFrames = 0;
// //                     if (stageConfirmFrames >= 2) {
// //                         if (ROUND_PRE_BRAKE_MS > 0) {
// //                             move(-ROUND_PRE_BRAKE_PWM, -ROUND_PRE_BRAKE_PWM);
// //                             delay(ROUND_PRE_BRAKE_MS);
// //                         }
// //                         roundStage = RND_TURN_CIRCLE;
// //                         roundStageMs = millis();
// //                         stageConfirmFrames = 0;
// //                         integral = 0; dFiltered = 0;
// //                         beep(1, 60, 0);
// //                         bleSend((circleTurnDir > 0) ? "LOG:TOP_BAR_TURN_R" : "LOG:TOP_BAR_TURN_L");
// //                         currentCaseName = (circleTurnDir > 0) ? "CIRCLE_TURN_R" : "CIRCLE_TURN_L";
// //                         driveSharpTurn(circleTurnDir);
// //                         return;
// //                     }
// //                     lastPidUs = micros();
// //                     currentCaseName = "TOP_BAR_HIT";
// //                     move(straightSpd, straightSpd);
// //                     return;
// //                 } else {
// //                     if (stageConfirmFrames > 0) smoothResetPid = true;
// //                     stageConfirmFrames = 0;

// //                     if (cnt == 0 && el >= 80) {
// //                         stageWhiteFrames++;
// //                         if (stageWhiteFrames >= 4) {
// //                             roundStage = RND_TURN_CIRCLE;
// //                             roundStageMs = nowMs;
// //                             stageWhiteFrames = 0;
// //                             integral = 0; dFiltered = 0;
// //                             beep(1, 60, 0);
// //                             bleSend((circleTurnDir > 0) ? "LOG:TOP_WHITE_TURN_R" : "LOG:TOP_WHITE_TURN_L");
// //                             currentCaseName = (circleTurnDir > 0) ? "CIRCLE_TURN_R" : "CIRCLE_TURN_L";
// //                             driveSharpTurn(circleTurnDir);
// //                             return;
// //                         }
// //                         lastPidUs = micros();
// //                         currentCaseName = "TOP_CONFIRM_W";
// //                         move(60, 60);
// //                         return;
// //                     }
// //                     stageWhiteFrames = 0;
// //                 }
// //             }
// //         }
// //         else if (roundStage == RND_TURN_CIRCLE) {
// //             // Xoay vao vong tron theo circleTurnDir (+1 = Re Phai, -1 = Re Trai) -> chuyen sang RND_ARC_WAIT_MID
// //             unsigned long el = nowMs - roundStageMs;
// //             bool caughtArc = (el >= ROUND_TURN_HOLD_MS) &&
// //                              mid6On && !edgeL && !edgeR &&
// //                              (cnt >= 1 && cnt <= 5 && span <= 5);
// //             if (!caughtArc && el < ROUND_TURN_MAX_MS) {
// //                 currentCaseName = (circleTurnDir > 0) ? "CIRCLE_TURN_R" : "CIRCLE_TURN_L";
// //                 driveSharpTurn(circleTurnDir);
// //                 return;
// //             }
// //             if (ROUND_TURN_BRAKE_MS > 0) {
// //                 if (circleTurnDir > 0) move(-60, 60);
// //                 else                   move(60, -60);
// //                 delay(ROUND_TURN_BRAKE_MS);
// //                 cnt = docSensor();
// //                 edgeL = (pattern & 0x03) != 0;
// //                 edgeR = (pattern & (0x03 << (NS - 2))) != 0;
// //                 centerOn = (pattern & 0x00F0) != 0;
// //                 mid6On   = (pattern & 0x01F8) != 0;
// //                 firstIdx = -1; lastIdx = -1;
// //                 leftHalfCnt = 0; rightHalfCnt = 0;
// //                 for (int i = 0; i < NS; i++) {
// //                     if (pattern & (1 << i)) {
// //                         if (firstIdx < 0) firstIdx = i;
// //                         lastIdx = i;
// //                         if (i < 6) leftHalfCnt++; else rightHalfCnt++;
// //                     }
// //                 }
// //                 span = (cnt > 0) ? (lastIdx - firstIdx + 1) : 0;
// //             }
// //             nowMs = millis();
// //             roundStage = RND_ARC_WAIT_MID;
// //             roundStageMs = nowMs;
// //             stageConfirmFrames = 0;
// //             stageWhiteFrames = 0;
// //             arcSettleFrames = 0;
// //             startCenteredOnce = true;
// //             lostNow = false;
// //             lastEdgeMs = (cnt == 0) ? nowMs : 0;
// //             lastError = (cnt > 0) ? ((int)lastLinePos - SETPOINT) : ((circleTurnDir > 0) ? -3000 : 3000);
// //             lastE = (float)lastError;
// //             lastPidUs = 0;
// //             curV = (float)min(BASE, ROUND_ARC_SPEED);
// //             smoothResetPid = true;
// //             pidPos = lastLinePos;
// //             bleSend("LOG:CIRCLE_ARC_START");
// //             stageCaseOverride = "CIRCLE_ARC";
// //         }
// //         else if (roundStage == RND_ARC_WAIT_MID) {
// //             // Dang om vong cong: canh nhanh vong cong phia tren (VD: 000001110000 pos~6095 hoac 000001111000 pos~6519)
// //             stageCaseOverride = "CIRCLE_ARC";
// //             unsigned long el = nowMs - roundStageMs;

// //             bool huggingInner = (cnt >= 1 && cnt <= 3) &&
// //                                 ((circleTurnDir > 0) ? (lastIdx <= 6 && lastLinePos <= 5650)
// //                                                      : (firstIdx >= 5 && lastLinePos >= 5350));
// //             if (huggingInner && arcSettleFrames < 100) arcSettleFrames++;

// //             if (cnt == 0) {
// //                 // Neu di qua xa tren cung tron (> 700ms) ma gap trang tinh -> da toi dau mut cuoi cung tron, tra ve PID
// //                 if (el >= 700) {
// //                     stageWhiteFrames++;
// //                     if (stageWhiteFrames >= 4) {
// //                         roundStage = RND_DONE;
// //                         roundStageMs = nowMs;
// //                         postBoxTurnEnableAt = nowMs;
// //                         lastEdgeMs = nowMs;
// //                         lastError = (circleTurnDir > 0) ? 3500 : -3500;
// //                         lastE = (float)lastError;
// //                         bleSend("LOG:ARC_WHITE_DONE");
// //                     }
// //                 } else {
// //                     lastError = (circleTurnDir > 0) ? -3000 : 3000;
// //                 }
// //             } else {
// //                 stageWhiteFrames = 0;
// //             }

// //             bool armedForMid = (el >= ROUND_ARC_SETTLE_MS) && (arcSettleFrames >= 3 || el >= 200);
// //             if (armedForMid && cnt > 0) {
// //                 bool sawMidBranch = false;
// //                 if (circleTurnDir > 0) {
// //                     // Cung tron uon TRAI (mat <= 5), nhanh cong phia tren nho ra PHAI (000001110000 / 000001111000 / mat 8..11)
// //                     sawMidBranch = !edgeL && (
// //                         (cnt >= 3 && firstIdx >= 5 && lastIdx >= 7 && lastLinePos >= 5850) ||
// //                         (cnt >= 4 && span >= 4 && lastIdx >= 7) ||
// //                         (cnt >= 2 && span >= 4 && lastIdx >= 7) ||
// //                         ((pattern & 0x0F00) != 0)
// //                     );
// //                 } else {
// //                     // Doi xung: cung tron uon PHAI (mat >= 6), nhanh cong nho ra TRAI (000011100000 / 000111100000 / mat 0..3)
// //                     sawMidBranch = !edgeR && (
// //                         (cnt >= 3 && lastIdx <= 6 && firstIdx <= 4 && lastLinePos <= 5150) ||
// //                         (cnt >= 4 && span >= 4 && firstIdx <= 4) ||
// //                         (cnt >= 2 && span >= 4 && firstIdx <= 4) ||
// //                         ((pattern & 0x000F) != 0)
// //                     );
// //                 }

// //                 if (sawMidBranch) {
// //                     if (stageConfirmFrames == 0) smoothResetPid = true;
// //                     // Che han cum mat phia ngoai (6..11 hoac 0..5), giu xe tiep tuc bam muot theo mep trong vong tron!
// //                     if (circleTurnDir > 0) {
// //                         pidPos = min(getMaskedLinePos(0, 5, 4400), 4700U);
// //                     } else {
// //                         pidPos = max(getMaskedLinePos(6, 11, 6600), 6300U);
// //                     }
// //                     stageCaseOverride = "SKIP_UPPER_ARC";
// //                     stageConfirmFrames++;
// //                     if (stageConfirmFrames >= 2) {
// //                         roundStage = RND_ARC_PASS_MID;
// //                         roundStageMs = nowMs;
// //                         stageConfirmFrames = 0;
// //                         beep(1, 45, 0);
// //                         bleSend("LOG:SKIP_UPPER_ARC");
// //                     }
// //                 } else {
// //                     if (stageConfirmFrames > 0) smoothResetPid = true;
// //                     stageConfirmFrames = 0;
// //                 }
// //             }
// //         }
// //         else if (roundStage == RND_ARC_PASS_MID) {
// //             // Tiep tuc che mat phia ngoai de bo qua nhanh vong cong phia tren va chay tiep tren cung tron
// //             stageCaseOverride = "SKIP_UPPER_ARC";
// //             unsigned long el = nowMs - roundStageMs;

// //             if (circleTurnDir > 0) {
// //                 pidPos = min(getMaskedLinePos(0, 5, 4400), 4700U);
// //             } else {
// //                 pidPos = max(getMaskedLinePos(6, 11, 6600), 6300U);
// //             }

// //             bool cleanInnerArc = (cnt >= 1 && cnt <= 3 && span <= 3) &&
// //                                  ((circleTurnDir > 0) ? (lastIdx <= 6 && lastLinePos <= 5750)
// //                                                       : (firstIdx >= 5 && lastLinePos >= 5250));
// //             if (el >= ROUND_MID_HOLD_MS && cleanInnerArc) {
// //                 stageConfirmFrames++;
// //             } else {
// //                 stageConfirmFrames = 0;
// //             }

// //             if (stageConfirmFrames >= 4 || el >= ROUND_MID_MAX_MS) {
// //                 roundStage = RND_DONE;
// //                 roundStageMs = nowMs;
// //                 postBoxTurnEnableAt = nowMs + 60;
// //                 armStopAt = max(armStopAt, nowMs + 800);
// //                 startCenteredOnce = true;
// //                 lostNow = false;
// //                 lastEdgeMs = 0;
// //                 pidPos = lastLinePos;
// //                 smoothResetPid = true;
// //                 bleSend("LOG:UPPER_ARC_PASSED");
// //             }
// //         }
// //     }

// //     if (edgeL || edgeR) lastEdgeMs = nowMs;
// //     if (centerOn && !edgeL && !edgeR) startCenteredOnce = true;

// //     if (roundStage >= RND_DONE && cnt >= STOP_MIN_CNT && edgeL && edgeR) {
// //         if (fullBlackFrames < 1000) fullBlackFrames++;
// //         if (fullBlackFrames >= END_CONFIRM && !blackLatched) {
// //             blackLatched = true;
// //             if (nowMs >= armStopAt) {
// //                 cnt2++;
// //                 if (cnt2 >= STOP_CNT2) { finishRun(); return; }
// //             }
// //         }
// //     } else {
// //         fullBlackFrames = 0;
// //         blackLatched = false;
// //     }

// //     if (activeTurnDir != 0) {
// //         if (cnt > 0 && (nowMs < turnHoldUntil || !mid6On)) {
// //             if (activeTurnDir > 0) {
// //                 currentCaseName = "TURN_RIGHT";
// //                 move(TURN_BASE + TURN_OUT, TURN_BASE - TURN_IN);
// //             } else {
// //                 currentCaseName = "TURN_LEFT";
// //                 move(TURN_BASE - TURN_IN, TURN_BASE + TURN_OUT);
// //             }
// //             return;
// //         }
// //         activeTurnDir = 0;
// //     }

// //     if (cnt == 0) {
// //         integral = 0;
// //         dFiltered = 0;
// //         if (!lostNow) {
// //             lostNow = true;
// //             gapStart = nowMs;
// //             bool recentEdge = (lastEdgeMs != 0) && (nowMs - lastEdgeMs <= GAP_EDGE_GUARD_MS);
// //             gapStraight = (roundStage >= RND_DONE) &&
// //                           (nowMs >= postBoxTurnEnableAt) &&
// //                           !recentEdge &&
// //                           (abs(lastError) <= GAP_ERR_MAX) &&
// //                           (dashSprint || cnt1 < GAP_MAX_COUNT);
// //             if (gapStraight) cnt1++;
// //             int maxGapSpd = dashSprint ? max(BASE, DASH_SPRINT_SPEED) : BASE;
// //             gapSpeed = constrain((int)curV, vmin, maxGapSpd);
// //         }
// //         if (gapStraight && (GAP_TIMEOUT_MS == 0 || nowMs - gapStart < GAP_TIMEOUT_MS)) {
// //             if (!dashSprint && (nowMs - gapStart >= DASH_CONFIRM_MS || cnt1 >= 2)) {
// //                 dashSprint = true;
// //                 beep(1, 50, 0);
// //                 bleSend("LOG:DASH_SPRINT_150");
// //             }
// //             if (dashSprint) {
// //                 unsigned long us = micros();
// //                 float dtn = lastPidUs ? (us - lastPidUs) / (float)REF_DT_US : 1.0f;
// //                 lastPidUs = us;
// //                 dtn = constrain(dtn, 0.1f, 4.0f);
// //                 float sprintTarget = (float)max(BASE, DASH_SPRINT_SPEED);
// //                 if (curV < sprintTarget) {
// //                     curV = min(sprintTarget, curV + DASH_ACCEL_PER_S * dtn * REF_DT_US / 1e6f);
// //                 }
// //                 gapSpeed = (int)curV;
// //                 currentCaseName = "DASH_GAP_150";
// //             } else {
// //                 lastPidUs = micros();
// //                 currentCaseName = "GAP_STRAIGHT";
// //             }
// //             move(gapSpeed, gapSpeed);
// //             return;
// //         }
// //         currentCaseName = "LOST_LINE";
// //         if (lastError >= 0) move(LOST_TURN_SPEED, -LOST_TURN_SPEED);
// //         else                move(-LOST_TURN_SPEED, LOST_TURN_SPEED);
// //         return;
// //     }
// //     bool reacquired = lostNow;
// //     lostNow = false;

// //     bool allowTurn90 = (roundStage >= RND_DONE) &&
// //                        (nowMs >= postBoxTurnEnableAt) &&
// //                        (startCenteredOnce || (nowMs >= postBoxTurnEnableAt + 250));

// //     if (allowTurn90 && (pattern & (1 << (NS - 1))) && !(pattern & 1) && cnt >= 3 && cnt <= TURN_MAX_COUNT) {
// //         currentCaseName = "TURN_RIGHT";
// //         activeTurnDir = 1;
// //         turnHoldUntil = nowMs + TURN_HOLD_MS;
// //         lastLinePos = POS[NS - 1];
// //         lastError = POS[NS - 1] - SETPOINT;
// //         lastE = (float)lastError;
// //         curV = min(curV, (float)vmin);
// //         integral = 0; dFiltered = 0;
// //         move(TURN_BASE + TURN_OUT, TURN_BASE - TURN_IN);
// //         return;
// //     }
// //     if (allowTurn90 && (pattern & 1) && !(pattern & (1 << (NS - 1))) && cnt >= 3 && cnt <= TURN_MAX_COUNT) {
// //         currentCaseName = "TURN_LEFT";
// //         activeTurnDir = -1;
// //         turnHoldUntil = nowMs + TURN_HOLD_MS;
// //         lastLinePos = POS[0];
// //         lastError = POS[0] - SETPOINT;
// //         lastE = (float)lastError;
// //         curV = min(curV, (float)vmin);
// //         integral = 0; dFiltered = 0;
// //         move(TURN_BASE - TURN_IN, TURN_BASE + TURN_OUT);
// //         return;
// //     }

// //     float e = (float)pidPos - SETPOINT;
// //     unsigned long us = micros();
// //     float dtn = lastPidUs ? (us - lastPidUs) / (float)REF_DT_US : 1.0f;
// //     lastPidUs = us;
// //     dtn = constrain(dtn, 0.1f, 4.0f);
// //     if (reacquired || smoothResetPid) {
// //         lastE = e;
// //         integral = 0;
// //         dFiltered = 0;
// //     }

// //     if (fabsf(e) < I_ZONE) integral += e * dtn;
// //     else                   integral *= 0.8f;
// //     if (KI > 0) integral = constrain(integral, -I_MAX / KI, I_MAX / KI);
// //     else        integral = 0;

// //     float rawD = KD * (e - lastE) / dtn;
// //     dFiltered = dFiltered + D_FILTER * (rawD - dFiltered);
// //     float steer = KP * e + KI * integral + dFiltered;
// //     lastE = e;
// //     lastError = (int)e;

// //     int activeBase = BASE;
// //     int activeMin  = vmin;
// //     if (roundStage < RND_DONE) {
// //         int cap = (roundStage >= RND_ARC_WAIT_MID) ? ROUND_ARC_SPEED : ROUND_APPROACH_SPEED;
// //         activeBase = min(BASE, cap + 10);
// //         activeMin  = min(vmin, max(65, cap - 10));
// //     } else if (dashSprint) {
// //         activeBase = max(BASE, DASH_SPRINT_SPEED);
// //         activeMin  = max(vmin, DASH_SPRINT_MIN);
// //     }

// //     float f = max(fabsf(e) / (float)max(E_SLOW, 1), fabsf(steer) / STEER_SLOW);
// //     f = constrain(f, 0.0f, 1.0f);
// //     if (dashSprint && f < 0.35f) f = 0.0f;
// //     float target = activeBase - (activeBase - activeMin) * f;
// //     float accelRate = dashSprint ? DASH_ACCEL_PER_S : ACCEL_PER_S;
// //     if (target < curV) curV = target;
// //     else               curV = min(target, curV + accelRate * dtn * REF_DT_US / 1e6f);

// //     if (stageCaseOverride != nullptr) {
// //         currentCaseName = stageCaseOverride;
// //     } else if (dashSprint) {
// //         currentCaseName = "SPRINT_150";
// //     } else {
// //         currentCaseName = (f < 0.2f) ? "STRAIGHT_FAST" : (f > 0.6f ? "SLOW_CURVE" : "PID_FOLLOW");
// //     }
// //     movePID((int)curV, (int)steer);
// // }

// // // ==================== BLE COMMANDS ====================
// // void handleBLECommand(String cmd) {
// //     cmd.trim();
// //     if (cmd.startsWith("SPD:")) {
// //         speed_dk = constrain(cmd.substring(4).toInt(), 30, 255);
// //         char b[30]; snprintf(b, sizeof(b), "LOG:SPD=%d", speed_dk); bleSend(b); return;
// //     }
// //     if (cmd == "TEST") {
// //         testMode = !testMode; runLine = false; stopTimer(); stopMotor();
// //         beep(1, 60, 0);
// //         bleSend(testMode ? "LOG:TEST_ON" : "LOG:TEST_OFF"); return;
// //     }
// //     if (cmd == "CAL") {
// //         startCalibration();
// //         return;
// //     }
// //     if (cmd.startsWith("PID:")) {
// //         float kp, kd, ki = KI; int base;
// //         int n = sscanf(cmd.c_str() + 4, "%f,%f,%d,%f", &kp, &kd, &base, &ki);
// //         if (n >= 3) {
// //             KP = kp; KD = kd; BASE = constrain(base, 0, 255);
// //             if (n == 4) KI = max(ki, 0.0f);
// //             beep(1, 50, 0);
// //             bleSend("LOG:PID_OK");
// //         } else bleSend("LOG:PID_ERR");
// //         return;
// //     }
// //     if (cmd.startsWith("PROF:")) {
// //         int vm, es;
// //         if (sscanf(cmd.c_str() + 5, "%d,%d", &vm, &es) == 2) {
// //             SPEED_MIN = constrain(vm, 0, 255); E_SLOW = constrain(es, 200, 5500);
// //             beep(1, 50, 0);
// //             bleSend("LOG:PROF_OK");
// //         } else bleSend("LOG:PROF_ERR");
// //         return;
// //     }
// //     if (cmd.startsWith("TURN:")) {
// //         int o, i, b;
// //         int n = sscanf(cmd.c_str() + 5, "%d,%d,%d", &o, &i, &b);
// //         if (n >= 2) {
// //             TURN_OUT = constrain(o, 0, 255); TURN_IN = constrain(i, -255, 400);
// //             if (n == 3) TURN_BASE = constrain(b, 0, 255);
// //             beep(1, 50, 0);
// //             bleSend("LOG:TURN_OK");
// //         } else bleSend("LOG:TURN_ERR");
// //         return;
// //     }
// //     if (cmd == "GETPID") {
// //         char b[120];
// //         snprintf(b, sizeof(b), "PID:%.4f,%.4f,%d,%d,%d,%.5f,%d,%d,%d",
// //                  KP, KD, BASE, TURN_OUT, TURN_IN, KI, SPEED_MIN, E_SLOW, TURN_BASE);
// //         bleSend(b); return;
// //     }
// //     if (cmd == "AUTO") {
// //         startAutoRun();
// //         return;
// //     }
// //     if (cmd == "MANUAL") {
// //         if (runLine) hardBrakeStop();
// //         else         stopMotor();
// //         runLine = false; stopTimer();
// //         currentModeName = "MANUAL"; currentCaseName = "IDLE";
// //         beep(1, 100, 0);
// //         bleSend("LOG:MODE_MANUAL"); return;
// //     }
// //     if (!runLine) {
// //         if      (cmd == "FWD") move(speed_dk, speed_dk);
// //         else if (cmd == "BWD") move(-speed_dk, -speed_dk);
// //         else if (cmd == "LFT") move(-speed_dk, speed_dk);
// //         else if (cmd == "RGT") move(speed_dk, -speed_dk);
// //         else if (cmd == "STP") stopMotor();
// //     }
// // }

// // class MyCallbacks : public BLECharacteristicCallbacks {
// //     void onWrite(BLECharacteristic* c) override {
// //         auto v = c->getValue();
// //         if (v.length() > 0) {
// //             portENTER_CRITICAL(&cmdMux);
// //             pendingCmd = String(v.c_str());
// //             hasPendingCmd = true;
// //             portEXIT_CRITICAL(&cmdMux);
// //         }
// //     }
// // };
// // class MyServerCallbacks : public BLEServerCallbacks {
// //     void onConnect(BLEServer*) override { deviceConnected = true; }
// //     void onDisconnect(BLEServer*) override {
// //         deviceConnected = false;
// //         if (!runLine) stopMotor();
// //         BLEDevice::startAdvertising();
// //     }
// // };

// // // ==================== SETUP ====================
// // void setup() {
// //     Serial.begin(115200);

// //     pinMode(BUZZER_PIN, OUTPUT);
// //     setBuzzerPin(false);
// //     pinMode(BTN_PIN, INPUT);

// //     pinMode(STBY, OUTPUT); digitalWrite(STBY, LOW);
// //     pinMode(R_IN1, OUTPUT); pinMode(R_IN2, OUTPUT);
// //     pinMode(L_IN1, OUTPUT); pinMode(L_IN2, OUTPUT);
// //     pwmSetup();
// //     stopMotor();

// //     for (int b = 0; b < 4; b++) pinMode(MUX_S[b], OUTPUT);
// //     pinMode(SIG_PIN, INPUT);
// //     analogReadResolution(12);
// //     analogSetPinAttenuation(SIG_PIN, ADC_11db);
// //     resetCal();

// //     pinMode(ENC_L_A, INPUT); pinMode(ENC_R_A, INPUT);
// //     attachInterrupt(digitalPinToInterrupt(ENC_L_A), isrL, RISING);
// //     attachInterrupt(digitalPinToInterrupt(ENC_R_A), isrR, RISING);

// //     BLEDevice::init("ROBOT_BLE");
// //     BLEDevice::setMTU(517);
// //     pServer = BLEDevice::createServer();
// //     pServer->setCallbacks(new MyServerCallbacks());
// //     BLEService* svc = pServer->createService(SERVICE_UUID);
// //     pTx = svc->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
// //     pTx->addDescriptor(new BLE2902());
// //     BLECharacteristic* rx = svc->createCharacteristic(CHARACTERISTIC_UUID_RX,
// //         BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
// //     rx->setCallbacks(new MyCallbacks());
// //     svc->start();
// //     BLEAdvertising* adv = BLEDevice::getAdvertising();
// //     adv->addServiceUUID(SERVICE_UUID);
// //     adv->setScanResponse(true);
// //     BLEDevice::startAdvertising();

// //     digitalWrite(STBY, HIGH);
// //     beep(2, 70, 60);
// //     Serial.println("READY! Press 1: CAL -> Press 2: AUTO");
// // }

// // // ==================== LOOP ====================
// // void loop() {
// //     updateBuzzer();
// //     updateButton();

// //     if (!deviceConnected && oldDeviceConnected) {
// //         delay(300); BLEDevice::startAdvertising(); oldDeviceConnected = false;
// //     }
// //     if (deviceConnected && !oldDeviceConnected) {
// //         oldDeviceConnected = true;
// //         beep(1, 60, 0);
// //         bleSend("LOG:BLE_CONNECTED");
// //     }

// //     if (hasPendingCmd) {
// //         String cmd;
// //         portENTER_CRITICAL(&cmdMux);
// //         cmd = pendingCmd;
// //         hasPendingCmd = false;
// //         portEXIT_CRITICAL(&cmdMux);
// //         handleBLECommand(cmd);
// //     }

// //     if (calibrating) {
// //         calibrateStep();
// //         if (millis() >= calEnd) {
// //             calibrating = false; calibrated = true;
// //             currentModeName = "IDLE"; currentCaseName = "-";
// //             char bad[64] = "LOG:CAL_WARN mat:";
// //             bool anyBad = false;
// //             for (int c = 0; c < NS; c++) if (!channelOk(c)) {
// //                 char t[5]; snprintf(t, sizeof(t), " %d", c + 1);
// //                 strncat(bad, t, sizeof(bad) - strlen(bad) - 1); anyBad = true;
// //             }
// //             if (anyBad) bleSend(bad);
// //             beep(1, 1000, 0);
// //             bleSend("LOG:CAL_DONE");
// //         }
// //     }

// //     if (runLine)                          lineFollowPID();
// //     else if (calibrated && !calibrating)  docSensor();
// //     else                                  pattern = 0;

// //     if (testMode && !calibrating && !runLine && deviceConnected && millis() - lastRawTime > 200) {
// //         lastRawTime = millis();
// //         char raw[100]; int n = snprintf(raw, sizeof(raw), "RAW:");
// //         for (int i = 0; i < NS; i++)
// //             n += snprintf(raw + n, sizeof(raw) - n, i ? ",%u" : "%u", rawv[i]);
// //         bleSend(raw);
// //     }

// //     sendStatus();
// // }
// // mượt vãi lozzz bên phải
// #include <Arduino.h>
// #include <BLEDevice.h>
// #include <BLEServer.h>
// #include <BLEUtils.h>
// #include <BLE2902.h>

// #define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
// #define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
// #define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// // ==================== BUTTON & BUZZER ====================
// const int BTN_PIN    = 35;     // 3.3V -> Button -> IO35 + 1k pull-down to GND
// const int BUZZER_PIN = 13;     // IO13 -> Buzzer (+) -> GND
// #define BTN_PRESSED_LEVEL  HIGH
// #define BUZZER_ACTIVE_HIGH true
// #define BTN_DEBOUNCE_MS    40
// #define BTN_RECAL_HOLD_MS  1200

// // ==================== MOTOR ====================
// const int R_PWM = 23, R_IN1 = 21, R_IN2 = 22;   // U6 - right
// const int L_PWM = 25, L_IN1 = 27, L_IN2 = 26;   // U7 - left
// const int STBY  = 33;
// #define R_INVERT false
// #define L_INVERT false
// #define PWM_FREQ 20000
// #define PWM_RES  8

// // ==================== ENCODER ====================
// const int ENC_L_A = 4, ENC_R_A = 14;
// volatile long pulseLeft = 0, pulseRight = 0;
// void IRAM_ATTR isrL() { pulseLeft++; }
// void IRAM_ATTR isrR() { pulseRight++; }

// // ==================== 12 SENSORS (CD74HC4067) ====================
// #define NS 12
// const uint8_t MUX_S[4] = {19, 18, 17, 16};
// const uint8_t SIG_PIN  = 34;

// const uint8_t MUX_CH[NS] = {11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
// const uint16_t POS[NS] = {0, 1000, 2000, 3000, 4000, 5000,
//                           6000, 7000, 8000, 9000, 10000, 11000};
// #define SETPOINT 5500
// #define INVERT_COLOR false
// #define SENSOR_THRESHOLD 500
// #define MUX_SETTLE_US 25

// uint16_t calMin[NS], calMax[NS], rawv[NS], sensorVal[NS];
// uint16_t pattern = 0;
// unsigned int lastLinePos = SETPOINT;

// void readRaw() {
//     for (int i = 0; i < NS; i++) {
//         uint8_t ch = MUX_CH[i];
//         for (int b = 0; b < 4; b++) digitalWrite(MUX_S[b], (ch >> b) & 1);
//         delayMicroseconds(MUX_SETTLE_US);
//         analogRead(SIG_PIN);
//         rawv[i] = (analogRead(SIG_PIN) + analogRead(SIG_PIN)) / 2;
//     }
// }
// void resetCal() { for (int i = 0; i < NS; i++) { calMin[i] = 4095; calMax[i] = 0; } }
// void calibrateStep() {
//     readRaw();
//     for (int i = 0; i < NS; i++) {
//         if (rawv[i] < calMin[i]) calMin[i] = rawv[i];
//         if (rawv[i] > calMax[i]) calMax[i] = rawv[i];
//     }
// }
// bool channelOk(int i) { return calMax[i] > calMin[i] + 300; }

// int docSensor() {
//     readRaw();
//     uint32_t s = 0, w = 0;
//     int cnt = 0;
//     pattern = 0;
//     for (int i = 0; i < NS; i++) {
//         int range = (int)calMax[i] - (int)calMin[i];
//         int v = 0;
//         if (range > 50) {
//             v = constrain(((long)rawv[i] - calMin[i]) * 1000 / range, 0, 1000);
//             if (INVERT_COLOR) v = 1000 - v;
//         }
//         sensorVal[i] = (uint16_t)v;
//         if (v > SENSOR_THRESHOLD) { pattern |= (1 << i); cnt++; }
//         if (v > 100) {
//             uint32_t weight = v - 100;
//             s += weight * POS[i];
//             w += weight;
//         }
//     }
//     if (w) lastLinePos = s / w;
//     return cnt;
// }

// unsigned int getMaskedLinePos(int fromIdx, int toIdx, unsigned int fallbackPos) {
//     uint32_t s = 0, w = 0;
//     bool hasStrong = false;
//     for (int i = fromIdx; i <= toIdx; i++) {
//         if (sensorVal[i] > SENSOR_THRESHOLD) hasStrong = true;
//         if (sensorVal[i] > 100) {
//             uint32_t weight = sensorVal[i] - 100;
//             s += weight * POS[i];
//             w += weight;
//         }
//     }
//     return (hasStrong && w) ? (unsigned int)(s / w) : fallbackPos;
// }

// // ==================== PID & SPEED ====================
// float KP = 0.04f, KI = 0.00005f, KD = 0.8f;
// int   BASE      = 100;
// int   SPEED_MIN = 100;
// int   E_SLOW    = 2500;

// int   TURN_BASE = 100, TURN_OUT = 50, TURN_IN = 50;
// #define TURN_MAX_COUNT    8
// #define TURN_HOLD_MS      20
// #define LOST_TURN_SPEED   85    // Toc do xoay tim line cham, binh tinh khi mat vach
// const int BOOST = 15, BOOST_THRESHOLD = 160;

// #define REF_DT_US   4000
// #define D_FILTER    0.4f
// #define I_MAX       30.0f
// #define I_ZONE      2500
// #define STEER_SLOW  100.0f
// #define ACCEL_PER_S 500.0f

// // ==================== START BLACK BOX ====================
// #define START_BOX_SPEED         90    // Toc do chay thang binh tinh khi thoat khoi o START
// #define START_BOX_BLOCK_CNT     6     // >= 6 sensors (or span >= 6) = inside / crossing black block
// #define START_EXIT_CENTER_MS    15    // Confirm single line in center (both outer 2 sensors white)
// #define START_EXIT_NEAR_EDGE_MS 20    // Confirm single line when outermost tips (0 & 11) are white
// #define START_EXIT_EXTREME_MS   35    // Confirm single line if offset onto extreme sensor 0 or 11
// #define START_WHITE_LOST_MS     45    // All-white after black block -> slid off line, pivot back
// #define START_BOX_MAX_MS        2500  // Safety timeout for start-box exit mode
// #define POST_BOX_TURN_GUARD_MS  120   // Suppress false 90-deg turn right at black-box exit
// #define POST_BOX_STOP_GUARD_MS  600   // Extra finish-line lock after leaving start black box

// // ==================== KICH BAN DAU MAP ====================
// // DEFAULT_SIDE_MODE:
// //   1 = Nua san hien tai (Thoat START -> gap goc cong dau BO QUA DI THANG -> vach ngang tren RE TRAI
// //       -> gap nhanh tren 000011100000 / 000111100000 BO QUA di tiep vong cung -> PID -> net dut toc 150 -> DICH)
// //   0 = Nua san cu (Goc cong dau BO QUA DI THANG -> vach ngang tren RE PHAI -> bo qua nhanh tren 000001110000)
// #define ENABLE_ROUNDABOUT_START 1
// #define DEFAULT_SIDE_MODE       1
// #define ROUND_APPROACH_SPEED    85    // Toc do di thang binh tinh toi vach ngang dau
// #define ROUND_ARC_SPEED         85    // Toc do bam cung tron on dinh
// #define ROUND_ARM_LINE_MS       35    // On dinh tren vach thang don (1..3 mat) sau o START truoc khi canh goc cong dau
// #define ROUND_J1_HOLD_MS        85    // Thoi gian KHOA DI THANG toi thieu de bo qua hoan toan goc cong dau
// #define ROUND_J1_MAX_MS         260   // Thoi gian KHOA DI THANG toi da khi vuot goc cong dau
// #define ROUND_T_ARM_MS          40    // Quang nghi ngan sau khi vuot goc cong dau truoc khi canh vach ngang tren
// #define ROUND_PRE_BRAKE_MS      22    // Nhap phanh ngan triet quan tinh truoc khi xoay vao vong tron
// #define ROUND_PRE_BRAKE_PWM     160   // Luc phanh nguoc ngan truoc khi cua
// #define ROUND_PIVOT_OUT_PWM     115   // Banh ngoai tien khi xoay 90 do tai vach ngang vao vong cung
// #define ROUND_PIVOT_IN_PWM     -65    // Banh trong lui de xoay gon tai tam, khong lao thang qua vach ngang
// #define ROUND_TURN_HOLD_MS      145   // Thoi gian xoay toi thieu de tach han khoi vach ngang tren
// #define ROUND_TURN_MAX_MS       550   // Thoi gian xoay toi da khi re vao vong tron
// #define ROUND_TURN_BRAKE_MS     15    // Phanh nguoc chong quan tinh xoay khi vua bat duoc line vong tron
// #define ROUND_ARC_SETTLE_MS     160   // On dinh tren cung tron sau cu re truoc khi canh nhanh cong phia tren
// #define ROUND_MID_HOLD_MS       85    // Thoi gian che mat ngoai toi thieu de luot qua nhanh cong phia tren
// #define ROUND_MID_MAX_MS        250   // Thoi gian che mat ngoai toi da khi bo qua nhanh cong phia tren

// enum RoundStage : uint8_t {
//     RND_ARM_J1        = 0,
//     RND_WAIT_J1       = 1,
//     RND_PASS_J1       = 2,
//     RND_WAIT_CIRCLE_T = 3,
//     RND_TURN_CIRCLE   = 4,
//     RND_ARC_WAIT_MID  = 5,
//     RND_ARC_PASS_MID  = 6,
//     RND_DONE          = 7
// };

// // ==================== GAP, DASH SPRINT & FINISH ====================
// #define GAP_ERR_MAX         1800
// #define GAP_EDGE_GUARD_MS   120
// #define GAP_MAX_COUNT       30      // Cho phep vuot nhieu doan net dut lien tiep
// #define GAP_TIMEOUT_MS      1500

// #define DASH_SPRINT_SPEED   150     // Toc do chay khi gap net dut cho den khi dung o dich
// #define DASH_SPRINT_MIN     130     // Toc do toi thieu neu co khuc cua sau net dut
// #define DASH_CONFIRM_MS     10      // Xac nhan khoang trang net dut that (loc nhieu ADC ngan)
// #define DASH_ACCEL_PER_S    800.0f  // Gia toc muot len toc 150

// #define STOP_MIN_CNT        10
// #define STOP_CNT2           1
// #define END_CONFIRM         2
// #define START_GRACE_MS      1500
// #define BRAKE_REV_SPEED     255
// #define BRAKE_REV_MS        65
// #define SPRINT_BRAKE_REV_MS 95      // Phanh nguoc du luc khi dang chay toc 150 vao DICH

// int  cnt1 = 0, cnt2 = 0;
// bool lostNow = false, gapStraight = false, blackLatched = false;
// bool dashSprint = false;
// int  fullBlackFrames = 0, gapSpeed = 0;
// int  activeTurnDir = 0;
// unsigned long gapStart = 0, armStopAt = 0, lastEdgeMs = 0, turnHoldUntil = 0;

// bool inStartBox = false, sawStartBlack = false, startCenteredOnce = false;
// int  lastBoxSide = 0;
// unsigned long startLineConfirmMs = 0, startWhiteStartMs = 0, postBoxTurnEnableAt = 0;

// uint8_t roundStage = RND_DONE;
// unsigned long roundStageMs = 0;
// int  mapSideMode   = DEFAULT_SIDE_MODE; // 1 = Nua san ben kia (dao logic), 0 = Nua san ban dau, 2 = Auto
// int  j1BranchSide  = 1;   // +1 = goc cong dau ben PHAI (000001111100), -1 = goc cong dau ben TRAI (001111100000)
// int  circleTurnDir = -1;  // -1 = re TRAI theo mui xe (nua ben kia), +1 = re PHAI theo mui xe (nua ban dau)
// int  j1LeftFrames  = 0, j1RightFrames = 0;
// int  stageConfirmFrames = 0, stageWhiteFrames = 0;
// int  arcSettleFrames = 0;

// float integral = 0, lastE = 0, dFiltered = 0, curV = 100;
// unsigned long lastPidUs = 0;
// int   speed_dk = 120;
// int   lastError = 0;

// bool timing = false;
// unsigned long runStartMs = 0, runElapsedMs = 0;
// void startTimer() { runStartMs = millis(); runElapsedMs = 0; timing = true; }
// void stopTimer()  { if (timing) { runElapsedMs = millis() - runStartMs; timing = false; } }

// bool runLine = false, testMode = false;
// bool calibrated = false, calibrating = false;
// unsigned long calEnd = 0, lastRawTime = 0, lastStatusTime = 0;
// #define CAL_TIME 5000
// #define STATUS_INTERVAL 150
// const char* currentModeName = "IDLE";
// const char* currentCaseName = "-";

// BLEServer*         pServer = nullptr;
// BLECharacteristic* pTx     = nullptr;
// bool deviceConnected = false, oldDeviceConnected = false;

// portMUX_TYPE cmdMux = portMUX_INITIALIZER_UNLOCKED;
// String pendingCmd = "";
// volatile bool hasPendingCmd = false;

// // ==================== NON-BLOCKING BUZZER ====================
// int  buzRemain = 0;
// bool buzState  = false;
// unsigned long buzNextMs = 0, buzOnDur = 80, buzOffDur = 60;

// inline void setBuzzerPin(bool on) {
//     digitalWrite(BUZZER_PIN, (on ^ !BUZZER_ACTIVE_HIGH) ? HIGH : LOW);
// }

// void beep(int count, unsigned long onMs = 80, unsigned long offMs = 60) {
//     if (count <= 0) { buzRemain = 0; buzState = false; setBuzzerPin(false); return; }
//     buzRemain = count;
//     buzOnDur  = onMs;
//     buzOffDur = offMs;
//     buzState  = true;
//     setBuzzerPin(true);
//     buzNextMs = millis() + onMs;
// }

// void updateBuzzer() {
//     if (buzRemain <= 0 && !buzState) return;
//     unsigned long now = millis();
//     if ((long)(now - buzNextMs) < 0) return;

//     if (buzState) {
//         buzState = false;
//         setBuzzerPin(false);
//         buzRemain--;
//         if (buzRemain > 0) buzNextMs = now + buzOffDur;
//     } else if (buzRemain > 0) {
//         buzState = true;
//         setBuzzerPin(true);
//         buzNextMs = now + buzOnDur;
//     }
// }

// // ==================== MOTOR DRIVE ====================
// void pwmSetup() {
// #if ESP_ARDUINO_VERSION_MAJOR >= 3
//     ledcAttach(R_PWM, PWM_FREQ, PWM_RES);
//     ledcAttach(L_PWM, PWM_FREQ, PWM_RES);
// #else
//     ledcSetup(0, PWM_FREQ, PWM_RES); ledcAttachPin(R_PWM, 0);
//     ledcSetup(1, PWM_FREQ, PWM_RES); ledcAttachPin(L_PWM, 1);
// #endif
// }
// void pwmWrite(bool right, int duty) {
// #if ESP_ARDUINO_VERSION_MAJOR >= 3
//     ledcWrite(right ? R_PWM : L_PWM, duty);
// #else
//     ledcWrite(right ? 0 : 1, duty);
// #endif
// }
// void setMotor(bool right, int sp) {
//     sp = constrain(sp, -255, 255);
//     bool fwd = (sp >= 0) ^ (right ? R_INVERT : L_INVERT);
//     int in1 = right ? R_IN1 : L_IN1, in2 = right ? R_IN2 : L_IN2;
//     if (sp == 0) {
//         digitalWrite(in1, HIGH);
//         digitalWrite(in2, HIGH);
//         pwmWrite(right, 255);
//     } else {
//         digitalWrite(in1, fwd ? HIGH : LOW);
//         digitalWrite(in2, fwd ? LOW : HIGH);
//         pwmWrite(right, abs(sp));
//     }
// }
// void move(int left, int right) { setMotor(false, left); setMotor(true, right); }
// void stopMotor() { move(0, 0); }

// void hardBrakeStop() {
//     unsigned long brakeMs = dashSprint ? SPRINT_BRAKE_REV_MS : BRAKE_REV_MS;
//     if (brakeMs > 0) {
//         bool encActive = (pulseLeft > 30 && pulseRight > 30);
//         unsigned long minHold = encActive ? (brakeMs * 65 / 100) : brakeMs;
//         move(-BRAKE_REV_SPEED, -BRAKE_REV_SPEED);
//         unsigned long t0 = millis();
//         long lastPL = pulseLeft, lastPR = pulseRight;
//         unsigned long lastMoveMs = t0;
//         while (millis() - t0 < brakeMs) {
//             long pL = pulseLeft, pR = pulseRight;
//             unsigned long now = millis();
//             if (pL != lastPL || pR != lastPR) {
//                 lastPL = pL;
//                 lastPR = pR;
//                 lastMoveMs = now;
//             }
//             if (encActive && (now - t0 >= minHold) && (now - lastMoveMs >= 12)) {
//                 break;
//             }
//             delay(1);
//         }
//     }
//     stopMotor();
// }

// void movePID(int base, int steer) {
//     int L = base + steer, R = base - steer;
//     if (steer >  BOOST_THRESHOLD) L += BOOST;
//     if (steer < -BOOST_THRESHOLD) R += BOOST;
//     move(constrain(L, -255, 255), constrain(R, -255, 255));
// }

// inline void driveSharpTurn(int dir) {
//     // dir > 0: RE PHAI theo mui xe
//     // dir < 0: RE TRAI theo mui xe
//     if (dir > 0) move(TURN_BASE + TURN_OUT, TURN_BASE - TURN_IN);
//     else         move(TURN_BASE - TURN_IN, TURN_BASE + TURN_OUT);
// }

// inline void driveCircleTurn(int dir) {
//     // Xoay tai tam o vach ngang vao vong cung:
//     // dir > 0: RE PHAI (banh trai tien +115, banh phai lui -65)
//     // dir < 0: RE TRAI (banh trai lui -65, banh phai tien +115)
//     if (dir > 0) move(ROUND_PIVOT_OUT_PWM, ROUND_PIVOT_IN_PWM);
//     else         move(ROUND_PIVOT_IN_PWM, ROUND_PIVOT_OUT_PWM);
// }

// // ==================== BLE SEND ====================
// void bleSend(const char* msg) {
//     if (deviceConnected && pTx) {
//         pTx->setValue((uint8_t*)msg, strlen(msg));
//         pTx->notify();
//     }
//     Serial.println(msg);
// }

// void sendStatus() {
//     if (!deviceConnected || millis() - lastStatusTime < STATUS_INTERVAL) return;
//     lastStatusTime = millis();
//     char pat[NS + 1];
//     for (int i = 0; i < NS; i++) pat[i] = (pattern & (1 << i)) ? '1' : '0';
//     pat[NS] = 0;
//     char buf[256];
//     snprintf(buf, sizeof(buf),
//         "{\"mode\":\"%s\",\"case\":\"%s\",\"pos\":%u,\"pat\":\"%s\",\"spd\":%d,\"err\":%d,\"cal\":%d,"
//         "\"c1\":%d,\"c2\":%d,\"v\":%d,\"t\":%lu}",
//         currentModeName, currentCaseName, lastLinePos, pat,
//         speed_dk, lastError, calibrating ? 2 : (calibrated ? 1 : 0), cnt1, cnt2,
//         (int)curV, timing ? (millis() - runStartMs) : runElapsedMs);
//     pTx->setValue((uint8_t*)buf, strlen(buf));
//     pTx->notify();
// }

// // ==================== ACTIONS ====================
// void startCalibration() {
//     runLine = false; testMode = false; stopTimer(); stopMotor();
//     calibrated = false; calibrating = true; calEnd = millis() + CAL_TIME;
//     resetCal(); pattern = 0;
//     currentModeName = "CALIB"; currentCaseName = "CALIBRATING";
//     beep(1, 150, 0);
//     bleSend("LOG:CAL_START");
// }

// void startAutoRun() {
//     if (!calibrated) {
//         beep(2, 150, 100);
//         bleSend("LOG:NEED_CAL");
//         return;
//     }
//     testMode = false; runLine = true;
//     lastError = 0; lastE = 0; integral = 0; dFiltered = 0;
//     lastPidUs = 0; lastLinePos = SETPOINT;
//     pulseLeft = pulseRight = 0;
//     cnt1 = 0; cnt2 = 0;
//     lostNow = false; gapStraight = false;
//     dashSprint = false;
//     blackLatched = false; fullBlackFrames = 0;
//     activeTurnDir = 0; lastEdgeMs = 0; turnHoldUntil = 0;

//     inStartBox = true;
//     sawStartBlack = false;
//     startCenteredOnce = false;
//     lastBoxSide = 0;
//     startLineConfirmMs = 0;
//     startWhiteStartMs = 0;
//     postBoxTurnEnableAt = 0;

//     roundStage = ENABLE_ROUNDABOUT_START ? RND_ARM_J1 : RND_DONE;
//     roundStageMs = millis();
//     if (mapSideMode == 1) {
//         j1BranchSide  = 1;   // Nua ben kia: goc cong dau ben PHAI
//         circleTurnDir = -1;  // -> Vach ngang tren RE TRAI, bo qua nhanh cong ben TRAI
//     } else if (mapSideMode == 0) {
//         j1BranchSide  = -1;  // Nua ban dau: goc cong dau ben TRAI
//         circleTurnDir = 1;   // -> Vach ngang tren RE PHAI, bo qua nhanh cong ben PHAI
//     } else {
//         j1BranchSide  = 0;
//         circleTurnDir = -1;
//     }
//     j1LeftFrames = 0;
//     j1RightFrames = 0;
//     stageConfirmFrames = 0;
//     stageWhiteFrames = 0;
//     arcSettleFrames = 0;

//     curV = min(START_BOX_SPEED, min(SPEED_MIN, BASE));
//     armStopAt = millis() + START_GRACE_MS;
//     startTimer();
//     currentModeName = "AUTO"; currentCaseName = "START_BOX";
//     beep(1, 120, 0);
//     bleSend("LOG:AUTO_START");
// }

// // ==================== BUTTON IO35 ====================
// bool btnLastRaw = false, btnStable = false, btnLongHandled = false;
// unsigned long btnDebounceMs = 0, btnPressStartMs = 0;

// void updateButton() {
//     unsigned long now = millis();
//     bool pressed = (digitalRead(BTN_PIN) == BTN_PRESSED_LEVEL);

//     if (pressed != btnLastRaw) {
//         btnLastRaw = pressed;
//         btnDebounceMs = now;
//     }
//     unsigned long debounceReq = runLine ? 250 : BTN_DEBOUNCE_MS;
//     if (now - btnDebounceMs < debounceReq) return;

//     if (pressed != btnStable) {
//         btnStable = pressed;
//         if (btnStable) {
//             btnPressStartMs = now;
//             btnLongHandled = false;
//             if (runLine && !calibrating) {
//                 btnLongHandled = true;
//                 hardBrakeStop();
//                 runLine = false; stopTimer();
//                 currentModeName = "MANUAL"; currentCaseName = "IDLE";
//                 beep(1, 200, 0);
//                 bleSend("LOG:MODE_MANUAL");
//             }
//         } else {
//             if (!btnLongHandled && !calibrating && !runLine) {
//                 if (!calibrated) startCalibration();
//                 else             startAutoRun();
//             }
//         }
//     } else if (btnStable && !btnLongHandled && !calibrating && !runLine && calibrated) {
//         if (now - btnPressStartMs >= BTN_RECAL_HOLD_MS) {
//             btnLongHandled = true;
//             startCalibration();
//         }
//     }
// }

// void finishRun() {
//     stopTimer();
//     hardBrakeStop();
//     runLine = false;
//     currentModeName = "DONE"; currentCaseName = "FINISH";
//     beep(2, 200, 100);
//     char b[40]; snprintf(b, sizeof(b), "LOG:FINISH %.2fs", runElapsedMs / 1000.0f);
//     bleSend(b);
// }

// // ==================== LINE FOLLOW PID ====================
// void lineFollowPID() {
//     int cnt = docSensor();
//     bool edgeL = (pattern & 0x03) != 0;                // mat 0, 1 (bien TRAI)
//     bool edgeR = (pattern & (0x03 << (NS - 2))) != 0;  // mat 10, 11 (bien PHAI)
//     bool centerOn = (pattern & 0x00F0) != 0;           // 4 mat chinh giua doi xung: mat 4, 5, 6, 7
//     bool mid6On   = (pattern & 0x01F8) != 0;           // 6 mat thang giua: mat 3, 4, 5, 6, 7, 8
//     int  mid6Cnt  = __builtin_popcount(pattern & 0x01F8);
//     bool leftOuter3  = (pattern & 0x0007) != 0;        // 3 mat ngoai ben TRAI: 0, 1, 2
//     bool rightOuter3 = (pattern & 0x0E00) != 0;        // 3 mat ngoai ben PHAI: 9, 10, 11
//     unsigned long nowMs = millis();
//     int vmin = min(SPEED_MIN, BASE);

//     int firstIdx = -1, lastIdx = -1;
//     int leftHalfCnt = 0, rightHalfCnt = 0;
//     for (int i = 0; i < NS; i++) {
//         if (pattern & (1 << i)) {
//             if (firstIdx < 0) firstIdx = i;
//             lastIdx = i;
//             if (i < 6) leftHalfCnt++;
//             else       rightHalfCnt++;
//         }
//     }
//     int span = (cnt > 0) ? (lastIdx - firstIdx + 1) : 0;

//     // Neu bi loa bat trong khoi den xuat phat lam thoat som khi van con trong o START
//     if (!inStartBox && roundStage == RND_ARM_J1 && cnt1 == 0 && (nowMs - runStartMs < START_GRACE_MS) &&
//         (cnt > TURN_MAX_COUNT || (cnt >= 7 && edgeL && edgeR))) {
//         inStartBox = true;
//         sawStartBlack = true;
//         startLineConfirmMs = 0;
//         startWhiteStartMs = 0;
//         activeTurnDir = 0;
//     }

//     // ==================== CHAY THANG THOAT KHOI O VUONG DEN -> BAM PID ====================
//     if (inStartBox) {
//         armStopAt = max(armStopAt, nowMs + POST_BOX_STOP_GUARD_MS);
//         fullBlackFrames = 0;
//         blackLatched = false;
//         activeTurnDir = 0;
//         lostNow = false;
//         lastEdgeMs = 0;
//         roundStageMs = nowMs;

//         if (cnt > 0 && (firstIdx + lastIdx) != (NS - 1)) {
//             lastBoxSide = ((firstIdx + lastIdx) > (NS - 1)) ? 1 : -1;
//         }

//         if (cnt >= START_BOX_BLOCK_CNT || span >= START_BOX_BLOCK_CNT) {
//             sawStartBlack = true;
//             startLineConfirmMs = 0;
//             startWhiteStartMs = 0;
//         }

//         bool clearBothEdges = !edgeL && !edgeR;
//         bool clearOuterTips = ((pattern & 1) == 0) && ((pattern & (1 << (NS - 1))) == 0);
//         unsigned long confirmReqMs = 0;
//         bool singleLineCandidate = false;

//         if (clearBothEdges && cnt >= 1 && cnt <= 5 && span <= 5) {
//             singleLineCandidate = true;
//             confirmReqMs = START_EXIT_CENTER_MS;
//         } else if (clearOuterTips && cnt >= 1 && cnt <= 4 && span <= 4) {
//             singleLineCandidate = true;
//             confirmReqMs = START_EXIT_NEAR_EDGE_MS;
//         } else if (cnt >= 1 && cnt <= 4 && span <= 4) {
//             singleLineCandidate = true;
//             confirmReqMs = START_EXIT_EXTREME_MS;
//         }

//         bool exitedToLine = false;
//         bool slidOffToWhite = false;

//         if (singleLineCandidate) {
//             startWhiteStartMs = 0;
//             if (startLineConfirmMs == 0) startLineConfirmMs = nowMs;
//             if (nowMs - startLineConfirmMs >= confirmReqMs) {
//                 exitedToLine = true;
//             }
//         } else {
//             startLineConfirmMs = 0;
//             if (cnt == 0 && sawStartBlack) {
//                 if (startWhiteStartMs == 0) startWhiteStartMs = nowMs;
//                 else if (nowMs - startWhiteStartMs >= START_WHITE_LOST_MS) {
//                     slidOffToWhite = true;
//                 }
//             } else {
//                 startWhiteStartMs = 0;
//             }
//         }

//         if (!exitedToLine && !slidOffToWhite && (nowMs - runStartMs >= START_BOX_MAX_MS)) {
//             exitedToLine = true;
//         }

//         if (!exitedToLine && !slidOffToWhite) {
//             if (!singleLineCandidate) lastLinePos = SETPOINT;
//             lastError = 0;
//             lastE = 0;
//             integral = 0;
//             dFiltered = 0;

//             unsigned long us = micros();
//             float dtn = lastPidUs ? (us - lastPidUs) / (float)REF_DT_US : 1.0f;
//             lastPidUs = us;
//             dtn = constrain(dtn, 0.1f, 4.0f);

//             float boxTarget = (float)min(BASE, START_BOX_SPEED);
//             if (boxTarget < curV) curV = boxTarget;
//             else                  curV = min(boxTarget, curV + ACCEL_PER_S * dtn * REF_DT_US / 1e6f);

//             currentCaseName = "START_BOX";
//             move((int)curV, (int)curV);
//             return;
//         }

//         inStartBox = false;
//         postBoxTurnEnableAt = nowMs + POST_BOX_TURN_GUARD_MS;
//         roundStageMs = nowMs;
//         integral = 0;
//         dFiltered = 0;
//         lastPidUs = 0;

//         if (slidOffToWhite) {
//             startCenteredOnce = false;
//             lastLinePos = (lastBoxSide >= 0) ? POS[NS - 1] : POS[0];
//             lastError = (int)lastLinePos - SETPOINT;
//             lastE = (float)lastError;
//             lastEdgeMs = nowMs;
//             bleSend("LOG:BOX_RECOVER");
//         } else {
//             startCenteredOnce = (centerOn && !edgeL && !edgeR);
//             float initE = (cnt > 0) ? ((float)lastLinePos - SETPOINT) : 0.0f;
//             lastE = initE;
//             lastError = (int)initE;
//             lastEdgeMs = 0;
//             bleSend("LOG:BOX_EXIT_PID");
//         }
//     }

//     // ==================== XU LY GOC CONG DAU -> RE VONG TRON -> BO QUA VONG CONG PHIA TREN ====================
//     unsigned int pidPos = lastLinePos;
//     bool smoothResetPid = false;
//     const char* stageCaseOverride = nullptr;

//     if (roundStage < RND_DONE) {
//         armStopAt = max(armStopAt, nowMs + 1000);
//         fullBlackFrames = 0;
//         blackLatched = false;
//         activeTurnDir = 0;

//         int straightSpd = min(BASE, ROUND_APPROACH_SPEED);

//         if (roundStage == RND_ARM_J1) {
//             stageCaseOverride = "RND_ARM_LINE";
//             bool singleStraight = (cnt >= 1 && cnt <= 3 && span <= 3 && !edgeL && !edgeR);
//             if (singleStraight) {
//                 if (nowMs - roundStageMs >= ROUND_ARM_LINE_MS) {
//                     // Sau khi thoat START on dinh tren vach don -> vao RND_WAIT_J1 canh goc cong dau de BO QUA va DI THANG
//                     roundStage = RND_WAIT_J1;
//                     roundStageMs = nowMs;
//                     j1LeftFrames = 0;
//                     j1RightFrames = 0;
//                     stageConfirmFrames = 0;
//                     bleSend("LOG:ARM_WAIT_J1");
//                 }
//             } else {
//                 roundStageMs = nowMs;
//             }
//         }
//         else if (roundStage == RND_WAIT_J1) {
//             stageCaseOverride = "RND_WAIT_J1";

//             // Nhan dien goc cong dau ngay khi vach phinh lech 1 ben (001111100000 hoac 000001111100 hoac cnt >= 4)
//             bool wideOrSplit = (cnt >= 4 && span >= 4) || (cnt >= 3 && span >= 5);
//             bool candLeft  = wideOrSplit &&
//                              !rightOuter3 &&
//                              (firstIdx >= 0 && firstIdx <= 4) &&
//                              (lastIdx >= 3 && lastIdx <= 8) &&
//                              ((firstIdx + lastIdx) <= 10) &&
//                              (leftHalfCnt > rightHalfCnt);
//             bool candRight = wideOrSplit &&
//                              !leftOuter3 &&
//                              (lastIdx >= 7 && lastIdx < NS) &&
//                              (firstIdx >= 3 && firstIdx <= 8) &&
//                              ((firstIdx + lastIdx) >= 12) &&
//                              (rightHalfCnt > leftHalfCnt);

//             bool sawJ1 = false;
//             if (mapSideMode == 1) {
//                 // Nua san hien tai: bat ky goc cong dau nao (lech trai 001111100000 hay lech phai hay vach phinh)
//                 // DEU BO QUA VA KHOA DI THANG TIEP, chuan bi len vach ngang tren RE TRAI!
//                 sawJ1 = candLeft || candRight || (wideOrSplit && (edgeL || edgeR || mid6Cnt >= 3));
//                 if (sawJ1) {
//                     j1BranchSide = candRight ? 1 : -1;
//                     circleTurnDir = -1; // Vach ngang tren se RE TRAI
//                 }
//             } else {
//                 // Nua san cu: goc cong dau lech trai -> len vach ngang tren RE PHAI
//                 if (candLeft) {
//                     sawJ1 = true;
//                     j1BranchSide = -1;
//                     circleTurnDir = 1;
//                 } else if (candRight) {
//                     sawJ1 = true;
//                     j1BranchSide = 1;
//                     circleTurnDir = -1;
//                 }
//             }

//             if (sawJ1) {
//                 stageConfirmFrames++;
//                 if (stageConfirmFrames >= 2) {
//                     // Xac nhan dung la goc cong dau: KHOA DI THANG TIEP (BO QUA, KHONG XU LY RE O DAY!)
//                     roundStage = RND_PASS_J1;
//                     roundStageMs = nowMs;
//                     stageConfirmFrames = 0;
//                     beep(1, 45, 0);
//                     bleSend("LOG:IGNORE_J1_GO_STRAIGHT");
//                 }
//                 lastPidUs = micros();
//                 currentCaseName = "IGNORE_J1_PASS";
//                 move(straightSpd, straightSpd);
//                 return;
//             } else {
//                 if (stageConfirmFrames > 0) smoothResetPid = true;
//                 stageConfirmFrames = 0;
//             }
//         }
//         else if (roundStage == RND_PASS_J1) {
//             // Tiep tuc KHOA DI THANG bo qua hoan toan goc cong dau cho den khi tro lai vach don thang
//             unsigned long el = nowMs - roundStageMs;
//             bool cleanSingleLine = (cnt >= 1 && cnt <= 3 && span <= 3) && !leftOuter3 && !rightOuter3;
//             if (el >= ROUND_J1_HOLD_MS && cleanSingleLine) {
//                 stageConfirmFrames++;
//             } else {
//                 stageConfirmFrames = 0;
//             }

//             if (stageConfirmFrames < 3 && el < ROUND_J1_MAX_MS) {
//                 lastPidUs = micros();
//                 currentCaseName = "PASS_J1_STRAIGHT";
//                 move(straightSpd, straightSpd);
//                 return;
//             }

//             // Da vuot qua hoan toan goc cong dau, tiep tuc di thang len VACH NGANG TREN
//             roundStage = RND_WAIT_CIRCLE_T;
//             roundStageMs = nowMs;
//             stageConfirmFrames = 0;
//             stageWhiteFrames = 0;
//             lostNow = false;
//             lastEdgeMs = 0;
//             curV = (float)straightSpd;
//             smoothResetPid = true;
//             bleSend("LOG:TO_TOP_BAR_L");
//             stageCaseOverride = (circleTurnDir > 0) ? "TO_TOP_BAR_R" : "TO_TOP_BAR_L";
//         }

//         if (roundStage == RND_WAIT_CIRCLE_T) {
//             stageCaseOverride = (circleTurnDir > 0) ? "TO_TOP_BAR_R" : "TO_TOP_BAR_L";
//             unsigned long el = nowMs - roundStageMs;

//             if (el >= ROUND_T_ARM_MS) {
//                 bool hitTopBar = (mid6Cnt >= 4) ||
//                                  (cnt >= 4 && span >= 4 && mid6Cnt >= 3) ||
//                                  (cnt >= 5 && span >= 5) ||
//                                  (span >= 6 && leftHalfCnt >= 1 && rightHalfCnt >= 1) ||
//                                  (edgeL && edgeR && cnt >= 4);

//                 if (hitTopBar) {
//                     stageConfirmFrames++;
//                     stageWhiteFrames = 0;
//                     if (stageConfirmFrames >= 2) {
//                         if (ROUND_PRE_BRAKE_MS > 0) {
//                             move(-ROUND_PRE_BRAKE_PWM, -ROUND_PRE_BRAKE_PWM);
//                             delay(ROUND_PRE_BRAKE_MS);
//                         }
//                         roundStage = RND_TURN_CIRCLE;
//                         roundStageMs = millis();
//                         stageConfirmFrames = 0;
//                         integral = 0; dFiltered = 0;
//                         beep(1, 60, 0);
//                         bleSend((circleTurnDir > 0) ? "LOG:TOP_BAR_TURN_R" : "LOG:TOP_BAR_TURN_L");
//                         currentCaseName = (circleTurnDir > 0) ? "CIRCLE_TURN_R" : "CIRCLE_TURN_L";
//                         driveCircleTurn(circleTurnDir);
//                         return;
//                     }
//                     lastPidUs = micros();
//                     currentCaseName = "TOP_BAR_HIT";
//                     move(straightSpd, straightSpd);
//                     return;
//                 } else {
//                     if (stageConfirmFrames > 0) smoothResetPid = true;
//                     stageConfirmFrames = 0;

//                     if (cnt == 0 && el >= 120) {
//                         stageWhiteFrames++;
//                         if (stageWhiteFrames >= 3) {
//                             roundStage = RND_TURN_CIRCLE;
//                             roundStageMs = nowMs;
//                             stageWhiteFrames = 0;
//                             integral = 0; dFiltered = 0;
//                             beep(1, 60, 0);
//                             bleSend((circleTurnDir > 0) ? "LOG:TOP_WHITE_TURN_R" : "LOG:TOP_WHITE_TURN_L");
//                             currentCaseName = (circleTurnDir > 0) ? "CIRCLE_TURN_R" : "CIRCLE_TURN_L";
//                             driveCircleTurn(circleTurnDir);
//                             return;
//                         }
//                         lastPidUs = micros();
//                         currentCaseName = "TOP_CONFIRM_W";
//                         move(60, 60);
//                         return;
//                     }
//                     stageWhiteFrames = 0;
//                 }
//             }
//         }
//         else if (roundStage == RND_TURN_CIRCLE) {
//             // Xoay tai tam vao vong tron theo circleTurnDir (+1 = Re Phai, -1 = Re Trai) -> chuyen sang RND_ARC_WAIT_MID
//             unsigned long el = nowMs - roundStageMs;
//             int maxArcCnt = (el >= 230) ? 4 : 3;
//             bool caughtArc = (el >= ROUND_TURN_HOLD_MS) &&
//                              centerOn && !leftOuter3 && !rightOuter3 &&
//                              (cnt >= 1 && cnt <= maxArcCnt && span <= maxArcCnt);
//             if (!caughtArc && el < ROUND_TURN_MAX_MS) {
//                 currentCaseName = (circleTurnDir > 0) ? "CIRCLE_TURN_R" : "CIRCLE_TURN_L";
//                 driveCircleTurn(circleTurnDir);
//                 return;
//             }
//             if (ROUND_TURN_BRAKE_MS > 0) {
//                 if (circleTurnDir > 0) move(-70, 70);
//                 else                   move(70, -70);
//                 delay(ROUND_TURN_BRAKE_MS);
//                 cnt = docSensor();
//                 edgeL = (pattern & 0x03) != 0;
//                 edgeR = (pattern & (0x03 << (NS - 2))) != 0;
//                 centerOn = (pattern & 0x00F0) != 0;
//                 mid6On   = (pattern & 0x01F8) != 0;
//                 leftOuter3  = (pattern & 0x0007) != 0;
//                 rightOuter3 = (pattern & 0x0E00) != 0;
//                 firstIdx = -1; lastIdx = -1;
//                 leftHalfCnt = 0; rightHalfCnt = 0;
//                 for (int i = 0; i < NS; i++) {
//                     if (pattern & (1 << i)) {
//                         if (firstIdx < 0) firstIdx = i;
//                         lastIdx = i;
//                         if (i < 6) leftHalfCnt++; else rightHalfCnt++;
//                     }
//                 }
//                 span = (cnt > 0) ? (lastIdx - firstIdx + 1) : 0;
//             }
//             nowMs = millis();
//             roundStage = RND_ARC_WAIT_MID;
//             roundStageMs = nowMs;
//             stageConfirmFrames = 0;
//             stageWhiteFrames = 0;
//             arcSettleFrames = 0;
//             startCenteredOnce = true;
//             lostNow = false;
//             lastEdgeMs = (cnt == 0) ? nowMs : 0;
//             // Neu cnt == 0 ngay sau khi xoay, tiep tuc tim line theo dung chieu circleTurnDir (-1 = Re Trai < 0)
//             lastError = (cnt > 0) ? ((int)lastLinePos - SETPOINT) : ((circleTurnDir > 0) ? 3500 : -3500);
//             lastE = (float)lastError;
//             lastPidUs = 0;
//             curV = (float)min(BASE, ROUND_ARC_SPEED);
//             smoothResetPid = true;
//             pidPos = lastLinePos;
//             bleSend("LOG:CIRCLE_ARC_START");
//             stageCaseOverride = "CIRCLE_ARC";
//         }
//         else if (roundStage == RND_ARC_WAIT_MID) {
//             // Dang om vong cong: canh nhanh vong cong phia tren
//             stageCaseOverride = "CIRCLE_ARC";
//             unsigned long el = nowMs - roundStageMs;

//             bool huggingInner = (cnt >= 1 && cnt <= 3 && span <= 3 && !leftOuter3 && !rightOuter3) &&
//                                 ((circleTurnDir > 0) ? (lastLinePos <= 5800)
//                                                      : (lastLinePos >= 5200));
//             if (huggingInner && arcSettleFrames < 100) arcSettleFrames++;

//             if (cnt == 0) {
//                 // Neu di qua xa tren cung tron (> 700ms) ma gap trang tinh -> da toi dau mut cuoi cung tron, tra ve PID
//                 if (el >= 700) {
//                     stageWhiteFrames++;
//                     if (stageWhiteFrames >= 4) {
//                         roundStage = RND_DONE;
//                         roundStageMs = nowMs;
//                         postBoxTurnEnableAt = nowMs;
//                         lastEdgeMs = nowMs;
//                         lastError = (circleTurnDir > 0) ? 3500 : -3500;
//                         lastE = (float)lastError;
//                         bleSend("LOG:ARC_WHITE_DONE");
//                     }
//                 } else if (arcSettleFrames < 2) {
//                     // Chua bam on dinh vao cung tron -> tiep tuc xoay theo dung huong circleTurnDir (-1 = Trai, +1 = Phai)
//                     lastError = (circleTurnDir > 0) ? 3500 : -3500;
//                 } else {
//                     // Da bam tren cung tron ma truot ra mep ngoai -> uon nguoc vao long cung tron
//                     lastError = (circleTurnDir > 0) ? -3000 : 3000;
//                 }
//             } else {
//                 stageWhiteFrames = 0;
//             }

//             bool armedForMid = (el >= ROUND_ARC_SETTLE_MS) && (arcSettleFrames >= 5 || el >= 380);
//             if (armedForMid && cnt > 0) {
//                 bool sawMidBranch = false;
//                 if (circleTurnDir > 0) {
//                     // Cung tron uon TRAI (mat <= 5), nhanh cong phia tren nho ra PHAI (000001110000 / 000001111000 / mat 8..11)
//                     sawMidBranch = !edgeL && (
//                         (cnt >= 3 && firstIdx >= 5 && lastIdx >= 7 && lastLinePos >= 5850) ||
//                         (cnt >= 4 && span >= 4 && lastIdx >= 7) ||
//                         (cnt >= 2 && span >= 4 && lastIdx >= 7) ||
//                         ((pattern & 0x0F00) != 0)
//                     );
//                 } else {
//                     // Vong cung tren (circleTurnDir < 0): cung tron uon PHAI (mat >= 6), nhanh tren nho ra TRAI
//                     // Bat chinh xac mau 000011100000 (pos~4987) va 000111100000 (pos~4398) de bo qua nhanh va di tiep vong cung
//                     sawMidBranch = !edgeR && (
//                         (cnt >= 3 && lastIdx <= 6 && firstIdx <= 4 && lastLinePos <= 5300) ||
//                         (cnt >= 4 && span >= 4 && firstIdx <= 4) ||
//                         (cnt >= 2 && span >= 4 && firstIdx <= 4) ||
//                         ((pattern & 0x0007) != 0)
//                     );
//                 }

//                 if (sawMidBranch) {
//                     if (stageConfirmFrames == 0) smoothResetPid = true;
//                     // Che han cum mat phia ngoai (6..11 hoac 0..5), giu xe tiep tuc bam muot theo mep trong vong tron!
//                     if (circleTurnDir > 0) {
//                         pidPos = min(getMaskedLinePos(0, 5, 4400), 4700U);
//                     } else {
//                         pidPos = max(getMaskedLinePos(6, 11, 6600), 6300U);
//                     }
//                     stageCaseOverride = "SKIP_UPPER_ARC";
//                     stageConfirmFrames++;
//                     if (stageConfirmFrames >= 2) {
//                         roundStage = RND_ARC_PASS_MID;
//                         roundStageMs = nowMs;
//                         stageConfirmFrames = 0;
//                         beep(1, 45, 0);
//                         bleSend("LOG:SKIP_UPPER_ARC");
//                     }
//                 } else {
//                     if (stageConfirmFrames > 0) smoothResetPid = true;
//                     stageConfirmFrames = 0;
//                 }
//             }
//         }
//         else if (roundStage == RND_ARC_PASS_MID) {
//             // Tiep tuc che mat phia ngoai de bo qua nhanh vong cong phia tren va chay tiep tren cung tron
//             stageCaseOverride = "SKIP_UPPER_ARC";
//             unsigned long el = nowMs - roundStageMs;

//             if (circleTurnDir > 0) {
//                 pidPos = min(getMaskedLinePos(0, 5, 4400), 4700U);
//             } else {
//                 pidPos = max(getMaskedLinePos(6, 11, 6600), 6300U);
//             }

//             bool cleanInnerArc = (cnt >= 1 && cnt <= 3 && span <= 3) &&
//                                  ((circleTurnDir > 0) ? (lastIdx <= 6 && lastLinePos <= 5750)
//                                                       : (firstIdx >= 5 && lastLinePos >= 5250));
//             if (el >= ROUND_MID_HOLD_MS && cleanInnerArc) {
//                 stageConfirmFrames++;
//             } else {
//                 stageConfirmFrames = 0;
//             }

//             if (stageConfirmFrames >= 4 || el >= ROUND_MID_MAX_MS) {
//                 roundStage = RND_DONE;
//                 roundStageMs = nowMs;
//                 postBoxTurnEnableAt = nowMs + 60;
//                 armStopAt = max(armStopAt, nowMs + 800);
//                 startCenteredOnce = true;
//                 lostNow = false;
//                 lastEdgeMs = 0;
//                 pidPos = lastLinePos;
//                 smoothResetPid = true;
//                 bleSend("LOG:UPPER_ARC_PASSED");
//             }
//         }
//     }

//     if (edgeL || edgeR) lastEdgeMs = nowMs;
//     if (centerOn && !edgeL && !edgeR) startCenteredOnce = true;

//     if (roundStage >= RND_DONE && cnt >= STOP_MIN_CNT && edgeL && edgeR) {
//         if (fullBlackFrames < 1000) fullBlackFrames++;
//         if (fullBlackFrames >= END_CONFIRM && !blackLatched) {
//             blackLatched = true;
//             if (nowMs >= armStopAt) {
//                 cnt2++;
//                 if (cnt2 >= STOP_CNT2) { finishRun(); return; }
//             }
//         }
//     } else {
//         fullBlackFrames = 0;
//         blackLatched = false;
//     }

//     if (activeTurnDir != 0) {
//         if (cnt > 0 && (nowMs < turnHoldUntil || !mid6On)) {
//             if (activeTurnDir > 0) {
//                 currentCaseName = "TURN_RIGHT";
//                 move(TURN_BASE + TURN_OUT, TURN_BASE - TURN_IN);
//             } else {
//                 currentCaseName = "TURN_LEFT";
//                 move(TURN_BASE - TURN_IN, TURN_BASE + TURN_OUT);
//             }
//             return;
//         }
//         activeTurnDir = 0;
//     }

//     if (cnt == 0) {
//         integral = 0;
//         dFiltered = 0;
//         if (!lostNow) {
//             lostNow = true;
//             gapStart = nowMs;
//             bool recentEdge = (lastEdgeMs != 0) && (nowMs - lastEdgeMs <= GAP_EDGE_GUARD_MS);
//             gapStraight = (roundStage >= RND_DONE) &&
//                           (nowMs >= postBoxTurnEnableAt) &&
//                           !recentEdge &&
//                           (abs(lastError) <= GAP_ERR_MAX) &&
//                           (dashSprint || cnt1 < GAP_MAX_COUNT);
//             if (gapStraight) cnt1++;
//             int maxGapSpd = dashSprint ? max(BASE, DASH_SPRINT_SPEED) : BASE;
//             gapSpeed = constrain((int)curV, vmin, maxGapSpd);
//         }
//         if (gapStraight && (GAP_TIMEOUT_MS == 0 || nowMs - gapStart < GAP_TIMEOUT_MS)) {
//             if (!dashSprint && (nowMs - gapStart >= DASH_CONFIRM_MS || cnt1 >= 2)) {
//                 dashSprint = true;
//                 beep(1, 50, 0);
//                 bleSend("LOG:DASH_SPRINT_150");
//             }
//             if (dashSprint) {
//                 unsigned long us = micros();
//                 float dtn = lastPidUs ? (us - lastPidUs) / (float)REF_DT_US : 1.0f;
//                 lastPidUs = us;
//                 dtn = constrain(dtn, 0.1f, 4.0f);
//                 float sprintTarget = (float)max(BASE, DASH_SPRINT_SPEED);
//                 if (curV < sprintTarget) {
//                     curV = min(sprintTarget, curV + DASH_ACCEL_PER_S * dtn * REF_DT_US / 1e6f);
//                 }
//                 gapSpeed = (int)curV;
//                 currentCaseName = "DASH_GAP_150";
//             } else {
//                 lastPidUs = micros();
//                 currentCaseName = "GAP_STRAIGHT";
//             }
//             move(gapSpeed, gapSpeed);
//             return;
//         }
//         currentCaseName = "LOST_LINE";
//         if (lastError >= 0) move(LOST_TURN_SPEED, -LOST_TURN_SPEED);
//         else                move(-LOST_TURN_SPEED, LOST_TURN_SPEED);
//         return;
//     }
//     bool reacquired = lostNow;
//     lostNow = false;

//     bool allowTurn90 = (roundStage >= RND_DONE) &&
//                        (nowMs >= postBoxTurnEnableAt) &&
//                        (startCenteredOnce || (nowMs >= postBoxTurnEnableAt + 250));

//     if (allowTurn90 && (pattern & (1 << (NS - 1))) && !(pattern & 1) && cnt >= 3 && cnt <= TURN_MAX_COUNT) {
//         currentCaseName = "TURN_RIGHT";
//         activeTurnDir = 1;
//         turnHoldUntil = nowMs + TURN_HOLD_MS;
//         lastLinePos = POS[NS - 1];
//         lastError = POS[NS - 1] - SETPOINT;
//         lastE = (float)lastError;
//         curV = min(curV, (float)vmin);
//         integral = 0; dFiltered = 0;
//         move(TURN_BASE + TURN_OUT, TURN_BASE - TURN_IN);
//         return;
//     }
//     if (allowTurn90 && (pattern & 1) && !(pattern & (1 << (NS - 1))) && cnt >= 3 && cnt <= TURN_MAX_COUNT) {
//         currentCaseName = "TURN_LEFT";
//         activeTurnDir = -1;
//         turnHoldUntil = nowMs + TURN_HOLD_MS;
//         lastLinePos = POS[0];
//         lastError = POS[0] - SETPOINT;
//         lastE = (float)lastError;
//         curV = min(curV, (float)vmin);
//         integral = 0; dFiltered = 0;
//         move(TURN_BASE - TURN_IN, TURN_BASE + TURN_OUT);
//         return;
//     }

//     float e = (float)pidPos - SETPOINT;
//     unsigned long us = micros();
//     float dtn = lastPidUs ? (us - lastPidUs) / (float)REF_DT_US : 1.0f;
//     lastPidUs = us;
//     dtn = constrain(dtn, 0.1f, 4.0f);
//     if (reacquired || smoothResetPid) {
//         lastE = e;
//         integral = 0;
//         dFiltered = 0;
//     }

//     if (fabsf(e) < I_ZONE) integral += e * dtn;
//     else                   integral *= 0.8f;
//     if (KI > 0) integral = constrain(integral, -I_MAX / KI, I_MAX / KI);
//     else        integral = 0;

//     float rawD = KD * (e - lastE) / dtn;
//     dFiltered = dFiltered + D_FILTER * (rawD - dFiltered);
//     float steer = KP * e + KI * integral + dFiltered;
//     lastE = e;
//     lastError = (int)e;

//     int activeBase = BASE;
//     int activeMin  = vmin;
//     if (roundStage < RND_DONE) {
//         int cap = (roundStage >= RND_ARC_WAIT_MID) ? ROUND_ARC_SPEED : ROUND_APPROACH_SPEED;
//         activeBase = min(BASE, cap + 10);
//         activeMin  = min(vmin, max(65, cap - 10));
//     } else if (dashSprint) {
//         activeBase = max(BASE, DASH_SPRINT_SPEED);
//         activeMin  = max(vmin, DASH_SPRINT_MIN);
//     }

//     float f = max(fabsf(e) / (float)max(E_SLOW, 1), fabsf(steer) / STEER_SLOW);
//     f = constrain(f, 0.0f, 1.0f);
//     if (dashSprint && f < 0.35f) f = 0.0f;
//     float target = activeBase - (activeBase - activeMin) * f;
//     float accelRate = dashSprint ? DASH_ACCEL_PER_S : ACCEL_PER_S;
//     if (target < curV) curV = target;
//     else               curV = min(target, curV + accelRate * dtn * REF_DT_US / 1e6f);

//     if (stageCaseOverride != nullptr) {
//         currentCaseName = stageCaseOverride;
//     } else if (dashSprint) {
//         currentCaseName = "SPRINT_150";
//     } else {
//         currentCaseName = (f < 0.2f) ? "STRAIGHT_FAST" : (f > 0.6f ? "SLOW_CURVE" : "PID_FOLLOW");
//     }
//     movePID((int)curV, (int)steer);
// }

// // ==================== BLE COMMANDS ====================
// void handleBLECommand(String cmd) {
//     cmd.trim();
//     if (cmd.startsWith("SPD:")) {
//         speed_dk = constrain(cmd.substring(4).toInt(), 30, 255);
//         char b[30]; snprintf(b, sizeof(b), "LOG:SPD=%d", speed_dk); bleSend(b); return;
//     }
//     if (cmd.startsWith("SIDE:")) {
//         mapSideMode = constrain(cmd.substring(5).toInt(), 0, 2);
//         beep(1, 60, 0);
//         char b[32]; snprintf(b, sizeof(b), "LOG:SIDE=%d", mapSideMode); bleSend(b); return;
//     }
//     if (cmd == "TEST") {
//         testMode = !testMode; runLine = false; stopTimer(); stopMotor();
//         beep(1, 60, 0);
//         bleSend(testMode ? "LOG:TEST_ON" : "LOG:TEST_OFF"); return;
//     }
//     if (cmd == "CAL") {
//         startCalibration();
//         return;
//     }
//     if (cmd.startsWith("PID:")) {
//         float kp, kd, ki = KI; int base;
//         int n = sscanf(cmd.c_str() + 4, "%f,%f,%d,%f", &kp, &kd, &base, &ki);
//         if (n >= 3) {
//             KP = kp; KD = kd; BASE = constrain(base, 0, 255);
//             if (n == 4) KI = max(ki, 0.0f);
//             beep(1, 50, 0);
//             bleSend("LOG:PID_OK");
//         } else bleSend("LOG:PID_ERR");
//         return;
//     }
//     if (cmd.startsWith("PROF:")) {
//         int vm, es;
//         if (sscanf(cmd.c_str() + 5, "%d,%d", &vm, &es) == 2) {
//             SPEED_MIN = constrain(vm, 0, 255); E_SLOW = constrain(es, 200, 5500);
//             beep(1, 50, 0);
//             bleSend("LOG:PROF_OK");
//         } else bleSend("LOG:PROF_ERR");
//         return;
//     }
//     if (cmd.startsWith("TURN:")) {
//         int o, i, b;
//         int n = sscanf(cmd.c_str() + 5, "%d,%d,%d", &o, &i, &b);
//         if (n >= 2) {
//             TURN_OUT = constrain(o, 0, 255); TURN_IN = constrain(i, -255, 400);
//             if (n == 3) TURN_BASE = constrain(b, 0, 255);
//             beep(1, 50, 0);
//             bleSend("LOG:TURN_OK");
//         } else bleSend("LOG:TURN_ERR");
//         return;
//     }
//     if (cmd == "GETPID") {
//         char b[120];
//         snprintf(b, sizeof(b), "PID:%.4f,%.4f,%d,%d,%d,%.5f,%d,%d,%d",
//                  KP, KD, BASE, TURN_OUT, TURN_IN, KI, SPEED_MIN, E_SLOW, TURN_BASE);
//         bleSend(b); return;
//     }
//     if (cmd == "AUTO") {
//         startAutoRun();
//         return;
//     }
//     if (cmd == "MANUAL") {
//         if (runLine) hardBrakeStop();
//         else         stopMotor();
//         runLine = false; stopTimer();
//         currentModeName = "MANUAL"; currentCaseName = "IDLE";
//         beep(1, 100, 0);
//         bleSend("LOG:MODE_MANUAL"); return;
//     }
//     if (!runLine) {
//         if      (cmd == "FWD") move(speed_dk, speed_dk);
//         else if (cmd == "BWD") move(-speed_dk, -speed_dk);
//         else if (cmd == "LFT") move(-speed_dk, speed_dk);
//         else if (cmd == "RGT") move(speed_dk, -speed_dk);
//         else if (cmd == "STP") stopMotor();
//     }
// }

// class MyCallbacks : public BLECharacteristicCallbacks {
//     void onWrite(BLECharacteristic* c) override {
//         auto v = c->getValue();
//         if (v.length() > 0) {
//             portENTER_CRITICAL(&cmdMux);
//             pendingCmd = String(v.c_str());
//             hasPendingCmd = true;
//             portEXIT_CRITICAL(&cmdMux);
//         }
//     }
// };
// class MyServerCallbacks : public BLEServerCallbacks {
//     void onConnect(BLEServer*) override { deviceConnected = true; }
//     void onDisconnect(BLEServer*) override {
//         deviceConnected = false;
//         if (!runLine) stopMotor();
//         BLEDevice::startAdvertising();
//     }
// };

// // ==================== SETUP ====================
// void setup() {
//     Serial.begin(115200);

//     pinMode(BUZZER_PIN, OUTPUT);
//     setBuzzerPin(false);
//     pinMode(BTN_PIN, INPUT);

//     pinMode(STBY, OUTPUT); digitalWrite(STBY, LOW);
//     pinMode(R_IN1, OUTPUT); pinMode(R_IN2, OUTPUT);
//     pinMode(L_IN1, OUTPUT); pinMode(L_IN2, OUTPUT);
//     pwmSetup();
//     stopMotor();

//     for (int b = 0; b < 4; b++) pinMode(MUX_S[b], OUTPUT);
//     pinMode(SIG_PIN, INPUT);
//     analogReadResolution(12);
//     analogSetPinAttenuation(SIG_PIN, ADC_11db);
//     resetCal();

//     pinMode(ENC_L_A, INPUT); pinMode(ENC_R_A, INPUT);
//     attachInterrupt(digitalPinToInterrupt(ENC_L_A), isrL, RISING);
//     attachInterrupt(digitalPinToInterrupt(ENC_R_A), isrR, RISING);

//     BLEDevice::init("ROBOT_BLE");
//     BLEDevice::setMTU(517);
//     pServer = BLEDevice::createServer();
//     pServer->setCallbacks(new MyServerCallbacks());
//     BLEService* svc = pServer->createService(SERVICE_UUID);
//     pTx = svc->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
//     pTx->addDescriptor(new BLE2902());
//     BLECharacteristic* rx = svc->createCharacteristic(CHARACTERISTIC_UUID_RX,
//         BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
//     rx->setCallbacks(new MyCallbacks());
//     svc->start();
//     BLEAdvertising* adv = BLEDevice::getAdvertising();
//     adv->addServiceUUID(SERVICE_UUID);
//     adv->setScanResponse(true);
//     BLEDevice::startAdvertising();

//     digitalWrite(STBY, HIGH);
//     beep(2, 70, 60);
//     Serial.println("READY! Press 1: CAL -> Press 2: AUTO");
// }

// // ==================== LOOP ====================
// void loop() {
//     updateBuzzer();
//     updateButton();

//     if (!deviceConnected && oldDeviceConnected) {
//         delay(300); BLEDevice::startAdvertising(); oldDeviceConnected = false;
//     }
//     if (deviceConnected && !oldDeviceConnected) {
//         oldDeviceConnected = true;
//         beep(1, 60, 0);
//         bleSend("LOG:BLE_CONNECTED");
//     }

//     if (hasPendingCmd) {
//         String cmd;
//         portENTER_CRITICAL(&cmdMux);
//         cmd = pendingCmd;
//         hasPendingCmd = false;
//         portEXIT_CRITICAL(&cmdMux);
//         handleBLECommand(cmd);
//     }

//     if (calibrating) {
//         calibrateStep();
//         if (millis() >= calEnd) {
//             calibrating = false; calibrated = true;
//             currentModeName = "IDLE"; currentCaseName = "-";
//             char bad[64] = "LOG:CAL_WARN mat:";
//             bool anyBad = false;
//             for (int c = 0; c < NS; c++) if (!channelOk(c)) {
//                 char t[5]; snprintf(t, sizeof(t), " %d", c + 1);
//                 strncat(bad, t, sizeof(bad) - strlen(bad) - 1); anyBad = true;
//             }
//             if (anyBad) bleSend(bad);
//             beep(1, 1000, 0);
//             bleSend("LOG:CAL_DONE");
//         }
//     }

//     if (runLine)                          lineFollowPID();
//     else if (calibrated && !calibrating)  docSensor();
//     else                                  pattern = 0;

//     if (testMode && !calibrating && !runLine && deviceConnected && millis() - lastRawTime > 200) {
//         lastRawTime = millis();
//         char raw[100]; int n = snprintf(raw, sizeof(raw), "RAW:");
//         for (int i = 0; i < NS; i++)
//             n += snprintf(raw + n, sizeof(raw) - n, i ? ",%u" : "%u", rawv[i]);
//         bleSend(raw);
//     }

//     sendStatus();
// }
// code bên trái
