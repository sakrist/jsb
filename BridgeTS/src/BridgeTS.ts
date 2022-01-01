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

    // TODO: think of someting more unique, could be date + incremental_value ?
    static generateCallID(v:BigInt) : string {
      return v + "_" + Date.now() + "_" + Math.floor(Math.random() * 10000);
    }

    static promises : Map<string, Function> = new Map();

    static async add(a:number, b:number) : Promise<number> {

        let callid = BridgeTS.generateCallID(BigInt(0));

        var p = new Promise<number>((resolve) => {
          BridgeTS.promises.set(callid, resolve);
        });

        BridgeTS.getInstance().sendMessage('{ "function" : "add", \
        "args" : [' + a + ', ' + b + '], \
        "callback" : "BridgeTS.cb_add", "cid" : "'+ callid +'" }');
        return p;
    }

    static cb_add(key:string, value:number) {
        var resolve = BridgeTS.promises.get(key);
        BridgeTS.promises.delete(key);
        if(resolve) {
            resolve(value);
        }
    }
  }