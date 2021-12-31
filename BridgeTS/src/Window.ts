
import { BridgeTS } from "./BridgeTS"
import { App } from "./App"

export interface Window {
    webkit?: any;
    bridgets:BridgeTS;
    app:App;
}

