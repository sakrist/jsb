import { JSBinding } from './JSBinding';
import { BridgeTS } from './BridgeTS';

export class TestJSBinding extends JSBinding {
    public constructor(ptrr: BigInt) {
        super(ptrr);
    }

    public async getNumber() {
        BridgeTS.getInstance().sendMessage('{ "object" : ' + this.ptr + ', "op" : "getNumber"}');
        return 0;
    }

    public setNumber(index:number) : void {
        BridgeTS.getInstance().sendMessage('{ "object" : ' + this.ptr + ', "op" : "setNumber", "value" : ' + index + '}');
    }
}