import { JSBinding } from './JSBinding';
import { BridgeTS } from './BridgeTS';


export class TestJSBinding extends JSBinding {
    public constructor(ptrObject?: BigInt) {
        if (!ptrObject) {
            ptrObject = BridgeTS.getInstance().sync('{ "class" : "TestJSBinding", "function" : "constructor" }') as BigInt;
            super(ptrObject);
        } else {
            super(ptrObject);
        }
    }

    static promises : Map<string, Function> = new Map();
    static _callback(key:string, value:any) {
        var resolve = TestJSBinding.promises.get(key);
        TestJSBinding.promises.delete(key);
        if(resolve) {
            resolve(value);
        }
    }

    public getNumber3() : Promise<number> {
        let callid = BridgeTS.generateCallID(this.ptr);
        var p = new Promise<number>((resolve) => {
            TestJSBinding.promises.set(callid, resolve);
        });
        BridgeTS.getInstance().async('{ "class" : "TestJSBinding", "object" : ' + this.ptr + ', \
        "function" : "getNumber",  "callback" : "TestJSBinding._callback", "cid" : "'+ callid +'"}');
        return p;
    }

    public getNumber() : number {
        return BridgeTS.getInstance().sync('{ "class" : "TestJSBinding", "object" : ' + this.ptr + ', \
        "function" : "getNumber" }');
    }

    public setNumber(index:number) : void {
        BridgeTS.getInstance().async('{ "class" : "TestJSBinding", \
        "object" : ' + this.ptr + ', \
        "function" : "setNumber", \
        "args" : [' + index + ']}');
    }

    public setNumber2(index:number, index2:number) : void {
        BridgeTS.getInstance().async('{ "class" : "TestJSBinding", \
        "object" : ' + this.ptr + ', \
        "function" : "setNumber2", \
        "args" : [' + index + ', ' + index2 + '] }');
    }

    public static randomNumber(arg:number) : number {
        
        let r = BridgeTS.getInstance().sync('{ "class" : "TestJSBinding", \
        "function" : "randomNumber", "args" : [' + arg + ']  }');
        return Number(r);
    }
}

