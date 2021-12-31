// export default
import { Window } from "./Window"

export class BridgeTS {

    private static instance: BridgeTS;
    private constructor() { }
    public static getInstance(): BridgeTS {
        if (!BridgeTS.instance) {
            BridgeTS.instance = new BridgeTS();
        }
        return BridgeTS.instance;
    }

    public sendMessage(message:String) {
  
      // TODO: do for other browsers
  
      (window as unknown as Window).webkit.messageHandlers.BridgeTS.postMessage(message);
    }
  }