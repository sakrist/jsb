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

    public async(message:string) {
      // TODO: do for other browsers
      (window as unknown as Window).webkit.messageHandlers.BridgeTS.postMessage(message);
    }

    public sync(message:string) : any {
      try {
        var res = prompt(message);
        // @ts-ignore
        let object = JSON.parse(res);
        return object.r;
      } catch (error) {
        console.log('The native context does not exist yet');
      }
      return undefined;
    }

    // TODO: think of someting more unique, could be date + incremental_value ?
    static generateCallID(v?:BigInt) : string {
      return ((v) ? v : "0") + "_" + Date.now() + "_" + Math.floor(Math.random() * 10000);
    }

    static promises : Map<string, Function> = new Map();
    static _callback(key:string, value:any) {
      var resolve = BridgeTS.promises.get(key);
      BridgeTS.promises.delete(key);
      if(resolve) {
          resolve(value);
      }
    }

    static add(a:number, b:number) : number {
        let r = BridgeTS.getInstance().sync('{ "function" : "add", "args" : [' + a + ', ' + b + '] }');
        return Number(r);
    }
  }