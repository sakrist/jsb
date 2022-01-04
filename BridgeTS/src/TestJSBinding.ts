import { JSBinding } from './JSBinding';
import { BridgeTS } from './BridgeTS';


export class TestJSBinding extends JSBinding {
    public constructor(ptrObject?: BigInt) {
        if (!ptrObject) {
            ptrObject = BridgeTS.getInstance().sendMessageSync('{ "class" : "TestJSBinding", "function" : "constructor" }') as BigInt;
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

    public getNumber() : number {
        let r = BridgeTS.getInstance().sendMessageSync('{ "class" : "TestJSBinding", "object" : ' + this.ptr + ', \
        "function" : "getNumber" }');
        return Number(r);
    }

    public setNumber(index:number) : void {
        BridgeTS.getInstance().sendMessage('{ "class" : "TestJSBinding", \
        "object" : ' + this.ptr + ', \
        "function" : "setNumber", \
        "args" : [' + index + ']}');
    }

    public setNumber2(index:number, index2:number) : void {
        BridgeTS.getInstance().sendMessage('{ "class" : "TestJSBinding", \
        "object" : ' + this.ptr + ', \
        "function" : "setNumber2", \
        "args" : [' + index + ', ' + index2 + '] }');
    }

    public static randomNumber(arg:number) : number {
        
        let r = BridgeTS.getInstance().sendMessageSync('{ "class" : "TestJSBinding", \
        "function" : "randomNumber", "args" : [' + arg + ']  }');
        return Number(r);
    }
}

