#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// BLE UART Service UUIDs (Nordic NUS - works with most BLE terminal apps)
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLEServer* pServer = NULL;
BLECharacteristic* pTxCharacteristic;
bool deviceConnected = false;

void driveForward();
void driveBackward();
void steerLeft();
void steerRight();
void stopDrive();
void stopSteer();
void stopAll();

// Motor 1 - DRIVE (forward/backward)
#define IN1 26
#define IN2 27
#define ENA 32

// Motor 2 - STEERING (left/right)
#define IN3 14
#define IN4 13
#define ENB 33

#define DRIVE_SPEED 200
#define STEER_SPEED 180

// --- BLE Callbacks ---
class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Phone connected!");
  }
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    stopAll();
    Serial.println("Phone disconnected. Advertising again...");
    BLEDevice::startAdvertising();
  }
};

class RxCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    String rxValue = pCharacteristic->getValue();
    if (rxValue.length() > 0) {
      char cmd = rxValue[0];
      Serial.print("Command: ");
      Serial.println(cmd);

      switch (cmd) {
        case 'F': driveForward();  break;
        case 'B': driveBackward(); break;
        case 'L': steerLeft();     break;
        case 'R': steerRight();    break;
        case 'S': stopAll();       break;
        case 'G': driveForward();  steerLeft();  break;
        case 'I': driveForward();  steerRight(); break;
        case 'H': driveBackward(); steerLeft();  break;
        case 'J': driveBackward(); steerRight(); break;
      }
    }
  }
};

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  stopAll();

  // BLE Setup
  BLEDevice::init("ESP32_Mufasa_Running_Club");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);

  pTxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic* pRxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    BLECharacteristic::PROPERTY_WRITE
  );
  pRxCharacteristic->setCallbacks(new RxCallbacks());

  pService->start();
  BLEDevice::startAdvertising();

  Serial.println("BLE started. Waiting for iPhone connection...");
}

void loop() {
  // Nothing needed here - commands handled in BLE callbacks
  delay(10);
}

// --- DRIVE MOTOR (Motor 1) ---
void driveForward() {
  analogWrite(ENA, DRIVE_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  Serial.println("Drive: Forward");
}

void driveBackward() {
  analogWrite(ENA, DRIVE_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  Serial.println("Drive: Backward");
}

void stopDrive() {
  analogWrite(ENA, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

// --- STEERING MOTOR (Motor 2) ---
void steerLeft() {
  analogWrite(ENB, STEER_SPEED);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Steer: Left");
}

void steerRight() {
  analogWrite(ENB, STEER_SPEED);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Steer: Right");
}

void stopSteer() {
  analogWrite(ENB, 0);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// --- STOP ALL ---
void stopAll() {
  stopDrive();
  stopSteer();
  Serial.println("Stopped");
}