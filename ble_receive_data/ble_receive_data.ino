#include "CurieBle.h"

static const char* bluetoothDeviceName = "MyDevice"; 

static const char* serviceUuid = "1800";
static const char* characteristicUuid = "2803";
static const int   characteristicTransmissionLength = 20; 

// store details about transmission here
struct BleTransmission {
  char* data;
  unsigned int length;
  const char* uuid;
};
BleTransmission bleTransmissionData;
bool bleDataWritten = false; // true if data has been received

BLEService service(serviceUuid); 
BLECharacteristic characteristic(
  characteristicUuid,
  BLEWrite, // writable from client's perspective
  characteristicTransmissionLength
);

BLEPeripheral blePeripheral; 

// When data is sent from the client, it is processed here inside a callback
// it is best to handle the result of this inside the main loop
void onBleCharacteristicWritten(BLECentral& central, BLECharacteristic &characteristic) {
  bleDataWritten = true;
  
  bleTransmissionData.uuid = characteristic.uuid();
  bleTransmissionData.length = characteristic.valueLength();

  // Since we are playing with strings, we must use strcpy
  strcpy(bleTransmissionData.data, (char*) characteristic.value());
}


void setup() {
  blePeripheral.setLocalName(bluetoothDeviceName); 

  blePeripheral.setAdvertisedServiceUuid(service.uuid()); 
  blePeripheral.addAttribute(service);
  blePeripheral.addAttribute(characteristic);

  // trigger onbleCharacteristicWritten when data is sent to the characteristic
  characteristic.setEventHandler(
    BLEWritten,
    onBleCharacteristicWritten
  );
  blePeripheral.begin(); 
}

void loop() {
  // if the bleDataWritten flag has been set, print out the incoming data
  if (bleDataWritten) {
    bleDataWritten = false; // ensure only happens once
    
    Serial.print(bleTransmissionData.length);
    Serial.print(" bytes sent to characteristic ");
    Serial.print(bleTransmissionData.uuid);
    Serial.print(": ");
    Serial.println(bleTransmissionData.data);
  }
}

