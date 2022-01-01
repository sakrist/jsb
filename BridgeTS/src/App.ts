import { JSBinding } from './JSBinding';
import { BridgeTS } from './BridgeTS';
import { TestJSBinding } from './TestJSBinding'

export class App extends JSBinding  {
    public constructor(ptrObject: BigInt) {
        super(ptrObject);

        BridgeTS.add(5, 6).then(function(value:number) { log('add 5 and 6 ' + value) });
    }

    public static getTestJSBinding(objectPtr:BigInt) {
        let object = new TestJSBinding(objectPtr);
        object.setNumber(2);
        log('set value: 2');
        object.getNumber().then(function(value:number) { log('get value: ' + value) });

        object.setNumber2(2, 5);
        log('set value: 2 and 5');
        object.getNumber().then(function(value:number) { log('Now finally done with getting: ' + value) });
      }
}

function log(txt:string) {
    // @ts-ignore
    document.getElementById('msg').innerHTML += txt + '<br>'
}