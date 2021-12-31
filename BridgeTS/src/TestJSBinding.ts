import { JSBinding } from './JSBinding';
import { BridgeTS } from './BridgeTS';


export class TestJSBinding extends JSBinding {
    public constructor(ptrObject: BigInt) {
        super(ptrObject);
    }

    static promises : Map<string, Function> = new Map();

    public async getNumber() : Promise<number> {

        let callid = Date.now() + "_" + Math.floor(Math.random() * 10000);

        var p = new Promise<number>((resolve) => {
            TestJSBinding.promises.set(callid, resolve);
        });

        BridgeTS.getInstance().sendMessage('{ "object" : ' + this.ptr + ', "function" : "getNumber", \
        "callback" : "TestJSBinding._getNumber", "callback_id" : "'+ callid +'" }');
        return p;
    }

    static _getNumber(key:string, value:number) {
        var resolve = TestJSBinding.promises.get(key);
        if(resolve) {
            resolve(value);
        }
    }

    public setNumber(index:number) : void {
        BridgeTS.getInstance().sendMessage('{ "object" : ' + this.ptr + ', "function" : "setNumber", "value" : ' + index + '}');
    }
}

