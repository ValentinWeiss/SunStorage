import { batteryConfiguration } from "./batteryConfiguration";
import { chargerConfiguration } from "./chargerConfiguration";
import { displayConfiguration } from "./displayConfiguration";
import { nightShutdownConfiguration } from "./nightShutdownConfiguration";
import { positioningConfiguration } from "./positioningConfiguration";

export interface ESPStateData {
    charger: chargerConfiguration;
    positioning: positioningConfiguration;
    battery: batteryConfiguration;
    display: displayConfiguration;
    nightShutdown: nightShutdownConfiguration;
}
