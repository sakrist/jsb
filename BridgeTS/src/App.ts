import { JSBinding } from './JSBinding';
import { TestJSBinding } from './TestJSBinding'

export class App extends JSBinding  {
    public constructor(ptrObject: BigInt) {
        super(ptrObject);
    }

    public static getTestJSBinding(objectPtr:BigInt) {
        let object = new TestJSBinding(objectPtr);
        object.setNumber(2);
        log('setNumber(2)');

        let value = object.getNumber();
        log('getNumber: ' + value);

        object.setNumber2(9, 5);
        log('setNumber2(9, 5)');
        value = object.getNumber();
        log('getNumber ' + value);
      }
}

export function log(txt:string) {
    // @ts-ignore
    document.getElementById('msg').innerHTML += txt + '<br>'
}