import { JSBinding } from './JSBinding';
import { BridgeTS } from './BridgeTS';


export class TestJSBinding extends JSBinding {
    public constructor(ptrObject: BigInt) {
        super(ptrObject);
    }

    static promises : Map<string, Function> = new Map();

    public async getNumber() : Promise<number> {

        // TODO: think of someting more unique, could be date + incremental_value ?
        let callid = BridgeTS.generateCallID(this.ptr);

        var p = new Promise<number>((resolve) => {
            TestJSBinding.promises.set(callid, resolve);
        });

        BridgeTS.getInstance().sendMessage('{ "class" : "TestJSBinding", "object" : ' + this.ptr + ', \
        "function" : "getNumber", \
        "callback" : "TestJSBinding.cb_getNumber", \
        "cid" : "'+ callid +'" }');
        return p;
    }

    static cb_getNumber(key:string, value:number) {
        var resolve = TestJSBinding.promises.get(key);
        TestJSBinding.promises.delete(key);
        if(resolve) {
            resolve(value);
        }
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
}

