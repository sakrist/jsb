
export class JSBinding {
    public constructor(ptrObject: BigInt) {
        this.ptr = ptrObject;
    }
    protected ptr: BigInt;

    static _tmpPtrID:number = 0;
    static getTemporaryObjectId() : number {
        JSBinding._tmpPtrID++;
        return JSBinding._tmpPtrID;
    }
}