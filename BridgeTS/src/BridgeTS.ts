// export default
import { IBridgeTS } from "./IBridgeTS"
import { TestJSBinding } from "./TestJSBinding";
import { Window } from "./Window"

export class BridgeTS implements IBridgeTS {

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

    public registerTestJSBinding(objectPtr:BigInt) {
        let object = new TestJSBinding(objectPtr);
        object.getNumber();
        console.log(object);
      }
  }