/**
 * @file    MYO_EMG
 * @author  Alex Fricke, Marwan A Rahman, Martina Nobile, William Kwong (original code from Kira Wadden)
 * @date    Nov 2019
 * @brief   Communicating between the Myo armband and ESP32 via BLE to receive Gesture classifier notifications
 */

#include <BLEDevice.h>


// The remote service we wish to connect to.
static BLEUUID serviceUUID("d5060001-a904-deb9-4748-2c7f4a124842");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("d5060401-a904-deb9-4748-2c7f4a124842");

// EMG service UUID
static BLEUUID    emgSUUID("d5060005-a904-deb9-4748-2c7f4a124842");
// EMG characteristic UUID 
static BLEUUID    emgCUUID("d5060105-a904-deb9-4748-2c7f4a124842");
// Classifier characteristic UUID 2
static BLEUUID    classifierCUUID("d5060305-a904-deb9-4748-2c7f4a124842");

static BLEAddress *pServerAddress;
static boolean doConnect = false;
static boolean connected = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    // Print the gesture
    if (pData[0] == 0x03) {
      //Serial.print ("Gesture: ");
      switch (pData[1]) {
        case 0x0000:
          Serial.println ("rest");
          break;
        case 0x0001:
          Serial.println ("fist");
          break;
        case 0x0002:
          Serial.println ("wave in");
          break;
        case 0x0003:
          Serial.println ("wave out");
          break;
        case 0x0004:
          Serial.println ("fingers spread");
          break;
        case 0x0005:
          Serial.println ("double tap");
          break;
        case 0xffff:
          Serial.println ("unknown");
          break;
        default:
          break;
      }
    }
}

bool connectToServer(BLEAddress pAddress) {
    Serial.print("Forming a connection to ");
    Serial.println(pAddress.toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    // Connect to the remove BLE Server.
    pClient->connect(pAddress);
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      return false;
    }
    Serial.println(" - Found our service");
    
    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      return false;
    }
    Serial.println(" - Found our characteristic");

    // set sleep mode
    uint8_t sleepPkt[3] = {0x09, 0x01, 0x01};
    pRemoteCharacteristic->writeValue(sleepPkt, 3, true);
    delay(500);

    // set EMG mode to send filtered, and classifier mode to be on
    uint8_t emgPkt[5] = {0x01, 0x03, 0x02, 0x00, 0x01 }; 
    pRemoteCharacteristic->writeValue(emgPkt, 5, true);
    delay(500);


    const uint8_t notificationOn[] = {0x01, 0x00};

    // Obtain reference to EMG service UUID
    pRemoteService = pClient->getService(emgSUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(emgSUUID.toString().c_str());
      return false;
    }
    Serial.println(" - Found our EMG service");
    Serial.println(emgSUUID.toString().c_str());
    
// Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(classifierCUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(classifierCUUID.toString().c_str());
      return false;
    }
    Serial.println(" - Found our EMG characteristic");
    Serial.println(classifierCUUID.toString().c_str());
    
    pRemoteCharacteristic->registerForNotify(notifyCallback);
    pRemoteCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(serviceUUID)) {

      // 
      Serial.print("Found our device!  address: "); 
      advertisedDevice.getScan()->stop();

      pServerAddress = new BLEAddress(advertisedDevice.getAddress());
      doConnect = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks



//bool checkDoubleFlex() {
//    if (triggerPattern[findIndexInTriggerPattern(triggerPatternTail - 1)] && !triggerPattern[findIndexInTriggerPattern(triggerPatternTail - 2)] && triggerPattern[findIndexInTriggerPattern(triggerPatternTail - 3)])
//      return true;
//    
//    return false;
//  }
//
//  int findIndexInTriggerPattern(int index){
//    if (index >= 0) return index;
//    else {
//      return 3 + index;
//    }
//  }

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 30 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);
} // End of setup.


// This is the Arduino main loop functio
void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer(*pServerAddress)) {
      Serial.println("We are now connected to the BLE Server.");
      connected = true;
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }
  delay(1000);
} // End of loop
