// Only use one set of include guards
#ifndef LOCALMLENGINE_H
#define LOCALMLENGINE_H

#include <Arduino.h>
#include "PlantTypes.h"

// Debug mode - comment out to disable
// #define DEBUG_ML 1

// Action structure for pump control
struct LocalAction {
    bool shouldWater;
    unsigned int waterDuration; // milliseconds
    uint16_t waterAmount;       // ml (calculated)
    bool isFailsafe;            // emergency watering

    // Constructor with defaults
    LocalAction() : shouldWater(false), waterDuration(0),
               waterAmount(0), isFailsafe(false) {}
};

// Sensor data structure
struct LocalSensorData {
    int16_t moisture;          // 0-1023 analog reading
    int16_t temperature;       // Celsius x10 (store as tenths)
    int16_t humidity;          // Percentage x10 (store as tenths)
    int16_t lightLevel;        // 0-1023 analog reading
    unsigned long lastWatered; // Hours since last watering
    PlantType plantType;
    GrowthStage growthStage;

    // Constructor with defaults
    LocalSensorData() : moisture(0), temperature(25), humidity(50),
                   lightLevel(500), lastWatered(0),
                   plantType(TOMATO), growthStage(VEGETATIVE) {}
};

// Forward declarations
class DecisionTree;
class LookupTable;
class AnomalyDetector;

class LocalMLEngine {
private:
    DecisionTree* irrigationTree;
    LookupTable* plantThresholds;
    AnomalyDetector* sensorMonitor;
    PlantType plantTypes[2];
    GrowthStage growthStages[2];
    unsigned long lastWateringTime[2];
public:
    LocalMLEngine();
    bool begin();
    void setPlantType(int sensorIndex, PlantType type);
    void setGrowthStage(int sensorIndex, GrowthStage stage);
    float predictWaterNeed(const LocalSensorData &data);
    bool detectAnomaly(const LocalSensorData &data);
    LocalAction getImmediateAction(int sensorIndex, const LocalSensorData &data);
    float calculateFeatureScore(const LocalSensorData &data);
    WaterAmount mapToWaterAmount(float prediction);
    unsigned int calculateWaterDuration(WaterAmount amount);
    // void updatePlantThresholds(PlantType type, float moistureThreshold, float tempOptimal, float humidityOptimal); // removed for RAM savings
    void setFailsafeMode(bool enabled);
    bool isTimeToWater(int sensorIndex, unsigned long currentTime);
    float getMoistureThreshold(PlantType type, GrowthStage stage);
    void recordWatering(int sensorIndex, unsigned long timestamp);
    unsigned long getInferenceCount();
    float getAverageInferenceTime();
    void resetStats();
};

#endif // LOCALMLENGINE_H