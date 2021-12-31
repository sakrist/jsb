
export class JSBinding {
    public constructor(ptrObject: BigInt) {
        this.ptr = ptrObject;
    }
    protected ptr: BigInt;
}